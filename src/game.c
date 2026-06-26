#include "game.h"

#include "input.h"
#include "render.h"
#include "tetris.h"

#include <sys.h>

static TetrisState state;
static TetrisInput input;

void game_init(void)
{
    SYS_disableInts();
    input_init();
    render_init();
    SYS_enableInts();
    tetris_init(&state);
}

void game_update(void)
{
    input_read(&input);
    tetris_update(&state, &input);
    render_draw(&state);
}

u16 game_debug_buttons(void)
{
    return input.raw_buttons;
}
