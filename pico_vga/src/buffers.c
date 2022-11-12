#include "buffers.h"

// The currently programmed character generator ROM for text mode
uint8_t character_rom[256*8];

// Shadow copy of Apple memory locations $0400 - $0BFF
uint8_t text_memory[1024*2];
