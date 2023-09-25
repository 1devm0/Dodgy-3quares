# Dodge 3quares 
- Play as a square and dodge the enemy square. Don't let it touch you, until the level is finished. Now the roles have reversed and the square has to reach the end point without touching you. 

## Tools Used
- My own OpenGL graphics framework written in C (SGFX) 
- Font Pack from Daniel Linssen


## Platform Support
- Windows 10 (not tested on Windows 11)

## Compiling
- Clone the repository
- Go into the folder
- If you are on Windows, run: `tools\scripts\run.bat` in Command Prompt
- If you want to compile for the web, run `make web` in Command Prompt
- For the web, you will have to host the server and all the build files can be found in the build folder
> Note for Windows users, in order to properly compile the project examples or game, you will have to get all the SDL DLLs and library downloaded from the official SDL website or some package manager. 


## TODO List
- [x] Create a character controller (2d)
- [x] Implement Level Loading + Physics (2.5 hrs)
- [x] Implement a timer
- [x] Implement the second player/ghost  with trying to escape with end tile level (2 hrs)
- [x] Breaking its ankles
- [x] Implement alternate level (2 hrs)
- [x] Add SFX


