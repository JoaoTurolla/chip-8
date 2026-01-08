#include "chip8.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>

//After working on this for 4 weeks (reason on README) I realised I might've made a mistake and shouldn't have all the code on the hpp file
//It was kind of hard keeping track of what I was declaring and what I needed to define, be it functions or whatever
//So I decided to split it into two files, one to declare everything, and this one to define everything
//And now I think I understood why use a constructor 

const unsigned int fontsetSize = 80;
const unsigned int fontsetStartAddress = 0x50;
const unsigned int startAddress = 0x200;
const unsigned int screenWidth = 64;
const unsigned int screenHeight = 32; 
 

uint8_t fontset[fontsetSize] ={
	0xF0, 0x90, 0x90, 0x90, 0xF0,
	0x20, 0x60, 0x20, 0x20, 0x70,
	0xF0, 0x10, 0xF0, 0x80, 0xF0,
	0xF0, 0x10, 0xF0, 0x10, 0xF0,
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

void Chip8::op8XY6(){//SHR Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   if(V[X] & 1) V[0x000Fu] = 1;
   else V[0x000fu] = 0; //if the least significant bit is 1, V[F] will be one, otherwise 0.   

   V[X] >>= 1u;// as for this line I had to check on my reference how he did it, it didn't cross my mind to bit shift to make the division.
}

void Chip8::op8XY7(){//SUBN Vx,Vy 
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   if(V[Y] > V[X]) V[0x000Fu] = 1;
   else V[0x000Fu] = 0;

   V[X] = V[Y] - V[X];
}

void Chip8::op8XYE(){//SHL Vx {, Vy}
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   V[0x000Fu] = (V[X] & 0x0080u) >> 7u;

   V[X] <<= 1u;
}

void Chip8::op9XY0(){//SNE Vx, Vy
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   if(V[X] != V[Y]) PC +=2;
}

void Chip8::opANNN(){//LD I, addr
   uint16_t address = (opcode & 0x0FFFu);
   I = address;
}

void Chip8::opBNNN(){//JP V0, addr
   uint16_t address = (opcode & 0x0FFFu);
   PC = (V[0x0000u] + address);
}

void Chip8::opCXKK(){//RND Vx, Byte
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t KK = (opcode & 0x00FFu);

   V[X] = randByte(randGen) & KK;
}

void Chip8::opDXYN(){//DRW Vx, Vy, nibble
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;
   uint8_t N = (opcode & 0x000Fu);

   V[0x000Fu] = 0;
   uint8_t xPos = V[X] % screenWidth;
   uint8_t yPos = V[Y] % screenHeight;
   //Making it wrap if exceding screen limits and since VF's value matters we make sure to reset it to default value

   for(unsigned int i = 0; i < N; i++){
      uint8_t spriteByte = memory[I + i];
      for(unsigned int j = 0; j < 8; j++){
         uint8_t spritePixel = spriteByte & (0x0080u >> j);
         uint32_t* screenPixel = &graphics[(yPos + i) * screenWidth + (xPos + j)];
         
         if(spritePixel) if(*screenPixel == 0xFFFFFFFFu) V[0x000Fu] = 1;

         *screenPixel ^= 0xFFFFFFFFu;
      }
   }

}

void Chip8::opEX9E(){//SKP Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t key = V[X];
   if(keypad[key]) PC +=2;
}

void Chip8::opEXA1(){//SKNP Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t key = V[X];
   if(!(keypad[key])) PC +=2;
}

void Chip8::opFX07(){//LD Vx, DT
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   V[X] = DT;
}

void Chip8::opFX0A(){//LD Vx, K
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   
   if(keypad[0x0000u]) V[X] = 0;
   else if(keypad[0x0001u]) V[X] = 1;
   else if(keypad[0x0002u]) V[X] = 2;
   else if(keypad[0x0003u]) V[X] = 3;
   else if(keypad[0x0004u]) V[X] = 4;
   else if(keypad[0x0005u]) V[X] = 5;
   else if(keypad[0x0006u]) V[X] = 6;
   else if(keypad[0x0007u]) V[X] = 7;
   else if(keypad[0x0008u]) V[X] = 8;
   else if(keypad[0x0009u]) V[X] = 9;
   else if(keypad[0x000Au]) V[X] = 10;
   else if(keypad[0x000Bu]) V[X] = 11;
   else if(keypad[0x000Cu]) V[X] = 12;
   else if(keypad[0x000Du]) V[X] = 13;
   else if(keypad[0x000Eu]) V[X] = 14;
   else if(keypad[0x000Fu]) V[X] = 15;
   else PC -=2;
}

void Chip8::opFX15(){//LD DT, Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   DT = V[X];
}

void Chip8::opFX18(){//LD ST, Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   ST = V[X];
}

void Chip8::opFX1E(){//ADD I, Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   I += V[X];
}

void Chip8::opFX29(){//LD F, Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   I = fontsetStartAddress + (V[X] * 5);
}

void Chip8::opFX33(){//LD B, Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t digit = V[X];

   memory[I + 2] = digit % 10;
   digit /= 10;
   memory[I + 1] = digit % 10;
   digit /= 10;
   memory[I] = digit % 10;
   //Let's not talk about how I made it store only the hundreds the first time I coded this...
}

void Chip8::opFX55(){//LD [I], Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   for(unsigned int i = 0; i <= V[X]; ++i){
      memory[I + i] = V[i];
   }
}

void Chip8::opFX65(){
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   for(unsigned int i = 0; i <= V[X]; ++i){
      V[i] = memory[I + i];
   }
} 