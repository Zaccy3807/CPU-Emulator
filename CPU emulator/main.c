#include <stdio.h>

//Instruction Set Architecture
//LOGIC/ALU COMMANDS
#define NUL 0x00
#define ADD 0x01
#define MIN 0x02
#define AND 0x03
#define NOR 0x04
#define CMP 0x05
#define OVR 0x06

//CPU REGISTER COMMANDS
#define MWF 0x07
#define MWT 0x08
#define SET 0x09

//MEMORY COMMANDS
#define STW 0x0A
#define LDW 0x0B
#define STK 0x0C
#define JNZ 0x0D

//SYSTEM COMMANDS
#define IOB 0x0E
#define QUT 0x0F

typedef struct {
        char *memory;
        char *registers;
        char *devices;
        int MaddrSECT, devs, program_counter;
        unsigned char ins, opcode, operand;
        char RUNNING;
} CPU;

//Core functions
void fetch(CPU *cpu) {
     cpu->ins = cpu->memory[cpu->program_counter];
     cpu->program_counter++;
     return;
}

void decode(CPU *cpu) {
     cpu->opcode = cpu->ins >> 4;
     cpu->operand = cpu->ins & 0x0F;
     return;
}

void printRegisters(CPU *cpu) {
     /*for (int i = 0; i < 4; i++) {
          for (int n = 0; n < 4; n++) {
               printf("Reg%X: %02X   ", n+(4*i) & 0x0F, cpu->registers[n+(4*i)] & 0xFF);
          }
          printf("\n");
     }*/
    printf("agi1 %02X  agi2 %02X  gen1 %02X  gen2 %02X\ngen3 %02X  gen4 %02X  gen5 %02X  gen6 %02X\ngen7 %02X  gen8 %02X  iods %02X  stks %02X\nstkp %02X  flgs %02X  prms %02X  mems %02X\n",
    cpu->registers[0] & 0xFF, cpu->registers[1] & 0xFF, cpu->registers[2] & 0xFF, cpu->registers[3] & 0xFF, 
    cpu->registers[4] & 0xFF, cpu->registers[5] & 0xFF, cpu->registers[6] & 0xFF, cpu->registers[7] & 0xFF,
    cpu->registers[8] & 0xFF, cpu->registers[9] & 0xFF, cpu->registers[10] & 0xFF, cpu->registers[11] & 0xFF,
    cpu->registers[12] & 0xFF, cpu->registers[13] & 0xFF, cpu->registers[14] & 0xFF, cpu->registers[15] & 0xFF);
}

void printMemory(CPU *cpu, int startAddr, int width) {
     for (int i = 0; i < width; i++) printf("0x%04X: %02X\n", (startAddr+i) & 0xFFFF, cpu->memory[startAddr+i] & 0xFF);
     printf("\n");
}

