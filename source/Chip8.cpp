#include "chip8.hpp"
#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>
#include <cstring>

/*
After working on this for 4 weeks (reason on README) I realised I might've made a mistake and shouldn't have all the code on the hpp file
It was kind of hard keeping track of what I was declaring and what I needed to define, be it functions or whatever
So I decided to split it into two files, one to declare everything, and this one to define everything
And now I think I understood why use a constructor 
*/

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

   opcode = 0x0000u; 
   I = 0x0000u;
   SP = 0;

   for(unsigned int i = 0; i < 0x1000u; i++) memory[i] = 0;
   for(unsigned int i = 0; i < 0x800u; i++) memset(graphics, 0, sizeof(graphics));
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
      file.read(buffer, size); //since size should compreheend the whole file now, this should read all at once
      file.close();

      for(int32_t i = 0; i < size; i++){
         if((startAddress + i) >= 0x1000) break;
         memory[startAddress + i] = (uint8_t) buffer[i];
      }
      delete[] buffer;
   } else{
      exit(EXIT_FAILURE);
   }
}

void Chip8::Cycle(){
   
   //Fetch
   opcode = (memory[PC] << 8u) | memory[PC + 1]; //I initially used + instead of |, which I discovered to be fine in this scenario where there will be no overlapping bits, but I prefered to correct it
    
   PC+=2;
   
   //Decode and Execute
   switch(opcode & 0xF000u){//I kinda regret doing these switches... doesn't look as optimal as I thought I could do
      case 0x0000u:
      switch(opcode & 0x000Fu){
         case 0x000Eu:
         op00EE();
         break;
         default:
         op00E0();
      }
      break;
      case 0x1000u:
      op1NNN();
      break;
      case 0x2000u:
      op2NNN();
      break;
      case 0x3000u:
      op3XKK();
      break;
      case 0x4000u:
      op4XKK();
      break;
      case 0x5000u:
      op5XY0();
      break;
      case 0x6000u:
      op6XKK();
      break;
      case 0x7000u:
      op7XKK();
      break;
      case 0x8000u:
      switch(opcode & 0x000Fu){
         case 0x0000u:
         op8XY0();
         break;
         case 0x0001u:
         op8XY1();
         break;
         case 0x0002u:
         op8XY2();
         break;
         case 0x0003u:
         op8XY3();
         break;
         case 0x0004u:
         op8XY4();
         break;
         case 0x0005u:
         op8XY5();
         break;
         case 0x0006u:
         op8XY6();
         break;
         case 0x0007u:
         op8XY7();
         break;
         default:
         op8XYE();
         break;
      }
      break;
      case 0x9000u:
      op9XY0();
      break;
      case 0xA000u:
      opANNN();
      break;
      case 0xB000u:
      opBNNN();
      break;
      case 0xC000u:
      opCXKK();
      break;
      case 0xD000u:
      opDXYN();
      break;
      case 0xE000u:
      switch(opcode & 0x000Fu){
         case 0x000Eu:
         opEX9E();
         break;
         default:
         opEXA1();
         break;
      }
      break;
      case 0xF000u:
      switch(opcode & 0x00FFu){
         case 0x0007u:
         opFX07();
         break;
         case 0x000Au:
         opFX0A();
         break;
         case 0x0015u:
         opFX15();
         break;
         case 0x0018u:
         opFX18();
         break;
         case 0x001Eu:
         opFX1E();
         break;
         case 0x0029u:
         opFX29();
         break;
         case 0x0033u:
         opFX33();
         break;
         case 0x0055u:
         opFX55();
         break;
         default:
         opFX65();
         break;
      }
      break;
      default: 
      break;
   }
   
};

void Chip8::updateTimers() {
   if (DT > 0) DT--;
   if (ST > 0) ST--;
}

void Chip8::op0NNN(){//SYS addr; jump to a machine code routine at NNN; ignored in modern interpreters
   uint16_t address = opcode & 0x0FFFu;
   PC = address;
}

void Chip8::op00E0(){//CLS; clear the display;
   memset(graphics, 0 , sizeof(graphics));
   V[0x000Fu] = 1;
}

void Chip8::op00EE(){//RET, return 
   --SP;
   PC = Stack[SP];
}

void Chip8::op1NNN(){//JP addr; jump to location NNN, seems like the same as 0NNN? Could it be why it's ignored in modern interpreters?
   uint16_t address = opcode & 0x0FFFu; //OK, it took me quite some time to figure how this works. So 0x0FFFu is : 0000 1111 1111 1111 in binary, which means only the address in the opcode will stay after the bit by bit AND operation, then we pass that to the PC; 
   PC = address; 
}

void Chip8::op2NNN(){//CALL addr; call subroutine at NNN
   uint16_t address = opcode & 0x0FFFu; //after figuring out how the above works, this one was easy
   Stack[SP] = PC;
   ++SP;
   PC = address;
}

void Chip8::op3XKK(){//SE Vx, byte 
   uint8_t X = (opcode & 0x0F00u) >> 8u; //Same principle as before for the address, we extract what we want from the opcode with bit by bit AND operations, but here we need to dislocate the bits to the correct position.
   uint8_t KK = (opcode & 0x00FFu);
   if(V[X] == KK) PC += 2;
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
   
   if(V[X] >= V[Y]) V[0x000Fu] = 1;
   else V[0x000Fu] = 0;
   V[X] -= V[Y];
}

void Chip8::op8XY6(){//SHR Vx
   uint8_t X = (opcode & 0x0F00u) >> 8u;

   if(V[X] & 1) V[0x000Fu] = 1;
   else V[0x000fu] = 0; //if the least significant bit is 1, V[F] will be one, otherwise 0.   

   V[X] >>= 1u;
}

void Chip8::op8XY7(){//SUBN Vx,Vy 
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   uint8_t Y = (opcode & 0x00F0u) >> 4u;

   if(V[Y] >= V[X]) V[0x000Fu] = 1;
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

   for(unsigned int row = 0; row < N; ++row){
      uint8_t spriteByte = memory[I + row];
      if(yPos + row >= 32) break;
      for(unsigned int col = 0; col < 8; ++col){
         uint8_t spritePixel = spriteByte & (0x0080u >> col);
         if(xPos + col >= 64) break;
         
         if(spritePixel) {
            uint32_t* screenPixel = &graphics[(yPos + row) * screenWidth + (xPos + col)];
            if(*screenPixel == 0xFFFFFFFFu) V[0x000Fu] = 1;
            *screenPixel ^= 0xFFFFFFFFu;
         }
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
   if(!keypad[key]) PC +=2;
}

void Chip8::opFX07(){//LD Vx, DT
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   V[X] = DT;
}

void Chip8::opFX0A(){//LD Vx, K
   uint8_t X = (opcode & 0x0F00u) >> 8u;
   bool keyPressed = false;

   for(unsigned int i = 0; i < 0x0010u; ++i){
      if(keypad[i]){
         V[X] = i;
         keyPressed = true;
         break;
      }
   }
   
   if(!keyPressed) PC -=2;
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