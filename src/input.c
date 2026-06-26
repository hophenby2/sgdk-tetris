#include "input.h"

#include <genesis.h>

#define HORIZONTAL_DAS_FRAMES 10
#define HORIZONTAL_ARR_FRAMES 3

static u16 previous_buttons;
static u8 left_hold_frames;
static u8 right_hold_frames;

static u16 read_all_buttons(void)
{
    return JOY_readJoypad(JOY_1) | JOY_readJoypad(JOY_2);
}

static u8 horizontal_repeat(u8 held, u8 pressed, u8 *hold_frames)
{
    if (!held)
    {
        *hold_frames = 0;
        return 0;
    }

    if (pressed)
    {
        *hold_frames = 1;
        return 1;
    }

    if (*hold_frames < 255)
    {
        (*hold_frames)++;
    }

    if (*hold_frames < HORIZONTAL_DAS_FRAMES)
    {
        return 0;
    }

    return (((*hold_frames - HORIZONTAL_DAS_FRAMES) % HORIZONTAL_ARR_FRAMES) == 0);
}

void input_init(void)
{
    JOY_init();
    JOY_setSupport(PORT_1, JOY_SUPPORT_3BTN);
    JOY_setSupport(PORT_2, JOY_SUPPORT_3BTN);
    JOY_update();
    previous_buttons = read_all_buttons();
    left_hold_frames = 0;
    right_hold_frames = 0;
}

void input_read(TetrisInput *input)
{
    u16 buttons;
    u16 pressed;
    u8 left_held;
    u8 right_held;

    JOY_update();
    buttons = read_all_buttons();
    pressed = buttons & ~previous_buttons;
    left_held = (buttons & BUTTON_LEFT) != 0;
    right_held = (buttons & BUTTON_RIGHT) != 0;

    if (left_held && right_held)
    {
        input->left_pressed = 0;
        input->right_pressed = 0;
        left_hold_frames = 0;
        right_hold_frames = 0;
    }
    else
    {
        input->left_pressed = horizontal_repeat(left_held, (pressed & BUTTON_LEFT) != 0, &left_hold_frames);
        input->right_pressed = horizontal_repeat(right_held, (pressed & BUTTON_RIGHT) != 0, &right_hold_frames);
    }

    input->down_held = (buttons & BUTTON_DOWN) != 0;
    input->rotate_pressed = (pressed & (BUTTON_UP | BUTTON_A | BUTTON_B | BUTTON_C)) != 0;
    input->hard_drop_pressed = 0;
    input->start_pressed = (pressed & BUTTON_START) != 0;
    input->raw_buttons = buttons;

    previous_buttons = buttons;
}