void execute(CPU *cpu) {
     switch (cpu->opcode) {
          case NUL: {break;}
          case QUT: {cpu->RUNNING=0; break;}
          case ADD: {if (!(cpu->registers[0xD]&4)) {cpu->registers[cpu->operand] = cpu->registers[0x0] + cpu->registers[0x1];} else {cpu->registers[0x0]+=cpu->operand;} break;}
          case MIN: {if (!(cpu->registers[0xD]&4)) {cpu->registers[cpu->operand] = cpu->registers[0x0] - cpu->registers[0x1];} else {cpu->registers[0x0]-=cpu->operand;} break;}
          case AND: {cpu->registers[cpu->operand] = cpu->registers[0x0] & cpu->registers[0x1]; break;}
          case NOR: {cpu->registers[cpu->operand] = ~(cpu->registers[0x0] | cpu->registers[0x1]); break;}
          case CMP: {cpu->registers[cpu->operand] += cpu->registers[0x0] == cpu->registers[0x1]; break;}
          case OVR: {cpu->registers[cpu->operand] += cpu->registers[0x0] > cpu->registers[0x1]; break;}

          case MWF: {cpu->registers[0x0] = cpu->registers[cpu->operand]; break;}
          case MWT: {cpu->registers[cpu->operand] = cpu->registers[0x0]; break;}
          case SET: {if (cpu->registers[0xD]&(1<<(cpu->operand%8))) {cpu->registers[0xD] -= 1<<(cpu->operand%8);} else {cpu->registers[0xD] += 1<<(cpu->operand%8);}; break;}

          case STW: {cpu->memory[cpu->registers[0xF] * cpu->MaddrSECT + (cpu->registers[0x0] % cpu->MaddrSECT)] = cpu->registers[cpu->operand]; break;}
          case LDW: {cpu->registers[cpu->operand] = cpu->memory[cpu->registers[0xF] * cpu->MaddrSECT + (cpu->registers[0x0] % cpu->MaddrSECT)]; break;}
          case STK: {if (!(cpu->registers[0xD]&8)) {cpu->memory[cpu->registers[0xB] * 0x100 + cpu->registers[0xC]] = cpu->registers[cpu->operand]; cpu->registers[0xC]++;} else {cpu->registers[0xC]--; cpu->registers[cpu->operand] = cpu->memory[cpu->registers[0xB] * 0x100 + cpu->registers[0xC]];} break;}
          case JNZ: {if (cpu->registers[0xD] & 1) {cpu->program_counter = cpu->registers[0xF] * cpu->MaddrSECT + (cpu->registers[cpu->operand] % cpu->MaddrSECT);} break;}    
          case IOB: {if (cpu->registers[0xD] & 2) {cpu->devices[cpu->registers[0xA] % cpu->devs] = cpu->registers[cpu->operand];} else {cpu->registers[cpu->operand] = cpu->devices[cpu->registers[0xA] % cpu->devs];} break;}
     }
     return;
}

/*
Registers:
0: Auto ins. input 1
1: Auto ins. input 2
2...A: General Store
B: I/O device selector
C: SP (stack pointer)
D: flags
E: Program memory sector
F: Memory sector
*/

int main(int argc, char *argv[]) {
    char mem[0xFF*0x100+0xFF];
    char reg[16];
    char dev[255];
    
    for (int i = 0; i < sizeof(mem); i++) mem[i] = 0;
    for (int i = 0; i < sizeof(reg); i++) reg[i] = 0;
    for (int i = 0; i < sizeof(reg); i++) reg[i] = 0;
    
    CPU cpu = {
            mem, reg, dev,
            sizeof(mem)/(float)0x100, 0xFF, 0x0,
            0,0,0,
            1
    };

     int programAddr = 0x0000;
     char program[] = {
0x93,
0xC1,
0xC0,
0x5D,
0x90,
0x92,
0x11,
0x92,
0x93,
0xC0,
0xC1,
0xD2,
0xF0
     };
     cpu.registers[0xB] = 0x01;
     cpu.registers[0xC] = 0x02;
     cpu.memory[0x0101] = 0x05;

     for (int i = programAddr; i < sizeof(program)+programAddr; i++) cpu.memory[i] = program[i-programAddr];
     cpu.program_counter = programAddr;

    //printRegisters(&cpu);
    int a;
    while (cpu.RUNNING) {
          fetch(&cpu);
          decode(&cpu);
          a = cpu.program_counter;
          execute(&cpu);
          
          //Print instruction & state of registers
          printf("INS: %02X [PC: %04X]\n", cpu.ins & 0xFF, (a-1) & 0xFFFF);
          printRegisters(&cpu);
          printf("\n");
    }
    //printRegisters(&cpu);
    printMemory(&cpu, 0x0100, 5);
    printf("\n");

    //PRINTING SCREEN
    for (int i = 0; i < 8; i+=2) {
          for (int n = 8; n > 0; n--) printf("%d", (cpu.devices[i] >> n-1) & 1);
          for (int n = 8; n > 0; n--) printf("%d", (cpu.devices[i+1] >> n-1) & 1);
          printf("\n");
    }

    return 0;
}
