# Boggle Game

This project implements the classic Boggle™ game in C++, where a human player competes against a computer. The game uses recursive algorithms to find valid words on a dynamically generated board. It features a clean graphical user interface and leverages Stanford's C++ libraries for GUI and backtracking logic.

## 🧠 Features

- Graphical user interface using Stanford C++ libraries
- Option for random or user-defined board
- Human input validation and scoring
- Computer finds all valid remaining words using recursion
- Word dictionary loading and prefix optimization
- Adheres to Boggle™ adjacency rules (horizontal, vertical, diagonal)

## 🔍 Key Concepts

- Recursive backtracking
- Grid traversal with constraints
- Dictionary prefix pruning (optimization)
- GUI interaction handling
- Depth-first search on a character matrix

## 📂 File Structure

- `main.cpp` – Game entry point and control loop
- `boggle.cpp/h` – Core game logic: board management, word finding, scoring
- `lexicon.cpp/h` – Dictionary and prefix optimization
- `StanfordCPPLib/` – GUI and support libraries

## 🚀 How to Run

1. Clone the repository.
2. Open the project using Qt Creator or another C++ IDE that supports the Stanford C++ library.
3. Build and run.

> 🔧 Requires: Qt framework with Stanford C++ libraries pre-installed.
