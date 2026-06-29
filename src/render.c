#include "render.h"

#include <genesis.h>

#define BOARD_X 9
#define BOARD_Y 2
#define NEXT_X 31
#define NEXT_Y 4
#define SIDE_X 32

#define BLOCK_TILE_BASE TILE_USER_INDEX
#define BLOCK_TILE_EMPTY BLOCK_TILE_BASE
#define BLOCK_TILE_COUNT 61
#define BLOCK_TILE_GHOST_BASE (BLOCK_TILE_BASE + 32)
#define BLOCK_TILE_BORDER (BLOCK_TILE_BASE + 60)

#define BLOCK_COLOR_I 1
#define BLOCK_COLOR_O 2
#define BLOCK_COLOR_T 3
#define BLOCK_COLOR_S 4
#define BLOCK_COLOR_Z 5
#define BLOCK_COLOR_J 6
#define BLOCK_COLOR_L 7
#define BLOCK_COLOR_HIGHLIGHT 8
#define BLOCK_COLOR_SHADOW 9
#define BLOCK_COLOR_OUTLINE 10
#define BLOCK_COLOR_GHOST 11
#define BLOCK_COLOR_BORDER 12

#define BLOCK_ROW(a, b, c, d, e, f, g, h) \
    (((u32)(a) << 28) | ((u32)(b) << 24) | ((u32)(c) << 20) | ((u32)(d) << 16) | \
     ((u32)(e) << 12) | ((u32)(f) << 8) | ((u32)(g) << 4) | (u32)(h))

#define TILE_EMPTY \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0), \
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0)

#define TILE_FULL(c) \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c)

#define TILE_TL(main) \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main)

#define TILE_TR(main) \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW)

#define TILE_BL(main) \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_HIGHLIGHT, main, main, main, main, main, main, main), \
    BLOCK_ROW(BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW)

#define TILE_BR(main) \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, main, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(main, main, main, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW), \
    BLOCK_ROW(BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW)

#define BLOCK_2X2(main) TILE_TL(main), TILE_TR(main), TILE_BL(main), TILE_BR(main)

#define TILE_GHOST(c) \
    BLOCK_ROW(c, c, c, c, c, c, c, c), \
    BLOCK_ROW(c, c, 0, c, 0, c, 0, c), \
    BLOCK_ROW(c, 0, c, 0, c, 0, c, c), \
    BLOCK_ROW(c, c, 0, c, 0, c, 0, c), \
    BLOCK_ROW(c, 0, c, 0, c, 0, c, c), \
    BLOCK_ROW(c, c, 0, c, 0, c, 0, c), \
    BLOCK_ROW(c, 0, c, 0, c, 0, c, c), \
    BLOCK_ROW(c, c, c, c, c, c, c, c)

static char number_buffer[12];

static const u32 block_tiles[BLOCK_TILE_COUNT * 8] = {
    TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY,
    BLOCK_2X2(BLOCK_COLOR_I),
    BLOCK_2X2(BLOCK_COLOR_O),
    BLOCK_2X2(BLOCK_COLOR_T),
    BLOCK_2X2(BLOCK_COLOR_S),
    BLOCK_2X2(BLOCK_COLOR_Z),
    BLOCK_2X2(BLOCK_COLOR_J),
    BLOCK_2X2(BLOCK_COLOR_L),
    TILE_GHOST(BLOCK_COLOR_I), TILE_GHOST(BLOCK_COLOR_I), TILE_GHOST(BLOCK_COLOR_I), TILE_GHOST(BLOCK_COLOR_I),
    TILE_GHOST(BLOCK_COLOR_O), TILE_GHOST(BLOCK_COLOR_O), TILE_GHOST(BLOCK_COLOR_O), TILE_GHOST(BLOCK_COLOR_O),
    TILE_GHOST(BLOCK_COLOR_T), TILE_GHOST(BLOCK_COLOR_T), TILE_GHOST(BLOCK_COLOR_T), TILE_GHOST(BLOCK_COLOR_T),
    TILE_GHOST(BLOCK_COLOR_S), TILE_GHOST(BLOCK_COLOR_S), TILE_GHOST(BLOCK_COLOR_S), TILE_GHOST(BLOCK_COLOR_S),
    TILE_GHOST(BLOCK_COLOR_Z), TILE_GHOST(BLOCK_COLOR_Z), TILE_GHOST(BLOCK_COLOR_Z), TILE_GHOST(BLOCK_COLOR_Z),
    TILE_GHOST(BLOCK_COLOR_J), TILE_GHOST(BLOCK_COLOR_J), TILE_GHOST(BLOCK_COLOR_J), TILE_GHOST(BLOCK_COLOR_J),
    TILE_GHOST(BLOCK_COLOR_L), TILE_GHOST(BLOCK_COLOR_L), TILE_GHOST(BLOCK_COLOR_L), TILE_GHOST(BLOCK_COLOR_L),
    TILE_FULL(BLOCK_COLOR_BORDER)
};

