#ifndef NEVIL_SBC_VGA_CONFIG
#define NEVIL_SBC_VGA_CONFIG

#define CONFIG_SYSCLOCK 126.0 // MHz 

// Sync Pins
#define CONFIG_PIN_HSYNC 28
#define CONFIG_PIN_VSYNC 27
#define CONFIG_PIN_RGB_BASE 9 // 9 pins

// Other resources
#define CONFIG_VGA_PIO pio0
#define CONFIG_VGA_SPINLOCK_ID 31

#endif