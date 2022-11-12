#ifndef NEVIL_SBC_VGA_BUS
#define NEVIL_SBC_VGA_BUS

struct reg_state {
    uint8_t r_status;

    uint8_t r_cntl;

    uint8_t r_addr_lo;
    uint8_t r_addr_hi;
};

void bus_init();
void bus_loop();

#endif