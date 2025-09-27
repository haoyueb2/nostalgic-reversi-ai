# Reversi AI (Freshman Project, 2017)

This repository preserves the first programming project I wrote when I started learning C++ back in 2017: a Reversi (a.k.a. Othello) bot that plays against a human. It keeps the original Visual Studio layout, the evaluation tweaks I experimented with, and the reference material I collected at the time. Publishing it now is mostly for nostalgia—and to document the ideas that shaped my early interest in AI.

## Repository Layout

- `ConsoleApplication1_old/`: The original contest submission. `main.cpp` holds the full search logic, and the `Text*.txt` files are the scripted inputs I used while debugging the alpha-beta search.
- `ConsoleApplication2/`: A later refactor with clearer naming (everything uses `g_` prefixes) and additional helper functions for stability evaluation. Its `main.cpp` is the version that is easiest to read today.
- `notes/`: Loose notes and experiment snippets (alpha-beta pruning variants, timing tests, piece-ratio experiments, debugging print templates).
- `archive/`: A preserved copy of `original_ai.c`, the earliest standalone attempt before the Visual Studio projects.
- `reference_materials/`: Books, papers, and other AI resources (PDFs, archives, web links) I studied while building the bot.

> The `.gitignore` filters out build artifacts (`Debug/`, executables, object files) and large Visual Studio caches (`.sdf`, `.suo`, etc.), so the repository stays lightweight.

## Contest Platform

This project was originally written for the SSE 2017 “Reversi Fun” online judge. The archived platform description (rules, protocol, testing framework) lives here: <https://github.com/sse2017-reversi-fun/about>. Most of the interaction logic in `main.cpp` follows that specification.

## Build & Run

The project was originally built on Windows with Visual Studio 2012. The solution and project files (`.sln`, `.vcxproj`) are included:

1. Open `ConsoleApplication1.sln` or either project file in Visual Studio.
2. Choose `Release` or `Debug` and build.
3. The resulting executable talks to the referee via the contest protocol (e.g., receive `START <side>` → respond `OK`, then alternate between `PUT`/`TURN`).

You can also compile a single version directly with a modern compiler:

```bash
g++ -std=c++11 ConsoleApplication2/main.cpp -o reversi
```

> The code currently uses a 16×16 board (`N` constant). To play the standard 8×8 game you will need to adjust `N` and the initial setup.

## Ideas for Future Tweaks

- Keep tuning the evaluation weights in `ConsoleApplication2`, or add iterative deepening for stronger search.
- Implement an opening book or refined stability heuristics using the material stored in `reference_materials/` and the experiments under `notes/`.
- Curate the old match logs and notes into a `docs/` folder, accompanied by a write-up about the algorithms I tried as a beginner.

Fork it, keep it as a time capsule, or build on top of it—have fun!
