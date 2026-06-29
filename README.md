# SGDK Tetris

A small Mega Drive / Genesis homebrew Tetris project built with SGDK. The current version is a polished ROM named **Mega Blocks**: it uses a centered large playfield, custom 16x16 colored background tiles, and a landing-position predictor.

## Features

- 10x12 large-block Tetris playfield designed for the Mega Drive 320x224 screen
- 7-bag piece randomizer
- Custom 16x16 colored block tiles for the playfield and next-piece preview
- Ghost/landing prediction for the active piece
- Next-piece shape preview
- Soft drop scoring
- Basic clockwise rotation with simple wall kicks
- DAS/ARR-style horizontal repeat for smoother movement
- Line-clear flash animation before cleared rows collapse
- Line, score, and level tracking
- Title, pause, and game-over states
- Project-specific Mega Drive ROM header metadata

## Requirements

Install SGDK and set the `GDK` environment variable to the SGDK directory:

```bash
export GDK=/path/to/SGDK
```

The project Makefile includes SGDK's standard `makefile.gen`.

## Build

```bash
cd /Users/happyelements/sgdk-tetris
make
```

For a clean rebuild:

```bash
cd /Users/happyelements/sgdk-tetris
make clean
make
```

The generated ROM is produced by SGDK, commonly under `out/` as `rom.bin`. This project may also produce `out/release/rom.bin` depending on the SGDK setup.

## Run

Open the generated `.bin` ROM in a Mega Drive / Genesis emulator such as BlastEm, Genesis Plus GX, OpenEmu, or ares.

Example:

```bash
open -a OpenEmu /Users/happyelements/sgdk-tetris/out/release/rom.bin
```

## Controls

- Start: start game, pause/resume, reset after game over
- D-pad left/right: move piece with repeat delay
- D-pad down: soft drop
- D-pad up: rotate clockwise
- A/B/C: rotate clockwise

## Manual Verification

After building, run the ROM in an emulator and check:

1. The ROM boots to the Mega Blocks title screen.
2. The main playfield is centered and occupies most of the screen.
3. Start, Up, or A/B/C begins gameplay.
4. The x-axis is 10 blocks wide and the visible y-axis is 12 large blocks high.
5. Blocks are 16x16, colored, tightly connected, and enclosed by a gray frame.
6. The ghost/landing predictor appears below the active piece and updates when moving or rotating.
7. The ghost does not overwrite the active piece or locked blocks.
8. Left/right movement has a controlled repeat rhythm instead of moving every frame.
9. Down soft-drops and increases score while the piece moves down.
10. Up and A/B/C rotate the active piece.
11. NEXT shows a four-cell colored piece preview.
12. Completed lines flash twice before the board collapses into the cleared shape.
13. Line clears update score, lines, and level.
14. Start pauses/resumes during play.
15. Game over appears when the stack reaches the top, and Start resets.

## Current Scope

This version prioritizes a large, centered arcade-style playfield. To fit 16x16 blocks on the Mega Drive's 224px NTSC height, the playfield uses 12 visible rows instead of the traditional 20-row Tetris well.

## Known Limitations / Roadmap

Planned future improvements include:

- Sound effects and music
- Hold piece
- More complete SRS-style rotation kicks
- High score persistence
- Automated emulator smoke tests