static u16 block_attr(u16 tile_index)
{
    return TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, tile_index);
}

static u16 ghost_attr(u16 tile_index)
{
    return TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, tile_index);
}

static u16 cell_tile_base(u8 cell)
{
    return BLOCK_TILE_BASE + ((u16)cell * 4);
}

static void draw_2x2(u16 tile_base, u16 x, u16 y)
{
    VDP_setTileMapXY(BG_A, block_attr(tile_base), x, y);
    VDP_setTileMapXY(BG_A, block_attr(tile_base + 1), x + 1, y);
    VDP_setTileMapXY(BG_A, block_attr(tile_base + 2), x, y + 1);
    VDP_setTileMapXY(BG_A, block_attr(tile_base + 3), x + 1, y + 1);
}

static void draw_block_cell(u8 cell, u16 x, u16 y)
{
    if (cell <= 7)
    {
        draw_2x2(cell_tile_base(cell), x, y);
        return;
    }

    draw_2x2(BLOCK_TILE_GHOST_BASE, x, y);
}

static void draw_border(void)
{
    u16 x;
    u16 y;
    u16 right = BOARD_X + (TETRIS_BOARD_WIDTH * 2) + 1;
    u16 bottom = BOARD_Y + (TETRIS_BOARD_HEIGHT * 2);

    for (x = BOARD_X; x <= right; x++)
    {
        VDP_setTileMapXY(BG_A, block_attr(BLOCK_TILE_BORDER), x, BOARD_Y - 1);
        VDP_setTileMapXY(BG_A, block_attr(BLOCK_TILE_BORDER), x, bottom);
    }

    for (y = BOARD_Y; y < bottom; y++)
    {
        VDP_setTileMapXY(BG_A, block_attr(BLOCK_TILE_BORDER), BOARD_X, y);
        VDP_setTileMapXY(BG_A, block_attr(BLOCK_TILE_BORDER), right, y);
    }
}

static void clear_next_piece(void)
{
    VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), NEXT_X, NEXT_Y, 8, 4);
}

static u16 ghost_tile_base(u8 cell)
{
    if ((cell >= 1) && (cell <= 7))
    {
        return BLOCK_TILE_GHOST_BASE + ((u16)(cell - 1) * 4);
    }

    return BLOCK_TILE_GHOST_BASE;
}

static void draw_ghost_cell(u8 cell, u16 x, u16 y)
{
    u16 tile_base = ghost_tile_base(cell);

    VDP_setTileMapXY(BG_A, ghost_attr(tile_base), x, y);
    VDP_setTileMapXY(BG_A, ghost_attr(tile_base + 1), x + 1, y);
    VDP_setTileMapXY(BG_A, ghost_attr(tile_base + 2), x, y + 1);
    VDP_setTileMapXY(BG_A, ghost_attr(tile_base + 3), x + 1, y + 1);
}

