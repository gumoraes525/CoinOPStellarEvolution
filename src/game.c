#include "game.h"
#include "hardware.h"

#define FLAG_POPIII      0x01
#define FLAG_WR          0x02
#define FLAG_TZO         0x04
#define FLAG_CONTACT     0x08
#define FLAG_BLUE_STRAG  0x10
#define FLAG_MAGNETAR    0x20

#define STATE_ATTRACT       0
#define STATE_NEW_STAR      1
#define STATE_INTERVENTION  2
#define STATE_EVOLVE        3
#define STATE_SCORE         4

static Star current_star;
static Star high_scores[5];
static uint32_t rng_state = 0x837219u;
static uint8_t game_state;
static uint8_t selected_intervention;
static uint8_t evolve_step;
static uint8_t last_inputs;

static uint16_t rand16(void)
{
    rng_state = (rng_state * 1103515245u) + 12345u;
    return (uint16_t)(rng_state >> 16);
}

static uint16_t range16(uint16_t min, uint16_t max)
{
    return (uint16_t)(min + (rand16() % (uint16_t)(max - min + 1u)));
}

static void print_star_class(void)
{
    if (current_star.mass_centi_solar < 45u) {
        hw_print(2, 3, "RED DWARF CANDIDATE");
    } else if (current_star.mass_centi_solar > 3000u && current_star.metallicity_ppm < 10u) {
        hw_print(2, 3, "MASSIVE POP III CANDIDATE");
    } else if (current_star.mass_centi_solar > 800u) {
        hw_print(2, 3, "MASSIVE STAR CANDIDATE");
    } else {
        hw_print(2, 3, "MAIN SEQUENCE CANDIDATE");
    }
}

static void generate_star(void)
{
    current_star.id = ((uint32_t)rand16() << 16) | rand16();
    current_star.mass_centi_solar = range16(8u, 12000u);
    current_star.metallicity_ppm = range16(1u, 30000u);
    current_star.rotation_kms = range16(0u, 520u);
    current_star.magnetic_gauss = range16(1u, 60000u);
    current_star.has_binary = (uint8_t)((rand16() & 7u) == 0u);
    current_star.interventions = 3u;
    current_star.score = 0u;
    current_star.flags = 0u;

    if (current_star.metallicity_ppm < 15u && current_star.mass_centi_solar > 1000u) {
        current_star.flags |= FLAG_POPIII;
    }
    if (current_star.has_binary && (rand16() % 64u) == 0u) {
        current_star.flags |= FLAG_CONTACT;
    }
    if (current_star.has_binary && current_star.mass_centi_solar > 700u && (rand16() % 96u) == 0u) {
        current_star.flags |= FLAG_TZO;
    }
}

static void apply_intervention(void)
{
    if (current_star.interventions == 0u) {
        return;
    }

    switch (selected_intervention) {
    case 0:
        current_star.mass_centi_solar += 75u;
        hw_print(2, 20, "MASS ADDED       ");
        break;
    case 1:
        if (current_star.mass_centi_solar > 85u) {
            current_star.mass_centi_solar -= 60u;
        }
        hw_print(2, 20, "MASS REMOVED     ");
        break;
    case 2:
        current_star.rotation_kms += 45u;
        hw_print(2, 20, "ROTATION BOOSTED ");
        break;
    case 3:
        current_star.has_binary = 1u;
        current_star.mass_centi_solar += 120u;
        hw_print(2, 20, "BINARY TRANSFER  ");
        break;
    default:
        current_star.metallicity_ppm += 250u;
        hw_print(2, 20, "METALS INCREASED ");
        break;
    }

    current_star.interventions--;
    hw_play_event(1u);
}

static void draw_intervention(void)
{
    hw_print(2, 12, "SELECT INTERVENTION");
    hw_print(2, 14, selected_intervention == 0u ? "> ADD MASS" : "  ADD MASS");
    hw_print(2, 15, selected_intervention == 1u ? "> REMOVE MASS" : "  REMOVE MASS");
    hw_print(2, 16, selected_intervention == 2u ? "> SPIN UP" : "  SPIN UP");
    hw_print(2, 17, selected_intervention == 3u ? "> BINARY TRANSFER" : "  BINARY TRANSFER");
    hw_print(2, 18, selected_intervention == 4u ? "> ADD METALS" : "  ADD METALS");
    hw_print(2, 22, "CHANCES LEFT:");
    hw_print_number(16, 22, current_star.interventions);
}

static void score_event(const char *label, uint32_t points)
{
    hw_print(2, (uint8_t)(6u + evolve_step), label);
    current_star.score += points;
    hw_play_event((uint8_t)(2u + evolve_step));
}

