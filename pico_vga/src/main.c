#include <pico/stdlib.h>
#include <pico/multicore.h>
#include "config.h"
#include "vga.h"
#include "bus.h"

static void core1_main()
{
    vga_init();
    render_init();
    render_loop();
}

int main()
{
    set_sys_clock_khz(CONFIG_SYSCLOCK*1000, true);
    //vga_init();
    //render_init();
    //render_loop();

    multicore_launch_core1(core1_main);

    bus_init();
    bus_loop(); 
}