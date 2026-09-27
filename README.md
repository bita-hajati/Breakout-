# 🎮 Breakout

A simple **Breakout game in C++** that runs in the Windows terminal.

The game is inspired by the classic Breakout gameplay: control the paddle, bounce the ball, and destroy all the bricks.


**This project was developed as a team project as part of a programming course.**

## ✨ Features

- 🎯 Destroy bricks using the ball
- 🕹️ Two game modes:
    - **Simple Mode**
    - **Advanced Mode**
- ❤️ 3 lives
- 💥 Power-ups in Advanced Mode
    - **Heart:** increases paddle size
    - **Bomb:** decreases paddle size
- ⏸️ Pause, resume, and restart the game
- 💾 Save and load unfinished games
- 📜 Game history with player name, score, mode, result, date, and time
- 🎨 Different ball and paddle styles
- 🔊 Sound effects for bricks, walls, losing a ball, victory, and game over
- 🌈 Colored terminal interface
- 🖥️ Console-based rendering with reduced screen flickering

## 🎮 Controls

| Key | Action |
|-----|--------|
| `A` | Move paddle left |
| `D` | Move paddle right |
| `SPACE` | Start / continue game |
| `P` | Pause game |
| `W` | Move up in menus |
| `S` | Move down in menus |
| `ENTER` | Select menu option |

## 🧩 Game Modes

### Simple Mode

The classic version of the game. Destroy all bricks while keeping the ball in play.

### Advanced Mode

Includes power-ups that can change the paddle size during the game.

## 📋 Main Menu

- `NEW GAME`
- `LOAD GAME`
- `HELP`
- `GAME HISTORY`
- `SETTINGS`
- `EXIT`

The Settings menu allows changing the ball and paddle appearance.

## 💾 Save & Load

An unfinished game can be saved and loaded later.

The saved game contains:

- Game mode
- Ball position and velocity
- Paddle position and size
- Brick states
- Power-ups
- Score
- Lives
- Player name

## 🛠️ Technologies

- **C++**
- Windows Console API
- `conio.h`
- `Windows.h`
- `winmm`
- File handling with `fstream`

## ▶️ How to Run

This project is designed for **Windows**.

Compile `Breakout.cpp` with a C++ compiler such as **MinGW g++** and run the generated executable.

Make sure the required `.wav` sound files are available next to the executable.

## 📁 Project Files

```text
Breakout/
│
├── Breakout.cpp
├── saved_game.txt
├── settings.txt
├── gameHistory.txt
├── aboutgame.txt
│
├── brick.wav
├── wall.wav
├── loseball.wav
├── Victory.wav
└── GameOver.wav