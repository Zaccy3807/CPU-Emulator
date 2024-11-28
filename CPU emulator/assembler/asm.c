#include <stdio.h>

char instructions[][3] = {"add", "min", "and", "nor", "cmp", "ovr", "mwf", "mwt", "set", "stw", "ldw", "stk", "jnz", "iob", "qut"};

char ASM2ins(char *ins, int size) {
    if (size < 5) {printf("INVALID SIZE OF INSTRUCTION\n"); return 0x00;}
    char opcode = 0;
    char operand = 0;
    for (int i = 0; i < 16; i++) {
        if (ins[0] == instructions[i][0] && ins[1] == instructions[i][1] && ins[2] == instructions[i][2]) opcode = (i+1) << 4;
    }
    switch (ins[4]) {
        case '1': {operand = 0x1; break;}
        case '2': {operand = 0x2; break;}
        case '3': {operand = 0x3; break;}
        case '4': {operand = 0x4; break;}
        case '5': {operand = 0x5; break;}
        case '6': {operand = 0x6; break;}
        case '7': {operand = 0x7; break;}
        case '8': {operand = 0x8; break;}
        case '9': {operand = 0x9; break;}
        case 'A': {operand = 0xA; break;}
        case 'B': {operand = 0xB; break;}
        case 'C': {operand = 0xC; break;}
        case 'D': {operand = 0xD; break;}
        case 'E': {operand = 0xE; break;}
        case 'F': {operand = 0xF; break;}
    }
    return opcode+operand;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {printf("Invalid arg. count!\n"); return 1;}
    char ins[255];
    FILE *toread = fopen(argv[1], "r");
    char read = 1;
    while (read) {
            fgets(ins, sizeof(ins), toread);
            if (ins[0] != '\n' && ins[0] != '#') {
                if (ins[0] == 'E' && ins[1] == 'N' && ins[2] == 'D') return 0;
                printf("0x%X,\n", ASM2ins(ins, 10) & 0xFF);
            }
    }
    return 0;
}