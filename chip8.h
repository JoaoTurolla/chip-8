#include <cstdint>
#include <random>
#include <fstream>

class Chip8{


   //Not sure how would I read the opcode yet, but thought it would be nice to have the codes ready for when I do
   //Hopefully I make them right on  my first try 

   void ZeroNNN(uint16_t N){//SYS addr, jump to a machine code routine at NNN; ignored in modern interpreters
      PC = N;
   }
   
   void ZeroZeroEZero(){//CLS, clear the display; I assume reseting should do it?
      for(int i = 0; i < 2048; i++) graphics[i] = 0;
   }

   void ZeroZeroEE(){//RET, return 
      PC = SP; //Still quite confusing how to make the Return, gonna leave it at that for now, maybe I gotta look a bit into assembly
      SP -= 1;
   }

   void OneNNN(int NNN){//JP addr, jump to location NNN seems like the same as 0NNN?? Could it be why it's ignored in modern interpreters?
      PC = NNN;
   }

   public:

   Chip8::Chip8(){
      //So, I don't quite understand yet why do I need the constructor to set everything to it's starting value, but since my reference did it
      //for now I'll simply use it too, but set the values to what I understand they should be

      PC = 0x200; //Since 0x1FF is the last bit reserved to the interpreter, this should be the first for the instructions
      opcode = 0; 
      //still wondering if I need to simulate the binary code in opcode, like 8xy4 -> 1000 0000 0001 0100 which if I understood means:
      // Set V0 = V0 + V1 (ADD Vx, Vy)
      I = 0;
      SP = 0;

      for(int i = 0; i < 0x1000; i++) memory[i] = 0;
      for(int i = 0; i < 2048; i++) graphics[i] = 0;
      for(int i = 0; i < 16; i++){
         V[i] = 0;
         Stack[i] = 0;
      }

   }
   

   void loadRom(const char* fileName){

      std::ifstream file(fileName, std::ios::binary, std::ios::ate); //still don't understand why does it give me an error here, what's wrong 
      //with fileName?

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
   void cycle(){
      //Ok, based on what I've learned so far, I'll need to code the Fetch Decode Execute sequence here, but first I gotta understand how to
      //manipulate the memory
   };

   uint32_t graphics[2048];
   uint8_t keypad[16];

   private:
   
   uint8_t memory[4096], V[16], DT, ST;
   uint16_t I, PC, Stack[16], SP, opcode;


};