static void init_block_palette(void)
{
    PAL_setColor(32, RGB3_3_3_TO_VDPCOLOR(0, 0, 1));
    PAL_setColor(33, RGB3_3_3_TO_VDPCOLOR(0, 7, 7));
    PAL_setColor(34, RGB3_3_3_TO_VDPCOLOR(7, 7, 0));
    PAL_setColor(35, RGB3_3_3_TO_VDPCOLOR(5, 1, 7));
    PAL_setColor(36, RGB3_3_3_TO_VDPCOLOR(0, 7, 2));
    PAL_setColor(37, RGB3_3_3_TO_VDPCOLOR(7, 1, 1));
    PAL_setColor(38, RGB3_3_3_TO_VDPCOLOR(1, 2, 7));
    PAL_setColor(39, RGB3_3_3_TO_VDPCOLOR(7, 4, 0));
    PAL_setColor(40, RGB3_3_3_TO_VDPCOLOR(7, 7, 7));
    PAL_setColor(41, RGB3_3_3_TO_VDPCOLOR(1, 1, 2));
    PAL_setColor(42, RGB3_3_3_TO_VDPCOLOR(0, 0, 0));
    PAL_setColor(43, RGB3_3_3_TO_VDPCOLOR(4, 4, 4));
    PAL_setColor(44, RGB3_3_3_TO_VDPCOLOR(4, 4, 4));
    PAL_setColor(45, RGB3_3_3_TO_VDPCOLOR(0, 4, 4));
    PAL_setColor(46, RGB3_3_3_TO_VDPCOLOR(4, 4, 0));
    PAL_setColor(47, RGB3_3_3_TO_VDPCOLOR(3, 0, 4));

    PAL_setColor(48, RGB3_3_3_TO_VDPCOLOR(0, 0, 1));
    PAL_setColor(49, RGB3_3_3_TO_VDPCOLOR(0, 3, 3));
    PAL_setColor(50, RGB3_3_3_TO_VDPCOLOR(4, 4, 0));
    PAL_setColor(51, RGB3_3_3_TO_VDPCOLOR(3, 0, 4));
    PAL_setColor(52, RGB3_3_3_TO_VDPCOLOR(0, 4, 1));
    PAL_setColor(53, RGB3_3_3_TO_VDPCOLOR(4, 0, 0));
    PAL_setColor(54, RGB3_3_3_TO_VDPCOLOR(0, 1, 4));
    PAL_setColor(55, RGB3_3_3_TO_VDPCOLOR(4, 2, 0));
}

static void load_block_tiles(void)
{
    VDP_loadTileData(block_tiles, BLOCK_TILE_BASE, BLOCK_TILE_COUNT, CPU);
}

static void draw_number(u32 value, u16 x, u16 y)
{
    char reversed[12];
    u8 count = 0;
    u8 i;

    VDP_drawText("        ", x, y);

    if (value == 0)
    {
        VDP_drawText("0", x, y);
        return;
    }

    while ((value > 0) && (count < sizeof(reversed) - 1))
    {
        reversed[count++] = (char)('0' + (value % 10));
        value /= 10;
    }

    for (i = 0; i < count; i++)
    {
        number_buffer[i] = reversed[count - i - 1];
    }

    number_buffer[count] = 0;
    VDP_drawText(number_buffer, x, y);
}

static u8 ghost_collides(const TetrisState *state, s8 piece_x, s8 piece_y)
{
    u8 i;

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        s8 x;
        s8 y;
        s8 cell_x;
        s8 cell_y;

        tetris_piece_cell(state->active_piece, state->rotation, i, &x, &y);
        cell_x = piece_x + x;
        cell_y = piece_y + y;

        if ((cell_x < 0) || (cell_x >= TETRIS_BOARD_WIDTH) || (cell_y >= TETRIS_BOARD_HEIGHT))
        {
            return 1;
        }

        if ((cell_y >= 0) && tetris_cell_at(state, cell_x, cell_y))
        {
            return 1;
        }
    }

    return 0;
}

static s8 ghost_y_for_state(const TetrisState *state)
{
    s8 ghost_y = state->piece_y;

    while (!ghost_collides(state, state->piece_x, ghost_y + 1))
    {
        ghost_y++;
    }

    return ghost_y;
}

static u8 is_ghost_cell(const TetrisState *state, s8 ghost_y, s8 x, s8 y)
{
    u8 i;

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        s8 piece_x;
        s8 piece_y;

        tetris_piece_cell(state->active_piece, state->rotation, i, &piece_x, &piece_y);
        if ((state->piece_x + piece_x == x) && (ghost_y + piece_y == y))
        {
            return 1;
        }
    }

    return 0;
}

static void draw_static_text(void)
{
    VDP_setTextPalette(PAL1);
    VDP_drawText("MEGA", 2, 1);
    VDP_drawText("BLOCKS", 1, 2);
    VDP_drawText("UP/A/B/C", 1, 24);
    VDP_drawText("ROTATE", 2, 25);
    VDP_drawText("DOWN", 2, 26);
    VDP_drawText("DROP", 2, 27);

    VDP_setTextPalette(PAL0);
    VDP_drawText("NEXT", SIDE_X, 2);
    VDP_drawText("SCORE", SIDE_X, 10);
    VDP_drawText("LINES", SIDE_X, 14);
    VDP_drawText("LEVEL", SIDE_X, 18);

    draw_border();
}

