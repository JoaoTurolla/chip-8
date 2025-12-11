#include <cstdint>
#include <random>
#include <fstream>

const unsigned int startAdress = 0x200; //Reason being the space needed for the interpreter, it should ocuppy from 0x000 to 0x1FF (511);

class Chip8{
   public:
   void loadRom(const char* fileName){

      std::ifstream file(fileName, std::ios::binary, std::ios::ate);

      if(file.is_open()){
         std::streampos size = file.tellg(); //since I opened the file from the end this should give me the size. Hopefully
         char* buffer = new char[size];

         file.seekg(0, std::ios::beg); //Got stuck here after 2 hours trying, not really used to file handling.
         // Well, if I open a file surely I would read it... why didn't I do that earlier? Idk
         file.read(buffer, size); //since size should compreheend the whole file, this should read all at once
         file.close();


         //delete[] buffer; Gotta let this here for a bit to shame on myself, great job! Read the file and delete what you've read
         //imediatelly after
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