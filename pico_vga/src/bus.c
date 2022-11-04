#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/sync.h>
#include "config.h"
#include "bus.pio.h"
#include "bus.h"

enum {
    BUS_READ_SM = 0,
    BUS_WRITE_SM = 1,
};

static void bus_read_setup(PIO pio, uint sm)
{
    uint program_offset = pio_add_program(pio, &bus_read_program);
    pio_sm_claim(pio, sm);

    pio_sm_config c = bus_read_program_get_default_config(program_offset);
    
    for (int i = D0; i <= DIR; i++)
        pio_gpio_init(pio, i);

    for (int i = D0; i < CS; i++)
        pio_set_pulls(i, false, true);

    // set initial pindirs: D0 - D7 are (also) output
    pio_sm_set_consecutive_pindirs(pio, sm, D0, 8, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CE, 1, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, RW, 1, true);
   
    // set initial pindirs: A0 - A4
    pio_sm_set_consecutive_pindirs(pio, sm, A0, 3, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, IRQ, 1, true);

    // set CE pindir
    pio_sm_set_consecutive_pindirs(pio, sm, DIR, 1, true);

    // pio 'in' inins: inputs at at the first data bit (D0)
    sm_config_set_in_pins(&sm, D0);

    // set out pins    
    sm_config_set_out_pins(&sm, D0, 8);

    // set DIR as set pin. 
    sm_config_set_set_pins(&sm, DIR, 1);

    // Reading from rxFIFO: shift to left, autopull disabled
    sm_config_set_in_shift(&sm, false, false, 32);

    // Writing from rxFIFO: shift to right, autopull disabled
    sm_config_set_out_shift(&sm, true, false, 32);

    pio_sm_init(pio, sm, program_offset, &sm);
    
    pio_sm_set_enabled(pio, sm, true);
}

static void bus_write_setup(PIO pio, uint sm)
{
    uint program_offset = pio_add_program(pio, &bus_write_program);
    pio_sm_claim(pio, sm);

    pio_sm_config c = bus_write_program_get_default_config(program_offset);
    
    for (int i = D0; i <= DIR; i++)
        pio_gpio_init(pio, i);

    for (int i = D0; i < CS; i++)
        pio_set_pulls(i, false, true);

    // set initial pindirs: D0 - D7 are (also) output
    pio_sm_set_consecutive_pindirs(pio, sm, D0, 8, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CE, 1, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, RW, 1, true);
   
    // set initial pindirs: A0 - A4
    pio_sm_set_consecutive_pindirs(pio, sm, A0, 3, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, IRQ, 1, true);

    // set CE pindir
    pio_sm_set_consecutive_pindirs(pio, sm, DIR, 1, true);

    // pio 'in' inins: inputs at at the first data bit (D0)
    sm_config_set_in_pins(&sm, D0);

    // set out pins    
    sm_config_set_out_pins(&sm, D0, 8);

    // set DIR as set pin. 
    sm_config_set_set_pins(&sm, DIR, 1);

    // Reading from rxFIFO: shift to left, autopull disabled
    sm_config_set_in_shift(&sm, false, false, 32);

    // Writing from rxFIFO: shift to right, autopull disabled
    sm_config_set_out_shift(&sm, true, false, 32);
}

void bus_run()
{
    uint32_t addr_data;
    uint8_t data;

    uint16_t address;
    uint8_t current_pc;

    pio_enable_sm_mask_in_sync(CONFIG_BUS_PIO, (1 << BUS_READ_SM) | (1 << BUS_WRITE_SM));

    uint bitoffs;
    const uint32_t mask = PIO_FLEVEL_RX0_BITS >> PIO_FLEVEL_RX0_LSB;
    while (1)
    {
        bitoffs = PIO_FLEVEL_RX0_LSB + BUS_READ_SM * (PIO_FLEVEL_RX1_LSB - PIO_FLEVEL_RX0_LSB);
        if ((CONFIG_BUS_PIO->flevel >> bitoffs) & mask > 0)
        {
            addr_data = CONFIG_BUS_PIO->rxf[BUS_READ_SM];
            address = addr_data >> 10;


        }

        bitoffs = PIO_FLEVEL_RX0_LSB + BUS_WRITE_SM * (PIO_FLEVEL_RX1_LSB - PIO_FLEVEL_RX0_LSB);
        if ((CONFIG_BUS_PIO->flevel >> bitoffs) & mask > 0)
        {
            addr_data = CONFIG_BUS_PIO->rxf[BUS_READ_SM];
            address = addr_data >> 10;


        }
    }
}

void bus_init()
{
    bus_read_setup(CONFIG_BUS_PIO, BUS_READ_SM);
    bus_write_setup(CONFIG_BUS_PIO, BUS_WRITE_SM);

    // Fill regs and data ect.
     
}