#include "game.h"
#include "hardware.h"

int main(void)
{
    hw_init();
    game_init();

    for (;;) {
        game_tick();
    }

    return 0;
}
