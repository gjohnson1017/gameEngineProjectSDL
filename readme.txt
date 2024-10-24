# HW 2 README

_The engine code was built using both windows and mac system and should be compilable in both systems_
_Holding down 1 slows down the game client and 2 speeds up the game client_
_Pressing P pauses and resumes the game_
_Use Arrow Keys to control the player_
_Spin the server first and the run the clients_

# Windows Instructions
## Client Paramaters
Each time a new client is created use the below command line arguments
Client tcp://localhost:5555 tcp://localhost:5556 <uniqueId>

_uniqueId can start from 1 and incrementally give each client a unique id_

Server tcp://localhost:5555 tcp://localhost:5556 0


Sample complete command for running the game.exe
## SERVER COMMAND
./bin/debug/game.exe Server tcp://localhost:5555 tcp://localhost:5556 0

## CLIENT COMMANDS
./bin/debug/game.exe Client tcp://localhost:5555 tcp://localhost:5556 1
./bin/debug/game.exe Client tcp://localhost:5555 tcp://localhost:5556 2
./bin/debug/game.exe Client tcp://localhost:5555 tcp://localhost:5556 3


## COMMAND TO GENERATE THE EXE FILE
"</path/to/g++.exe> -g ./src/*.cpp -I ./include -L ./lib -o ./bin/debug/game.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lzmq-mt-4_3_5"


# Instructions to run on MacOS

First make sure you have install libzmq using homebrew, and follow all the other installation instructions from HW1
Then we need to compile the files to get an output file. This can be done with the following command -
"g++ src/*.cpp -I./include -g -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -rpath /Library/Frameworks -std=c++17 -stdlib=libc++ -I/opt/homebrew/include -L/opt/homebrew/lib -lzmq"

This will provide a ./a.out output file.

## Client Paramaters
Each time a new client is created use the below command line arguments
Client tcp://localhost:5555 tcp://localhost:5556 <uniqueId>

_uniqueId can start from 1 and incrementally give each client a unique id_

Server tcp://localhost:5555 tcp://localhost:5556 0


Sample complete command for running the ./a.out
## SERVER COMMAND
./a.out Server tcp://localhost:5555 tcp://localhost:5556 0

## CLIENT COMMANDS
./a.out Client tcp://localhost:5555 tcp://localhost:5556 1
./a.out Client tcp://localhost:5555 tcp://localhost:5556 2
./a.out Client tcp://localhost:5555 tcp://localhost:5556 3


This should give you 3 clients running a multiplayer game.

HW 1 README

_The engine code was built using both windows and mac system and should be compilable in both systems_

## To run the game in windows:

- Change the path to g++.exe in all the json files under .vscode
- Keep the -o flag and game.exe path in task.json

<code>
  {
  "version": "2.0.0",
  "tasks": [
  {
  "type": "shell",
  "label": "SDL2",
  "command": "D:\\msys64\\ucrt64\\bin\\g++.exe",
  "args": [
  "-g",
  "${workspaceFolder}\\src\\*.cpp",
  "-I",
  "${workspaceFolder}\\include",
  "-L",
  "${workspaceFolder}\\lib",
  "-o", <----------------------- like this
  "${workspaceFolder}\\bin\\debug\\game.exe",  <----------------------- like this
  "-lmingw32",
  "-lSDL2main",
  "-lSDL2",
  "-lSDL2_image"
  ],
  "options": {
  "cwd": "${workspaceFolder}"
    },
    "problemMatcher": ["$gcc"],
  "group": {
  "kind": "build",
  "isDefault": true
  }
  }
  ]
  }
</code>

## To run the game engine in windows:

- Change the path to g++.exe in all the json files under .vscode
- Remove the -o flag and game.exe path and replace it by -c flag in task.json ( as demonstrated below )

- Replace this with:
  <code>
  {
  "version": "2.0.0",
  "tasks": [
  {
  "type": "shell",
  "label": "SDL2",
  "command": "D:\\msys64\\ucrt64\\bin\\g++.exe",
  "args": [
  "-g",
  "${workspaceFolder}\\src\\*.cpp",
  "-I",
  "${workspaceFolder}\\include",
  "-L",
  "${workspaceFolder}\\lib",
  "-o",
  "${workspaceFolder}\\bin\\debug\\game.exe",
  "-lmingw32",
  "-lSDL2main",
  "-lSDL2",
  "-lSDL2_image"
  ],
  "options": {
  "cwd": "${workspaceFolder}"
    },
    "problemMatcher": ["$gcc"],
  "group": {
  "kind": "build",
  "isDefault": true
  }
  }
  ]
  }
  </code>
- With this:

  <code>
  {
  "version": "2.0.0",
  "tasks": [
  {```
  "type": "shell",
  "label": "SDL2",
  "command": "D:\\msys64\\ucrt64\\bin\\g++.exe", // relative to you system's g++ path
  "args": [
  "-g",
  "${workspaceFolder}\\src\\*.cpp",
  "-I",
  "${workspaceFolder}\\include",
  "-L",
  "${workspaceFolder}\\lib",
  "-c",
  "-lmingw32",
  "-lSDL2main",
  "-lSDL2",
  "-lSDL2_image"
  ],
  "options": {
  "cwd": "${workspaceFolder}"
        },
        "problemMatcher": ["$gcc"],
  "group": {
  "kind": "build",
  "isDefault": true
  }
  }
  ]
  }
  </code>

_Note: g++.exe path needs to be changed in all .json files_

- Set your default terminal to powershell or command prompt since the .json filesystem uses "\" instead of "/"

- Since SDL2 has been moved locally into include, lib and bin/debug the linker points relatively to the local directories
