#ifndef TETRIS_H
#define TETRIS_H

#include <types.h>

#define TETRIS_BOARD_WIDTH 10
#define TETRIS_BOARD_HEIGHT 12
#define TETRIS_VISIBLE_HEIGHT 12
#define TETRIS_PIECE_CELLS 4

#define TETRIS_EVENT_MOVE      0x0001
#define TETRIS_EVENT_ROTATE    0x0002
#define TETRIS_EVENT_SOFT_DROP 0x0004
#define TETRIS_EVENT_HARD_DROP 0x0008
#define TETRIS_EVENT_LOCK      0x0010
#define TETRIS_EVENT_LINE      0x0020
#define TETRIS_EVENT_GAME_OVER 0x0040
#define TETRIS_EVENT_START     0x0080

typedef struct TetrisInput
{
    u8 left_pressed;
    u8 right_pressed;
    u8 down_held;
    u8 rotate_pressed;
    u8 hard_drop_pressed;
    u8 start_pressed;
    u16 raw_buttons;
} TetrisInput;

typedef struct TetrisState
{
    u8 board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH];
    u8 active_piece;
    u8 next_piece;
    u8 rotation;
    s8 piece_x;
    s8 piece_y;
    u32 score;
    u16 lines;
    u8 level;
    u8 game_over;
    u8 started;
    u8 paused;
    u8 drop_timer;
    u8 sequence_index;
    u16 events;
    u8 bag[7];
    u8 bag_index;
    u16 rng_state;
} TetrisState;

void tetris_init(TetrisState *state);
void tetris_update(TetrisState *state, const TetrisInput *input);
u8 tetris_cell_at(const TetrisState *state, s8 x, s8 y);
u8 tetris_is_active_cell(const TetrisState *state, s8 x, s8 y);
void tetris_piece_cell(u8 piece, u8 rotation, u8 index, s8 *x, s8 *y);

#endif
