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

    pio_sm_config c = bus_read_program_get_default(program_offset);
    
    // set initial pindirs: D0 - D7 are (also) output
    pio_sm_set_consecutive_pindirs(pio, sm, D0, 8, true);
    
    // set initial pindirs: A0 - A4
    pio_sm_set_consecutive_pindirs(pio, sm, A0, 5, true);

    // set RWB pindir
    pio_sm_set_consecutive_pindirs(pio, sm, RWB, 1, true);

    // set CE pindir
    pio_sm_set_consecutive_pindirs(pio, sm, CE, 1, true);

    // pio 'in' inins: inputs at at the first data bit (D0)
    sm_config_set_in_pins(&sm, D0);

    //
    sm_config_set_in

    // pio 'out'
    sm_config_set_out_pins(&sm, D0, 8);

    //
    
}