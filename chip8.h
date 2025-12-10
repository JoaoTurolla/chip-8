#include <cstdint>
#include <random>
#include <fstream>

const unsigned int startAdress = 0x200;

class Chip8{
   public:
   void loadRom(const char* fileName){

      std::ifstream file(fileName, std::ios::binary, std::ios::ate);

      if(file.is_open()){
         std::streampos size = file.tellg();
         char* buffer = new char[size];

         file.seekg(0, std::ios::beg);
         //Got stuck here after 2 hours trying, not really used to file handling
      }
   }
   void cycle();

   uint8_t gfx[64 * 32];
   uint8_t keypad[16];

   private:
   
   
   uint8_t opcode;
   
   uint8_t memory[4096], V[16], DT, ST, SP;
   uint16_t I, PC, Stack[16];


};