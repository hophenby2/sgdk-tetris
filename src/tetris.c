#include "tetris.h"

#define DROP_FRAMES_BASE 45

#define SCORE_SINGLE 100
#define SCORE_DOUBLE 300
#define SCORE_TRIPLE 500
#define SCORE_TETRIS 800
#define SCORE_SOFT_DROP 1
#define SCORE_HARD_DROP 2

typedef struct PieceCell
{
    s8 x;
    s8 y;
} PieceCell;

static const PieceCell piece_shapes[7][4][TETRIS_PIECE_CELLS] = {
    /* I */
    {
        {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
        {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
        {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
        {{1, 0}, {1, 1}, {1, 2}, {1, 3}}
    },
    /* O */
    {
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {2, 1}}
    },
    /* T */
    {
        {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
        {{1, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    /* S */
    {
        {{1, 0}, {2, 0}, {0, 1}, {1, 1}},
        {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
        {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
    },
    /* Z */
    {
        {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
        {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
        {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
    },
    /* J */
    {
        {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
        {{1, 0}, {1, 1}, {0, 2}, {1, 2}}
    },
    /* L */
    {
        {{2, 0}, {0, 1}, {1, 1}, {2, 1}},
        {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
        {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {1, 2}}
    }
};

static u8 drop_frames(const TetrisState *state)
{
    u8 speedup = state->level * 4;

    if (speedup >= 35)
    {
        return 10;
    }

    return DROP_FRAMES_BASE - speedup;
}

static u16 rng_next(TetrisState *state)
{
    state->rng_state = (u16)((state->rng_state * 25173) + 13849);
    return state->rng_state;
}

static void refill_bag(TetrisState *state)
{
    s8 i;

    for (i = 0; i < 7; i++)
    {
        state->bag[(u8)i] = (u8)i;
    }

    for (i = 6; i > 0; i--)
    {
        u8 j = (u8)(rng_next(state) % (u16)(i + 1));
        u8 tmp = state->bag[(u8)i];
        state->bag[(u8)i] = state->bag[j];
        state->bag[j] = tmp;
    }

    state->bag_index = 0;
}

static u8 next_piece(TetrisState *state)
{
    if (state->bag_index >= 7)
    {
        refill_bag(state);
    }

    return state->bag[state->bag_index++];
}

static u8 collides(const TetrisState *state, s8 x, s8 y, u8 rotation)
{
    u8 i;
    const PieceCell *shape = piece_shapes[state->active_piece][rotation & 3];

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        s8 cell_x = x + shape[i].x;
        s8 cell_y = y + shape[i].y;

        if ((cell_x < 0) || (cell_x >= TETRIS_BOARD_WIDTH) || (cell_y >= TETRIS_BOARD_HEIGHT))
        {
            return 1;
        }

        if ((cell_y >= 0) && state->board[(u8)cell_y][(u8)cell_x])
        {
            return 1;
        }
    }

    return 0;
}

static void spawn_piece(TetrisState *state)
{
    state->active_piece = state->next_piece;
    state->next_piece = next_piece(state);
    state->rotation = 0;
    state->piece_x = 3;
    state->piece_y = 0;

    if (collides(state, state->piece_x, state->piece_y, state->rotation))
    {
        state->game_over = 1;
        state->events |= TETRIS_EVENT_GAME_OVER;
    }
}

static void clear_lines(TetrisState *state)
{
    s8 y;
    u8 cleared = 0;

    for (y = TETRIS_BOARD_HEIGHT - 1; y >= 0; y--)
    {
        u8 x;
        u8 full = 1;

        for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
        {
            if (!state->board[(u8)y][x])
            {
                full = 0;
                break;
            }
        }

        if (full)
        {
            s8 copy_y;

            for (copy_y = y; copy_y > 0; copy_y--)
            {
                for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
                {
                    state->board[(u8)copy_y][x] = state->board[(u8)(copy_y - 1)][x];
                }
            }

            for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
            {
                state->board[0][x] = 0;
            }

            cleared++;
            y++;
        }
    }

    if (cleared)
    {
        static const u16 line_scores[4] = {SCORE_SINGLE, SCORE_DOUBLE, SCORE_TRIPLE, SCORE_TETRIS};

        state->lines += cleared;
        state->score += (u32)line_scores[cleared - 1] * (u32)(state->level + 1);
        state->level = (u8)(state->lines / 10);
        state->events |= TETRIS_EVENT_LINE;
    }
}

static void lock_piece(TetrisState *state)
{
    u8 i;
    const PieceCell *shape = piece_shapes[state->active_piece][state->rotation & 3];

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        s8 cell_x = state->piece_x + shape[i].x;
        s8 cell_y = state->piece_y + shape[i].y;

        if ((cell_x >= 0) && (cell_x < TETRIS_BOARD_WIDTH) && (cell_y >= 0) && (cell_y < TETRIS_BOARD_HEIGHT))
        {
            state->board[(u8)cell_y][(u8)cell_x] = state->active_piece + 1;
        }
    }

    state->events |= TETRIS_EVENT_LOCK;
    clear_lines(state);
    spawn_piece(state);
}

static u8 try_move(TetrisState *state, s8 dx, s8 dy)
{
    s8 new_x = state->piece_x + dx;
    s8 new_y = state->piece_y + dy;

    if (!collides(state, new_x, new_y, state->rotation))
    {
        state->piece_x = new_x;
        state->piece_y = new_y;
        return 1;
    }

    if (dy > 0)
    {
        lock_piece(state);
    }

    return 0;
}

static u8 try_rotate(TetrisState *state)
{
    u8 new_rotation = (state->rotation + 1) & 3;

    if (!collides(state, state->piece_x, state->piece_y, new_rotation))
    {
        state->rotation = new_rotation;
        return 1;
    }

    if (!collides(state, state->piece_x - 1, state->piece_y, new_rotation))
    {
        state->piece_x--;
        state->rotation = new_rotation;
        return 1;
    }

    if (!collides(state, state->piece_x + 1, state->piece_y, new_rotation))
    {
        state->piece_x++;
        state->rotation = new_rotation;
        return 1;
    }

    return 0;
}

static u8 hard_drop(TetrisState *state)
{
    u8 rows = 0;

    while (!collides(state, state->piece_x, state->piece_y + 1, state->rotation))
    {
        state->piece_y++;
        rows++;
    }

    state->score += (u32)rows * SCORE_HARD_DROP;
    state->events |= TETRIS_EVENT_HARD_DROP;
    lock_piece(state);

    return rows;
}

static void reset_playfield(TetrisState *state, u8 started)
{
    u8 x;
    u8 y;

    for (y = 0; y < TETRIS_BOARD_HEIGHT; y++)
    {
        for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
        {
            state->board[y][x] = 0;
        }
    }

    state->score = 0;
    state->lines = 0;
    state->level = 0;
    state->game_over = 0;
    state->started = started;
    state->paused = 0;
    state->drop_timer = 0;
    state->sequence_index = 0;
    state->events = 0;
    state->bag_index = 7;
    state->rng_state = 0xACE1;
    state->active_piece = 0;
    state->next_piece = next_piece(state);
    state->rotation = 0;
    state->piece_x = 3;
    state->piece_y = 0;

    if (started)
    {
        spawn_piece(state);
    }
}

void tetris_init(TetrisState *state)
{
    reset_playfield(state, 0);
}

void tetris_update(TetrisState *state, const TetrisInput *input)
{
    state->events = 0;

    if (!state->started)
    {
        if (input->start_pressed || input->rotate_pressed || input->hard_drop_pressed || input->down_held)
        {
            reset_playfield(state, 1);
            state->events |= TETRIS_EVENT_START;
        }

        return;
    }

    if (state->game_over)
    {
        if (input->start_pressed)
        {
            reset_playfield(state, 1);
            state->events |= TETRIS_EVENT_START;
        }

        return;
    }

    if (input->start_pressed)
    {
        state->paused = !state->paused;
        state->events |= TETRIS_EVENT_START;
        return;
    }

    if (state->paused)
    {
        return;
    }

    if (input->left_pressed && try_move(state, -1, 0))
    {
        state->events |= TETRIS_EVENT_MOVE;
    }

    if (input->right_pressed && try_move(state, 1, 0))
    {
        state->events |= TETRIS_EVENT_MOVE;
    }

    if (input->rotate_pressed && try_rotate(state))
    {
        state->events |= TETRIS_EVENT_ROTATE;
    }

    if (input->hard_drop_pressed)
    {
        hard_drop(state);
        return;
    }

    state->drop_timer++;

    if (input->down_held || (state->drop_timer >= drop_frames(state)))
    {
        u8 moved;

        state->drop_timer = 0;
        moved = try_move(state, 0, 1);

        if (input->down_held && moved)
        {
            state->score += SCORE_SOFT_DROP;
            state->events |= TETRIS_EVENT_SOFT_DROP;
        }
    }
}

u8 tetris_cell_at(const TetrisState *state, s8 x, s8 y)
{
    if ((x < 0) || (x >= TETRIS_BOARD_WIDTH) || (y < 0) || (y >= TETRIS_BOARD_HEIGHT))
    {
        return 0;
    }

    return state->board[(u8)y][(u8)x];
}

u8 tetris_is_active_cell(const TetrisState *state, s8 x, s8 y)
{
    u8 i;
    const PieceCell *shape = piece_shapes[state->active_piece][state->rotation & 3];

    if (!state->started || state->game_over)
    {
        return 0;
    }

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        if ((state->piece_x + shape[i].x == x) && (state->piece_y + shape[i].y == y))
        {
            return state->active_piece + 1;
        }
    }

    return 0;
}

void tetris_piece_cell(u8 piece, u8 rotation, u8 index, s8 *x, s8 *y)
{
    const PieceCell *shape;

    if ((piece >= 7) || (index >= TETRIS_PIECE_CELLS))
    {
        *x = 0;
        *y = 0;
        return;
    }

    shape = piece_shapes[piece][rotation & 3];
    *x = shape[index].x;
    *y = shape[index].y;
}
