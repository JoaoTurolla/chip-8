#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>

class Chip8{ 

   public:

   Chip8();
   void loadROM(const char* fileName);
   void Cycle();
   
   uint32_t graphics[2048]{};
   uint8_t keypad[16]{};

   private:
   
   uint8_t memory[4096]{}, V[16]{}, DT{}, ST{};
   uint16_t I{}, PC{}, Stack[16]{}, SP{}, opcode{};

   std::default_random_engine randGen;
   std::uniform_int_distribution<uint8_t> randByte;
   
   void op0NNN();
   
   void op00E0();
   
   void op00EE();
   
   void op1NNN();
   
   void op2NNN();
   
   void op3XKK();
   
   void op4XKK();
   
   void op5XY0();
   
   void op6XKK();
   
   void op7XKK();
   
   void op8XY0();
   
   void op8XY1();
   
   void op8XY2();
   
   void op8XY3();
   
   void op8XY4();
   
   void op8XY5();
   
   void op8XY6();

   void op8XY7();

   void op8XYE();

   void op9XY0();

   void opANNN();

   void opBNNN();

   void opCXKK();

   void opDXYN();

   void opEX9E();

   void opEXA1();

   void opFX07();

   void opFX0A();

   void opFX15();

   void opFX18();

   void opFX1E();

   void opFX29();

   void opFX33();

   void opFX55();

   void opFX65();

};