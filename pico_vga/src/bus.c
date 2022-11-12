#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/dma.h>
#include <hardware/irq.h>
#include <hardware/pio.h>
#include <hardware/sync.h>
#include "config.h"
#include "buffers.h"
#include "bus.pio.h"
#include "bus.h"

enum {
    BUS_READ_SM = 0,
    BUS_WRITE_SM = 1,
};

enum {
    CTRL_ADDR_INC_32 = 1 << 0,
    CNTL_SOMETHING = 1 << 1,
};

static void bus_read_setup(PIO pio, uint sm)
{
    uint program_offset = pio_add_program(pio, &bus_read_program);
    pio_sm_claim(pio, sm);

    pio_sm_config c = bus_read_program_get_default_config(program_offset);
    
    for (int i = D0; i <= DIR; i++)
        pio_gpio_init(pio, i);

    for (int i = D0; i < CE; i++)
        gpio_set_pulls(i, false, true);

    // set initial pindirs: D0 - D7 are (also) output
    pio_sm_set_consecutive_pindirs(pio, sm, D0, 8, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CE, 1, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, RW, 1, true);
   
    // set initial pindirs: A0 - A4
    pio_sm_set_consecutive_pindirs(pio, sm, A0, 3, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CIRQ, 1, true);

    // set CE pindir
    pio_sm_set_consecutive_pindirs(pio, sm, DIR, 1, true);

    // pio 'in' inins: inputs at at the first data bit (D0)
    sm_config_set_in_pins(&c, D0);

    // set out pins    
    sm_config_set_out_pins(&c, D0, 8);

    // set DIR as set pin. 
    sm_config_set_set_pins(&c, DIR, 1);

    // Reading from rxFIFO: shift to left, autopull disabled
    sm_config_set_in_shift(&c, false, false, 32);

    // Writing from rxFIFO: shift to right, autopull disabled
    sm_config_set_out_shift(&c, true, false, 32);

    pio_sm_init(pio, sm, program_offset, &c);
    
    pio_sm_set_enabled(pio, sm, true);
}

static void bus_write_setup(PIO pio, uint sm)
{
    uint program_offset = pio_add_program(pio, &bus_write_program);
    pio_sm_claim(pio, sm);

    pio_sm_config c = bus_write_program_get_default_config(program_offset);
    
    for (int i = D0; i <= DIR; i++)
        pio_gpio_init(pio, i);

    for (int i = D0; i < CE; i++)
        gpio_set_pulls(i, false, true);

    // set initial pindirs: D0 - D7 are (also) output
    pio_sm_set_consecutive_pindirs(pio, sm, D0, 8, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CE, 1, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, RW, 1, true);
   
    // set initial pindirs: A0 - A4
    pio_sm_set_consecutive_pindirs(pio, sm, A0, 3, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CIRQ, 1, true);

    // set CE pindir
    pio_sm_set_consecutive_pindirs(pio, sm, DIR, 1, true);

    // pio 'in' inins: inputs at at the first data bit (D0)
    sm_config_set_in_pins(&c, D0);

    // set out pins    
    sm_config_set_out_pins(&c, D0, 8);

    // set DIR as set pin. 
    sm_config_set_set_pins(&c, DIR, 1);

    // Reading from rxFIFO: shift to left, autopull disabled
    sm_config_set_in_shift(&c, false, false, 32);

    // Writing from rxFIFO: shift to right, autopull disabled
    sm_config_set_out_shift(&c, true, false, 32);

    pio_sm_init(pio, sm, program_offset, &c);

    pio_sm_set_enabled(pio, sm, true);
}

void bus_loop()
{
    uint32_t addr_data;
    uint8_t data;

    uint16_t address;
    uint8_t current_pc;

    pio_enable_sm_mask_in_sync(CONFIG_BUS_PIO, (1 << BUS_READ_SM) | (1 << BUS_WRITE_SM));

    uint bitoffs;
    const uint32_t mask = PIO_FLEVEL_RX0_BITS >> PIO_FLEVEL_RX0_LSB;

    struct reg_state _reg_state = {
        .r_status = 0,
        .r_cntl = 0,
        .r_addr_lo = 0,
        .r_addr_hi = 0
    };

    printf("starting loop..\n");
    
    while (1)
    {
        bitoffs = PIO_FLEVEL_RX0_LSB + BUS_READ_SM * (PIO_FLEVEL_RX1_LSB - PIO_FLEVEL_RX0_LSB);
        if ((CONFIG_BUS_PIO->flevel >> bitoffs) & mask > 0)
        {
            addr_data = CONFIG_BUS_PIO->rxf[BUS_READ_SM];
            address = addr_data >> 10;

            printf("Reading address %d\n", address);

            CONFIG_BUS_PIO->txf[BUS_READ_SM] = 0;
        }

        bitoffs = PIO_FLEVEL_RX0_LSB + BUS_WRITE_SM * (PIO_FLEVEL_RX1_LSB - PIO_FLEVEL_RX0_LSB);
        if ((CONFIG_BUS_PIO->flevel >> bitoffs) & mask > 0)
        {
            addr_data = CONFIG_BUS_PIO->rxf[BUS_WRITE_SM];
            data = addr_data & 0xFF;
            address = addr_data >> 10;

            if(address == 1) {
                _reg_state.r_cntl = data;
            }

            if(address == 6) {
                if(_reg_state.r_status & 1 > 0) {
                    _reg_state.r_addr_hi = data;
                } else { 
                    _reg_state.r_addr_lo = data;
                }

                _reg_state.r_status = (_reg_state.r_status ^ 1);
            }

            if(address == 7) {
                uint_fast16_t t_addr = (uint_fast16_t)_reg_state.r_addr_hi << 8 | _reg_state.r_addr_lo;
                text_memory[(0x7FF & t_addr)] = data;
                
                if(_reg_state.r_cntl & CTRL_ADDR_INC_32) {
                    t_addr += ((0x1 & 0x7) << 7) + (((0x1 >> 3) & 0x3) * 40);
                } else {
                    t_addr = t_addr + 1;
                }

                _reg_state.r_addr_hi = (uint_fast8_t)((t_addr & 0xFF00) >> 8);
                _reg_state.r_addr_lo = (uint_fast8_t)((t_addr & 0x00FF));

            }

            printf("Writing %d to addr %d\n", data, address);
        }
    }
}

void bus_init()
{
    bus_read_setup(CONFIG_BUS_PIO, BUS_READ_SM);
    bus_write_setup(CONFIG_BUS_PIO, BUS_WRITE_SM);

    // Fill regs and data ect.
    stdio_init_all();


     
}