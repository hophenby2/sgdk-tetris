#include "game.h"

#include <genesis.h>

int main(bool hardReset)
{
    game_init();

    while (TRUE)
    {
        game_update();
        SYS_doVBlankProcess();
    }

    return 0;
}
