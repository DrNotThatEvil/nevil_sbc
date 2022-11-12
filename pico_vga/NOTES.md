# PCB Design notes.
Data bus: GPIO0  - GPIO7
Addr bus: GPIO10 - GPIO12
VGA bus:  GPIO17 - GPIO28
    R: GPIO17 - GPIO19
    G: GPIO20 - GPIO22
    B: GPIO26 - GPIO28


Control signals:
CS: GPIO8
R/W: GPIO9
IRQ: GPIO13
DIR: GPIO14

HSYNC: GPIO15
VSYNC: GPIO16

# Registry design

0x00 - STATUS
    0b0000 0000 
              |
              + -- Addr state

0x01 - CONTROL
    0b0000 0000
              |
              + -- Increment addr on write to data (0 add 1, 1 add line)

0x02 - Res
0x03 - Res
0x04 - Res
0x05 - Res
0x06 - Address
0x07 - Data


