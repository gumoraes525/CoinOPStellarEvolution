#ifndef STELLAR_HARDWARE_H
#define STELLAR_HARDWARE_H

#include <stdint.h>

#define INPUT_COIN   0x01
#define INPUT_START  0x02
#define INPUT_LEFT   0x04
#define INPUT_RIGHT  0x08
#define INPUT_BUTTON 0x10

void hw_init(void);
uint8_t hw_read_inputs(void);
void hw_clear_screen(void);
void hw_print(uint8_t x, uint8_t y, const char *text);
void hw_print_number(uint8_t x, uint8_t y, uint32_t value);
void hw_play_event(uint8_t event_id);

#endif
