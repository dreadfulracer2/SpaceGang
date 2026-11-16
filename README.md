# SpaceGang
**SpaceGang** is a cooperative multiplayer space mission game developed in C++ using SFML.

Players work together to build and launch a crewed rocket. Each player votes on different rocket components while balancing cost, mass, thrust, and mission requirements.

## Features

- Multiplayer support for up to 4 players over LAN.
- Dedicated TCP server.
- Cooperative lobby and zone selection.
- Three operational zones:
  - **Mission Control** — Ground-based mission operations.
  - **Spacecraft** — Crewed spacecraft operations.
  - **Space Station** — Orbital station operations.
- Simultaneous rocket component voting.
- Three options for each rocket component.
- Random tie-breaking.
- Rocket launch evaluation based on component statistics.

## Technologies

- C++17
- SFML 3.0.2
- CMake
- Visual Studio 2022

## How to Play

1. Start `SpaceGangServer.exe`.
2. Launch `SpaceGangClient.exe`.
3. Connect to the server.
4. Select your operational zone.
5. Wait for all players to select their zones.
6. The host starts the game.
7. Vote on the crew module, first stage, and second stage.
8. Confirm your selections and wait for the other players.
9. Review the selected components and launch results.

## Building from Source

Requirements:

- CMake
- Visual Studio 2022 with C++ development tools
- Internet access during the initial dependency download

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

## Project Status

**Alpha — Work in Progress**

The project is under active development. Gameplay mechanics, graphics, mission simulation, and multiplayer functionality are subject to change.

## Author

Developed as an independent C++ multiplayer game project.
