#ifndef NEVIL_SBC_VGA_CONFIG
#define NEVIL_SBC_VGA_CONFIG

#define CONFIG_SYSCLOCK 126.0 // MHz 

// Sync Pins
#define CONFIG_PIN_HSYNC 15
#define CONFIG_PIN_VSYNC 16
#define CONFIG_PIN_RGB_BASE 20 // 3 pins

// Vga resources
#define CONFIG_VGA_PIO pio0
#define CONFIG_VGA_SPINLOCK_ID 31

// Bus resources
#define CONFIG_BUS_PIO pio1

#endif