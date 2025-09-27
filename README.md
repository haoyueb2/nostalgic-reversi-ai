# Reversi AI (Freshman Project, 2017)

This repository preserves the first programming project I wrote when I started learning C++ back in 2017: a Reversi (a.k.a. Othello) bot that plays against a human. I kept the original Visual Studio projects, the heaps of experiments around evaluation heuristics, and the reference material that inspired the work. I used GitHub Copilot / Codex to tidy everything up so the project reads like a time-capsule tour of that freshman-year effort.

## Repository Layout

- `projects/original/` — The exact contest submission I used in 2017. `main.cpp` contains the full alpha-beta search, while `sample_input_default.txt`/`sample_input_endgame.txt` reproduce the judge I/O logs I debugged against.
- `projects/refined/` — A later cleanup with clearer naming (global variables use a `g_` prefix) and extra stability-heuristic helpers. Its `main.cpp` is the easiest codebase to read today.
- `notes/` — Lab notebooks in text form: pruning experiments, timing tests, piece-ratio analysis, and handy `printf` templates (all converted to UTF-8).
- `source_archive/` — `original_ai.c`, my earliest standalone C prototype before I created the Visual Studio projects.
- `resources/` — PDFs, websites, and other reading material (plus classic engines such as Zebra) that I studied while building the bot.

> `.gitignore` already filters Visual Studio artefacts (`Debug/`, executables, object files, `.sdf`, `.suo`, etc.), keeping the repository tidy.

## Contest Platform

The bot was built for the SSE 2017 “Reversi Fun” online judge. The archived platform description—including the command protocol and testing framework—is available at <https://github.com/sse2017-reversi-fun/about>. The interaction loop in both `main.cpp` files follows that specification exactly.

## Build & Run

The projects were developed on Windows with Visual Studio 2012. To rebuild the original environment:

1. Open `ReversiAI.sln` in Visual Studio.
2. Choose `Release` or `Debug`, then build the solution.
3. Run the produced executable; it communicates with the judge using `START`/`PUT`/`TURN` commands.

You can also compile a single target with a modern compiler:

```bash
g++ -std=c++11 projects/refined/main.cpp -o reversi
```

> The current code plays on a 16×16 board (`N` constant). For the standard 8×8 game, adjust `N` and the initial disk placement inside `main.cpp`.

## Ideas for Future Tweaks

- Keep tuning the heuristic weights in `projects/refined/main.cpp`, or add iterative deepening and move ordering for stronger play.
- Build an opening book / stability table using the documents under `resources/` and the experiments in `notes/`.
- Curate the old match logs into a `docs/` folder and write up the algorithmic journey as a companion story.

Fork it, keep it as a time capsule, or build on top of it—have fun!