static void evolve_star(void)
{
    switch (evolve_step) {
    case 0:
        score_event("HYDROGEN IGNITION +100", 100u);
        break;
    case 1:
        if (current_star.mass_centi_solar > 50u) {
            score_event("HELIUM IGNITION +500", 500u);
        }
        break;
    case 2:
        if (current_star.mass_centi_solar > 800u) {
            score_event("CARBON BURNING +2000", 2000u);
        }
        break;
    case 3:
        if (current_star.mass_centi_solar > 2500u && current_star.metallicity_ppm > 900u) {
            current_star.flags |= FLAG_WR;
            score_event("WOLF-RAYET BONUS +15000", 15000u);
        }
        break;
    case 4:
        if (current_star.rotation_kms > 420u && current_star.magnetic_gauss > 45000u) {
            current_star.flags |= FLAG_MAGNETAR;
            score_event("MAGNETAR JACKPOT +30000", 30000u);
        }
        break;
    case 5:
        if (current_star.mass_centi_solar > 6500u && current_star.metallicity_ppm < 200u) {
            score_event("PAIR-INSTABILITY +100000", 100000u);
        } else if (current_star.mass_centi_solar > 2500u) {
            score_event("BLACK HOLE +50000", 50000u);
        } else if (current_star.mass_centi_solar > 800u) {
            score_event("SUPERNOVA +10000", 10000u);
            score_event("NEUTRON STAR +20000", 20000u);
        } else if (current_star.mass_centi_solar < 45u) {
            score_event("TRILLION YEAR DWARF +5000", 5000u);
        } else {
            score_event("WHITE DWARF +1000", 1000u);
        }
        break;
    default:
        game_state = STATE_SCORE;
        return;
    }
    evolve_step++;
}

static void update_high_scores(void)
{
    uint8_t i;
    for (i = 0u; i < 5u; ++i) {
        if (current_star.score > high_scores[i].score) {
            uint8_t j;
            for (j = 4u; j > i; --j) {
                high_scores[j] = high_scores[(uint8_t)(j - 1u)];
            }
            high_scores[i] = current_star;
            return;
        }
    }
}

void game_init(void)
{
    uint8_t i;
    game_state = STATE_ATTRACT;
    selected_intervention = 0u;
    evolve_step = 0u;
    last_inputs = 0u;
    for (i = 0u; i < 5u; ++i) {
        high_scores[i].score = 0u;
    }
    hw_clear_screen();
}

void game_tick(void)
{
    uint8_t inputs = hw_read_inputs();
    uint8_t pressed = (uint8_t)(inputs & (uint8_t)~last_inputs);
    last_inputs = inputs;

    if (game_state == STATE_ATTRACT) {
        hw_print(4, 4, "STELLAR EVOLUTION");
        hw_print(5, 8, "INSERT COIN");
        hw_print(3, 12, "ONE CREDIT = ONE STAR");
        if ((pressed & INPUT_COIN) != 0u) {
            game_state = STATE_NEW_STAR;
        }
        return;
    }

    if (game_state == STATE_NEW_STAR) {
        hw_clear_screen();
        generate_star();
        hw_print(2, 1, "STAR #");
        hw_print_number(9, 1, current_star.id);
        print_star_class();
        hw_print(2, 5, "MASS CX SOL:");
        hw_print_number(16, 5, current_star.mass_centi_solar);
        hw_print(2, 6, "METAL PPM:");
        hw_print_number(16, 6, current_star.metallicity_ppm);
        hw_print(2, 7, "ROT KM/S:");
        hw_print_number(16, 7, current_star.rotation_kms);
        selected_intervention = 0u;
        game_state = STATE_INTERVENTION;
        return;
    }

    if (game_state == STATE_INTERVENTION) {
        if ((pressed & INPUT_LEFT) != 0u) {
            selected_intervention = (selected_intervention == 0u) ? 4u : (uint8_t)(selected_intervention - 1u);
        }
        if ((pressed & INPUT_RIGHT) != 0u) {
            selected_intervention = (uint8_t)((selected_intervention + 1u) % 5u);
        }
        if ((pressed & INPUT_BUTTON) != 0u) {
            apply_intervention();
        }
        draw_intervention();
        if ((pressed & INPUT_START) != 0u || current_star.interventions == 0u) {
            hw_clear_screen();
            hw_print(2, 2, "EVOLUTION RUN");
            evolve_step = 0u;
            game_state = STATE_EVOLVE;
        }
        return;
    }

    if (game_state == STATE_EVOLVE) {
        evolve_star();
        return;
    }

    if (game_state == STATE_SCORE) {
        update_high_scores();
        hw_print(2, 18, "FINAL SCORE");
        hw_print_number(15, 18, current_star.score);
        hw_print(2, 22, "COIN FOR NEXT STAR");
        if ((pressed & INPUT_COIN) != 0u) {
            game_state = STATE_NEW_STAR;
        }
    }
}
