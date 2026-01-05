#include "chip8.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>

//After working on this for 2 weeks (reason on README) I realised I might've made a mistake and shouldn't have all the code on the hpp file
//It was kind of hard keeping track of what I was declaring and what I needed to define, be it functions or whatever
//So I decided to split it into two files, one to declare everything, and this one to define everything
//And now I think I understood why use a constructor 

const unsigned int fontsetSize = 80;
const unsigned int fontsetStartAddress = 0x50;
const unsigned int startAddress = 0x200;

uint8_t fontset[fontsetSize] ={
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
   
   PC = startAddress;

   for(unsigned int i = 0; i < fontsetSize; i++) memory[fontsetStartAddress + i] = fontset[i];
      
   randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
      
   //So, I don't quite understand yet why do I need the constructor to set everything to it's starting value, but since my reference did it
   //for now I'll simply use it too.

   opcode = 0; 
   I = 0;
   SP = 0;

   for(unsigned int i = 0; i < 0x1000u; i++) memory[i] = 0;
   for(unsigned int i = 0; i < 0x800u; i++) graphics[i] = 0;
   for(unsigned int i = 0; i < 0x010u; i++){
      V[i] = 0;
      Stack[i] = 0;
   }
}

void Chip8::loadROM(const char* fileName){

   std::ifstream file;

   file.open(fileName, std::ios::binary); //Fixed the problem, now it should open
   
   if(file.is_open()){

      file.seekg(0, std::ios::end);
      std::streampos size = file.tellg();
      char* buffer = new char[size];

      file.seekg(0, std::ios::beg); //Got stuck here after 2 hours trying, not really used to file handling.
      // Well, if I open a file surely I would read it... why didn't I do that earlier? Idk
      file.read(buffer, size); //since size should compreheend the whole file, this should read all at once
      file.close();

      for(long i = 0; i < size; i++) memory[startAddress + i] = buffer[i]; 
      
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
   uint16_t address = opcode & 0x0FFFu;
   PC = address;
}

void Chip8::op00E0(){//CLS, clear the display; I assume reseting should do it?
   for(unsigned int i = 0; i < 0x0800u; i++) graphics[i] = 0;
}

void Chip8::op00EE(){//RET, return 
   --SP;
   PC = Stack[SP];
}

void Chip8::op1NNN(){//JP addr, jump to location NNN, seems like the same as 0NNN? Could it be why it's ignored in modern interpreters?
   uint16_t address = opcode & 0x0FFFu; //OK, it took me quite some time to figure how this works. So 0xFFFu is : 0000 1111 1111 1111 in binary, which means only the address 
   PC = address; // in the opcode will stay after the bit by bit AND operation, then we pass that to the PC;
}

void Chip8::op2NNN(){//CALL addr, call subroutine at NNN
   uint16_t address = opcode & 0x0FFFu; //after figuring out how the above works, this one was easy
   Stack[SP] = PC;
   ++SP;
   PC = address;
}

void Chip8::op3XKK(){//SE Vx, byte 
   uint8_t X = (opcode & 0x0F00u) >> 8u; //Same principle as before for the address, we extract what we want from the opcode with bit by bit AND operations, but here we need to dislocate the bits to the correct position.
   uint8_t KK = (opcode & 0x00FFu);
   if(V[X] = KK) PC += 2;
}

void Chip8::op4XKK(){//SNE Vx, byte 
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t KK = opcode & 0x00FFu;
   if(V[X] != KK) PC += 2;
}

void Chip8::op5XY0(){//SE Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;
   if(V[X] == V[Y]) PC+=2;
}

void Chip8::op6XKK(){//LD Vx, byte
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t KK = opcode & 0x00FFu;

   V[X] = KK;
} //After truly understanding the opcode it makes total sense how to make all these functions without passing parameters. 

void Chip8::op7XKK(){//ADD Vx, byte
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t KK = opcode & 0x00FFu;

   V[X] += KK;
}

void Chip8::op8XY0(){//LD Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   V[X] = V[Y];
}

void Chip8::op8XY1(){//OR Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   V[X] = (V[X] | V[Y]);
} 

void Chip8::op8XY2(){//AND Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   V[X] = (V[X] & V[Y]);
}

void Chip8::op8XY3(){//XOR Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   V[X] ^= V[Y]; //Apparentyl C++ had a XOR bit by bit and I didn't know... and this piece of code was wrong in past versions...
}

void Chip8::op8XY4(){//ADD Vx, Vy;
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;
   uint16_t total = V[X] + V[Y];

   if(total > 255u){ 
      V[0x000Fu] = 1;
   }   else V[15] = 0;

   V[X] = total & 0x00FFu; //I could put only 0xFFu, but I'll let it extended so I can understand better.     
}

void Chip8::op8XY5(){//SUB Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;
   
   if(V[X] > V[Y]) V[0x000Fu] = 1;
   else V[0x000Fu] = 0;
   V[X] -= V[Y];
}

void Chip8::op8XY6(){ //SHR Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   if(V[X] & 1) V[0x000Fu] = 1;
   else V[0x000fu] = 0; //if the least significant bit is 1, V[F] will be one, otherwise 0.   

   V[X] >>= 1;// as for this line I had to check on my reference how he did it, it didn't cross my mind to bit shift to make the division.
}