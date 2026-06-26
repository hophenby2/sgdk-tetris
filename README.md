# SGDK Tetris

A small Mega Drive / Genesis homebrew Tetris project built with SGDK. The current version is a polished ROM named **Mega Blocks**: it uses SGDK text for the UI and custom background tiles for real square colored Tetris blocks.

## Features

- 10x20 Tetris playfield
- 7-bag piece randomizer
- Custom square colored block tiles for the playfield and next-piece preview
- Next-piece shape preview
- Soft drop scoring
- Basic clockwise rotation with simple wall kicks
- DAS/ARR-style horizontal repeat for smoother movement
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

Open the generated `.bin` ROM in a Mega Drive / Genesis emulator such as BlastEm, Genesis Plus GX, or ares.

Example:

```bash
blastem /Users/happyelements/sgdk-tetris/out/rom.bin
```

If your SGDK build outputs to the release directory instead:

```bash
blastem /Users/happyelements/sgdk-tetris/out/release/rom.bin
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
2. Start, Up, or A/B/C begins gameplay.
3. Left/right movement has a controlled repeat rhythm instead of moving every frame.
4. Down soft-drops and increases score while the piece moves down.
5. Up rotates the active piece without hard-dropping or immediately locking it.
6. A/B/C rotate the active piece.
7. The playfield uses square colored blocks instead of `II`, `OO`, or rectangular cells.
8. NEXT shows a four-cell square colored piece preview.
9. Piece order is randomized by 7-bag instead of repeating a fixed sequence.
10. Line clears update score, lines, and level.
11. Start pauses/resumes during play.
12. Game over appears when the stack reaches the top, and Start resets.

## Current Scope

This version intentionally keeps the UI simple while using custom SGDK background tiles for the Tetris blocks. It is meant to keep the ROM build pipeline simple while the core game loop, controls, scoring, visuals, and state flow are polished.

## Known Limitations / Roadmap

Planned future improvements include:

- Larger custom art and palettes
- Sound effects and music
- Ghost piece and hold piece
- More complete SRS-style rotation kicks
- Line-clear animation
- High score persistence
- Automated emulator smoke tests
