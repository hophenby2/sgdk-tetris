#include "render.h"

#include <genesis.h>

#define BOARD_X 2
#define BOARD_Y 3
#define PANEL_X 27
#define PANEL_W 10

#define BLOCK_TILE_BASE TILE_USER_INDEX
#define BLOCK_TILE_EMPTY BLOCK_TILE_BASE
#define BLOCK_TILE_COUNT 8

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

#define BLOCK_ROW(a, b, c, d, e, f, g, h) \
    (((u32)(a) << 28) | ((u32)(b) << 24) | ((u32)(c) << 20) | ((u32)(d) << 16) | \
     ((u32)(e) << 12) | ((u32)(f) << 8) | ((u32)(g) << 4) | (u32)(h))

#define BLOCK_SQUARE(main) \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, BLOCK_COLOR_HIGHLIGHT, BLOCK_COLOR_HIGHLIGHT, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, BLOCK_COLOR_HIGHLIGHT, main, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, main, main, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, main, main, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, main, main, main, main, main, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_SHADOW, BLOCK_COLOR_OUTLINE), \
    BLOCK_ROW(BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE, BLOCK_COLOR_OUTLINE)

static char number_buffer[12];

static const u32 block_tiles[BLOCK_TILE_COUNT * 8] = {
    /* Empty */
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),
    BLOCK_ROW(0, 0, 0, 0, 0, 0, 0, 0),

    /* I */
    BLOCK_SQUARE(BLOCK_COLOR_I),

    /* O */
    BLOCK_SQUARE(BLOCK_COLOR_O),

    /* T */
    BLOCK_SQUARE(BLOCK_COLOR_T),

    /* S */
    BLOCK_SQUARE(BLOCK_COLOR_S),

    /* Z */
    BLOCK_SQUARE(BLOCK_COLOR_Z),

    /* J */
    BLOCK_SQUARE(BLOCK_COLOR_J),

    /* L */
    BLOCK_SQUARE(BLOCK_COLOR_L)
};

static u16 block_attr(u16 tile_index)
{
    return TILE_ATTR_FULL(PAL2, FALSE, FALSE, FALSE, tile_index);
}

static void draw_block_cell(u8 cell, u16 x, u16 y)
{
    if (cell == 0)
    {
        VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), x, y, 1, 1);
        return;
    }

    if (cell <= 7)
    {
        VDP_setTileMapXY(BG_A, block_attr(BLOCK_TILE_BASE + cell), x, y);
        return;
    }

    VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), x, y, 1, 1);
}

static void clear_next_piece(void)
{
    VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), PANEL_X, 4, 8, 2);
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

    VDP_drawText("          ", x, y);

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

static void draw_panel_box(u16 x, u16 y, const char *title)
{
    VDP_drawText("+--------+", x, y);
    VDP_drawText("|        |", x, y + 1);
    VDP_drawText("|        |", x, y + 2);
    VDP_drawText("+--------+", x, y + 3);
    VDP_drawText(title, x + 2, y);
}

static void draw_static_text(void)
{
    s16 y;

    VDP_setTextPalette(PAL2);
    VDP_drawText("MEGA BLOCKS", 2, 1);
    VDP_drawText("2026 HOME BREW", 16, 1);

    VDP_setTextPalette(PAL1);
    VDP_drawText("+----------+", BOARD_X, BOARD_Y - 1);
    for (y = 0; y < TETRIS_BOARD_HEIGHT; y++)
    {
        VDP_drawText("|", BOARD_X, BOARD_Y + y);
        VDP_drawText("|", BOARD_X + TETRIS_BOARD_WIDTH + 1, BOARD_Y + y);
    }
    VDP_drawText("+----------+", BOARD_X, BOARD_Y + TETRIS_BOARD_HEIGHT);

    VDP_setTextPalette(PAL2);
    draw_panel_box(PANEL_X - 1, 3, "NEXT");
    draw_panel_box(PANEL_X - 1, 8, "SCORE");
    draw_panel_box(PANEL_X - 1, 13, "LINES");
    draw_panel_box(PANEL_X - 1, 18, "LEVEL");

    VDP_setTextPalette(PAL1);
    VDP_drawText("LEFT/RIGHT MOVE", 23, 23);
    VDP_drawText("UP/A/B/C ROTATE", 22, 24);
    VDP_drawText("DOWN SOFT DROP", 23, 25);
    VDP_drawText("START PAUSE", 25, 26);
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
        draw_block_cell(state->next_piece + 1, PANEL_X + 2 + x, 4 + y);
    }
}

static void clear_overlay(void)
{
    VDP_drawText("                    ", 4, 10);
    VDP_drawText("                    ", 4, 11);
    VDP_drawText("                    ", 4, 12);
    VDP_drawText("                    ", 4, 13);
    VDP_drawText("                    ", 4, 14);
    VDP_fillTileMapRect(BG_A, block_attr(BLOCK_TILE_EMPTY), 7, 16, 7, 1);
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

        VDP_drawText("MEGA BLOCKS", 7, 10);
        VDP_drawText("PRESS START", 7, 12);
        VDP_drawText("UP/A/B/C START", 6, 14);

        for (i = 0; i < 7; i++)
        {
            draw_block_cell(i + 1, 7 + i, 16);
        }
    }
    else if (state->game_over)
    {
        VDP_drawText("GAME OVER", 8, 12);
        VDP_drawText("START TO RESET", 6, 13);
    }
    else if (state->paused)
    {
        VDP_drawText("PAUSED", 10, 12);
        VDP_drawText("START TO RESUME", 5, 13);
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
    PAL_setColor(32, RGB3_3_3_TO_VDPCOLOR(0, 0, 1));
    PAL_setColor(33, RGB3_3_3_TO_VDPCOLOR(7, 6, 1));
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

    VDP_setTextPalette(PAL0);
    for (y = 0; y < TETRIS_BOARD_HEIGHT; y++)
    {
        for (x = 0; x < TETRIS_BOARD_WIDTH; x++)
        {
            u8 cell = tetris_is_active_cell(state, x, y);

            if (!cell)
            {
                cell = tetris_cell_at(state, x, y);
            }

            draw_block_cell(cell, BOARD_X + 1 + x, BOARD_Y + y);
        }
    }

    draw_next_piece(state);

    VDP_setTextPalette(PAL0);
    draw_number(state->score, PANEL_X + 1, 10);
    draw_number(state->lines, PANEL_X + 1, 15);
    draw_number(state->level, PANEL_X + 1, 20);

    draw_overlay(state);
}
