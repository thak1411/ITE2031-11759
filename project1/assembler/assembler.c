/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int rn_is_number(const char*, int, int);
int rn_atoi(const char*);
int check_register3(char*, char*, char*);
int check_register2(char*, char*);
int readAndParse(FILE *, char *, char *, char *, char *, char *);

int main(int argc, char *argv[]) 
{
	int i, address, print_machine_code, tmp;
	unsigned int machine_code;
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	char labels[65536][7] = { 0, };
	int label_addresses[65536] = { 0, };
	int label_count = 0;

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	// pre-calculate labels
	for (address = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); ++address) {
		if (label[0] != '\0') {
			if (strlen(label) > 6) {
				printf("error: label %s too long\n", label);
				exit(1);
			}
			for (i = 0; i < label_count; ++i) {
				if (!strcmp(labels[i], label)) {
					printf("error: duplicate label %s\n", label);
					exit(1);
				}
			}
			strcpy(labels[label_count], label);
			label_addresses[label_count] = address;
			++label_count;
		}
	}

	rewind(inFilePtr);

	// generate machine code & checking errors
	for (address = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); ++address) {
		machine_code = 0;
		print_machine_code = 1;
		if (!strcmp(opcode, "add")) {
			machine_code |= 0 << 22;
			machine_code |= check_register3(arg0, arg1, arg2);
		} else if (!strcmp(opcode, "nor")) {
			machine_code |= 1 << 22;
			machine_code |= check_register3(arg0, arg1, arg2);
		} else if (!strcmp(opcode, "lw")) {
			machine_code |= 2 << 22;
			machine_code |= check_register2(arg0, arg1);
		} else if (!strcmp(opcode, "sw")) {
			machine_code |= 3 << 22;
			machine_code |= check_register2(arg0, arg1);
		} else if (!strcmp(opcode, "beq")) {
			machine_code |= 4 << 22;
			machine_code |= check_register2(arg0, arg1);
		} else if (!strcmp(opcode, "jalr")) {
			machine_code |= 5 << 22;
			machine_code |= check_register2(arg0, arg1);
		} else if (!strcmp(opcode, "halt")) {
			machine_code |= 6 << 22;
		} else if (!strcmp(opcode, "noop")) {
			machine_code |= 7 << 22;
		} else if (!strcmp(opcode, ".fill")) {
			print_machine_code = 0;
			if (rn_is_number(arg0, -2147483648, 2147483647)) {
				fprintf(outFilePtr, "%d\n", atoi(arg0));
			} else {
				for (i = 0; i < label_count; ++i) {
					if (!strcmp(labels[i], arg0)) {
						fprintf(outFilePtr, "%d\n", label_addresses[i]);
						break;
					}
				}
				if (i == label_count) {
					printf("error: undefined label %s\n", arg0);
					exit(1);
				}
			}
		} else {
			printf("error: unrecognized opcode %s\n", opcode);
			exit(1);
		}
		if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) {
			tmp = rn_is_number(arg2, -32768, 32767);

			if (tmp == 0) { // label
				for (i = 0; i < label_count; ++i) {
					if (!strcmp(labels[i], arg2)) {
						tmp = label_addresses[i];
						break;
					}
				}
				if (i == label_count) {
					printf("error: use of undefined labels");
					exit(1);
				}
				if (!strcmp(opcode, "beq")) {
					tmp = tmp - (address + 1);
				}
				if (tmp > 32767 || tmp < -32768) {
					printf("error: offsetFields that does not fit in 16 bits");
					exit(1);
				}
				machine_code |= tmp & ((1U << 16U) - 1U);
			} else if (tmp == 1) { // constant
				tmp = rn_atoi(arg2);
				machine_code |= tmp & ((1U << 16U) - 1U);
			} else {
				printf("error: offsetFields that does not fit in 16 bits");
				exit(1);
			}
		}

		if (print_machine_code) {
			fprintf(outFilePtr, "%d\n", machine_code);
		}
	}

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return 0;
}

/*
 * Check Given String is a Number with Closed Ranges
 *
 * Range Must Represented by Integer
 * 
 * Return 1 If str is a Number
 * Return 0 If str is not a Number (Include str is Out of Integer Range)
 * Return -1 If str is Out of Range
 */
int rn_is_number(const char* str, int _min, int _max) {
	long long min, max, sign, v;
	const char* ptr;

	min = _min; max = _max;
	sign = 1;
	v = 0;

	ptr = str;
	if (*ptr == '-') {
		sign = -1;
		++ptr;
	} else if (*ptr == '+') ++ptr;

	for (; *ptr; ++ptr) {
		if (*ptr < '0' || *ptr > '9') return 0;
		v *= 10;
		v += *ptr - '0';
		if (v > 2147483647LL) return 0;
	}
	v *= sign;
	return v >= min && v <= max ? 1 : -1;
}

/*
 * Alpha to Integer. Assume str is Integer Value Represented String Format
 */
int rn_atoi(const char* str) {
	int v;

	sscanf(str, "%d", &v);
	return v;
}

/*
 * Check 3 Registers Argument is valid or not
 *
 * If the register is valid, return instruction code for registers
 */
int check_register3(char* arg0, char* arg1, char* arg2) {
	int a0, a1, a2;

	a0 = rn_is_number(arg0, 0, 7); a1 = rn_is_number(arg1, 0, 7); a2 = rn_is_number(arg2, 0, 7);
	if (a0 == 0 || a1 == 0 || a2 == 0) {
		printf("error: non-integer register arguments\n");
		exit(1);
	}
	if (a0 == -1 || a1 == -1 || a2 == -1) {
		printf("error: registers out of range [0, 7]\n");
		exit(1);
	}
	a0 = rn_atoi(arg0); a1 = rn_atoi(arg1); a2 = rn_atoi(arg2); 
	return (a0 << 19) | (a1 << 16) | a2;
}

/*
 * Check 2 Registers Argument is valid or not
 *
 * If the register is valid, return instruction code for registers
 */
int check_register2(char* arg0, char* arg1) {
	int a0, a1;

	a0 = rn_is_number(arg0, 0, 7); a1 = rn_is_number(arg1, 0, 7);
	if (a0 == 0 || a1 == 0) {
		printf("error: non-integer register arguments\n");
		exit(1);
	}
	if (a0 == -1 || a1 == -1) {
		printf("error: registers out of range [0, 7]\n");
		exit(1);
	}
	a0 = rn_atoi(arg0); a1 = rn_atoi(arg1);
	return (a0 << 19) | (a1 << 16);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}
