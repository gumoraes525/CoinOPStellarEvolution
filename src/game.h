#ifndef STELLAR_GAME_H
#define STELLAR_GAME_H

#include <stdint.h>

typedef struct Star {
    uint32_t id;
    uint16_t mass_centi_solar;
    uint16_t metallicity_ppm;
    uint16_t rotation_kms;
    uint16_t magnetic_gauss;
    uint8_t has_binary;
    uint8_t interventions;
    uint32_t score;
    uint8_t flags;
} Star;

void game_init(void);
void game_tick(void);

#endif
