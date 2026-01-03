#include "chip8.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>

//After working on this for 2 weeks (reason on README) I realised I might've made a mistake and shouldn't have all the code on the hpp file
//It was kind of hard keeping track of what I was declaring and what I needed to define, be it functions or whatever
//So I decided to split it into two files, one to declare everything, and this one to define everything
//And now I think I understood why use a constructor

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;

uint8_t fontset[FONTSET_SIZE] ={
   0xF0, 0x90, 0x90, 0x90, 0xF0,
   0x20, 0x60, 0x20, 0x20, 0x70,
   0xF0, 0x10, 0xF0, 0x80, 0xF0,
   0x90, 0x90, 0xF0, 0x10, 0x10,
   0xF0, 0x80, 0xF0, 0x10, 0xF0,
   0xF0, 0x80, 0xF0, 0x90, 0xF0,
   0xF0, 0x10, 0x20, 0x40, 0x40,
   0xF0, 0x90, 0xF0, 0x90, 0xF0,
   0xF0, 0x90, 0xF0, 0x10, 0xF0,
   0xF0, 0x90, 0xF0, 0x90, 0x90,
   0xE0, 0x90, 0xE0, 0x90, 0xE0,
   0xF0, 0x80, 0x80, 0x80, 0xF0,
   0xE0, 0x90, 0x90, 0x90, 0xE0,
   0xF0, 0x80, 0xF0, 0x80, 0xF0,
   0xF0, 0x80, 0xF0, 0x80, 0x80
};

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
   
   PC = START_ADDRESS; //Since 0x1FF is the last bit reserved to the interpreter, this should be the first for the instructions

   for(unsigned int i = 0; i < FONTSET_SIZE; i++) memory[FONTSET_START_ADDRESS + i] = fontset[i];
      
   randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
      
   //So, I don't quite understand yet why do I need the constructor to set everything to it's starting value, but since my reference did it
   //for now I'll simply use it too.

   opcode = 0; 
   I = 0;
   SP = 0;

   for(int i = 0; i < 0x1000; i++) memory[i] = 0;
   for(int i = 0; i < 2048; i++) graphics[i] = 0;
   for(int i = 0; i < 16; i++){
      V[i] = 0;
      Stack[i] = 0;
   }
}

void Chip8::loadROM(const char* fileName){

  std::ifstream file(fileName, std::ios::binary, std::ios::ate); //still don't understand why does it give me an error here, what's wrong 
   //with fileName?

   if(file.is_open()){
      std::streampos size = file.tellg(); //since I opened the file from the end this should give me the size. Hopefully
      char* buffer = new char[size];

      file.seekg(0, std::ios::beg); //Got stuck here after 2 hours trying, not really used to file handling.
      // Well, if I open a file surely I would read it... why didn't I do that earlier? Idk
      file.read(buffer, size); //since size should compreheend the whole file, this should read all at once
      file.close();

      for(long i = 0; i < size; i++) memory[START_ADDRESS + i] = buffer[i]; 
      
      delete[] buffer;
   }
}

void Chip8::Cycle(){
   //Ok, based on what I've learned so far, I'll need to code the Fetch Decode Execute sequence here, but first I gotta understand how to
   //manipulate the memory and decide which registers will be used...
};

//Not sure how would I read the opcode yet, but thought it would be nice to have the codes ready for when I do
//Hopefully I make them right on my first try 

//First things first, there are a lot of errors for now cause I still need to compreheend how to make this work WITHOUT passing any parameters
//So I'll leave it like this for a bit cause I want to still have a reference as to how it should somewhat behave

void Chip8::op0NNN(){//SYS addr, jump to a machine code routine at NNN; ignored in modern interpreters
   PC = NNN;
}

void Chip8::op00E0(){//CLS, clear the display; I assume reseting should do it?
   for(int i = 0; i < 2048; i++) graphics[i] = 0;
}

void Chip8::op00EE(){//RET, return 
   PC = SP; //Still quite confusing how to make the Return, gonna leave it at that for now, maybe I gotta look a bit into assembly
   SP -= 1;
}

void Chip8::op1NNN(){//JP addr, jump to location NNN seems like the same as 0NNN?? Could it be why it's ignored in modern interpreters?
   PC = NNN;
}

void Chip8::op2NNN(){//CALL addr, call subroutine at NNN
   SP++;
   Stack[SP] = PC;
   PC = NNN;
}

void Chip8::op3XKK(){//SE Vx, byte 
   if(V[X] = KK) PC += 2;
}

void Chip8::op4XKK(){//SNE Vx, byte 
   if(V[X] != KK) PC += 2;
}

void Chip8::op5XY0(){//SE Vx, Vy
   if(V[X] == V[Y]) PC+=2;
}

void Chip8::op6XKK(){//LD Vx, byte
   V[X] = KK;
}

void Chip8::op7XKK(){//ADD Vx, byte
   V[X] += KK;
}

void Chip8::op8XY0(){//LD Vx, Vy
   V[X] = V[Y];
}

void Chip8::op8XY1(){//OR Vx, Vy
   V[X] = (V[X] | V[Y]);
} 

void Chip8::op8XY2(){//AND Vx, Vy
   V[X] = (V[X] & V[Y]);
}

void Chip8::op8XY3(){//XOR Vx, Vy
   V[X] = (V[X] | V[Y] & !(V[X] & V[Y]));
}

void Chip8::op8XY4(){//ADD Vx, Vy; At this point I'm considering if I should have a return type in all these... might change it soon;
   V[X] += V[Y];
   if(V[X] > 255){ 
      V[15] = 1;
      V[X] = 255;      
   }   else V[15] = 0;
}

void Chip8::op8XY5(){//SUB Vx, Vy
   if(V[X] > V[Y]) V[15] = 1;
   else V[15] = 0;
   V[X] -= V[Y];
}

void Chip8::op8XY6(){ //SHR Vx, Vy
   V[X] = V[X] & 1;
}