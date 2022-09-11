#include <pico/stdlib.h>
#include <pico/multicore.h>
#include "config.h"
#include "vga.h"

int main()
{
    set_sys_clock_khz(CONFIG_SYSCLOCK*1000, true);
    vga_init();
    render_init();
    render_loop();
}