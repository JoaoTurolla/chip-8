#include <cstdint>
#include <random>

class Chip8{
   public:
   void loadRom(const char* fileName);
   void cycle();

   uint8_t gfx[64 * 32];
   uint8_t keypad[16];

   private:
   
   
   uint8_t opcode;
   
   uint8_t memory[4096];  
   uint8_t V[16], DT, ST;
   uint16_t I;


};