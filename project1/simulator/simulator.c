/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

int main(int argc, char *argv[])
{
    int is_halted, ir, opcode, reg_a, reg_b, reg_dest, offset_field, executed_instructions;
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    state.pc = 0;
    memset(state.reg, 0, sizeof state.reg);

    for (executed_instructions = 0; ; ) {
        printState(&state);
        ir = state.mem[state.pc++];
        ++executed_instructions;

        opcode = ir >> 22;
        reg_a = (ir >> 19) & 0x7;
        reg_b = (ir >> 16) & 0x7;
        reg_dest = ir & 0x7;
        offset_field = convertNum(ir & 0xFFFF);
        if (opcode == 0) { // add
            state.reg[reg_dest] = state.reg[reg_a] + state.reg[reg_b];
        } else if (opcode == 1) { // nor
            state.reg[reg_dest] = ~(state.reg[reg_a] | state.reg[reg_b]);
        } else if (opcode == 2) { // lw
            state.reg[reg_b] = state.mem[state.reg[reg_a] + offset_field];
        } else if (opcode == 3) { // sw
            state.mem[state.reg[reg_a] + offset_field] = state.reg[reg_b];
        } else if (opcode == 4) { // beq
            state.pc += (state.reg[reg_a] == state.reg[reg_b]) ? offset_field : 0;
        } else if (opcode == 5) { // jalr
            state.reg[reg_b] = state.pc;
            state.pc = state.reg[reg_a];
        } else if (opcode == 6) { // halt
            break;
        } else if (opcode == 7) { // noop
            continue;
        } else {
            printf("error: invalid opcode\n");
            exit(1);
        }
    }

    printf("machine halted\n");
    printf("total of %d instructions executed\n", executed_instructions);
    printf("final state of machine:\n");

    printState(&state);
    return 0;
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}