static void draw_next_piece(const TetrisState *state)
{
    u8 i;

    clear_next_piece();

    if (!state->started)
    {
        return;
    }

    for (i = 0; i < TETRIS_PIECE_CELLS; i++)
    {
        s8 x;
        s8 y;

        tetris_piece_cell(state->next_piece, 0, i, &x, &y);
        draw_block_cell(state->next_piece + 1, NEXT_X + (x * 2), NEXT_Y + (y * 2));
    }
}

static void clear_overlay(void)
{
    VDP_drawText("                    ", 10, 10);
    VDP_drawText("                    ", 10, 11);
    VDP_drawText("                    ", 10, 12);
    VDP_drawText("                    ", 10, 13);
    VDP_drawText("                    ", 10, 14);
    VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), 13, 16, 14, 2);
}

static void draw_overlay(const TetrisState *state)
{
    static u8 overlay_visible = 0;

    if (state->started && !state->game_over && !state->paused)
    {
        if (overlay_visible)
        {
            clear_overlay();
            overlay_visible = 0;
        }

        return;
    }

    VDP_setTextPalette(PAL2);
    clear_overlay();
    overlay_visible = 1;

    if (!state->started)
    {
        u8 i;

        VDP_drawText("MEGA BLOCKS", 15, 10);
        VDP_drawText("PRESS START", 15, 12);
        VDP_drawText("UP/A/B/C START", 13, 14);

        for (i = 0; i < 7; i++)
        {
            draw_block_cell(i + 1, 13 + (i * 2), 16);
        }
    }
    else if (state->game_over)
    {
        VDP_drawText("GAME OVER", 16, 12);
        VDP_drawText("START RESET", 15, 13);
    }
    else if (state->paused)
    {
        VDP_drawText("PAUSED", 17, 12);
        VDP_drawText("START RESUME", 14, 13);
    }
}

void render_init(void)
{
    VDP_setScreenWidth320();
    VDP_setPlaneSize(64, 32, TRUE);
    VDP_setBackgroundColor(0);

    PAL_setColor(0, RGB3_3_3_TO_VDPCOLOR(0, 0, 1));
    PAL_setColor(1, RGB3_3_3_TO_VDPCOLOR(7, 7, 7));
    PAL_setColor(16, RGB3_3_3_TO_VDPCOLOR(0, 0, 1));
    PAL_setColor(17, RGB3_3_3_TO_VDPCOLOR(2, 6, 7));
    init_block_palette();

    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    load_block_tiles();
    draw_static_text();
}

void render_draw(const TetrisState *state)
{
    s8 x;
    s8 y;
    s8 ghost_y = 0;
    u8 show_ghost = state->started && !state->paused && !state->game_over && !tetris_line_clear_active(state);

    if (show_ghost)
    {
        ghost_y = ghost_y_for_state(state);
    }

    for (y = 0; y < TETRIS_BOARD_HEIGHT; y++)
    {
        for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
        {
            u8 cell;

            if (tetris_is_clearing_line(state, y) && tetris_line_clear_flash_hidden(state))
            {
                draw_block_cell(0, BOARD_X + 1 + (x * 2), BOARD_Y + (y * 2));
                continue;
            }

            cell = tetris_is_active_cell(state, x, y);

            if (!cell)
            {
                cell = tetris_cell_at(state, x, y);
            }

            if (!cell && show_ghost && is_ghost_cell(state, ghost_y, x, y))
            {
                draw_ghost_cell(state->active_piece + 1, BOARD_X + 1 + (x * 2), BOARD_Y + (y * 2));
            }
            else
            {
                draw_block_cell(cell, BOARD_X + 1 + (x * 2), BOARD_Y + (y * 2));
            }
        }
    }

    draw_border();
    draw_next_piece(state);

    VDP_setTextPalette(PAL0);
    draw_number(state->score, SIDE_X, 11);
    draw_number(state->lines, SIDE_X, 15);
    draw_number(state->level, SIDE_X, 19);

    draw_overlay(state);
}
