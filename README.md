# chip-8
Tried emulating a chip-8 in C++ code to better understand the lower levels of computers.

This was a challenge I had given myself;

For the next 6 weeks (from 2025-12-10 to 2026-01-21) I'll be searching for references and documentations about how a chip-8 works, to then translate it into code, I aim 
to not depend on Youtube videos that explain how it works, nor any "AI" that could help.

Important info:
  To run the program and interact with it you will need a ROM.
  If you already have a ROM, put the file in the same folder as the executable, go to that folder through your terminal and type the comand: 
  ./Chip8 {videoScale} {cycleDelay} ROMname
  
  videoScale is recommended to be set to 10
  
  cycleDelay is recommended to be set to 3 or 4
  
  If you look at my CMakeLists.txt you will see there is a /thirdParty folder that's not here, you need
to download those libraries by yourself in their respective github repositories and put in a file with the same name (or change the name to whatever you like), else the program won't have the GUI.
