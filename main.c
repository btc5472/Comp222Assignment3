/**********************************************************************************************************************
Brandon Cobb
Assignment 3 - Pipelining
November 7, 2019
Comp Org 222 9:30am

This program calculates the performance of a program with dependent arithmetic instructions by simulating the
execution on a simple 5-stage pipeline architecture without forwarding capabilities. Registers can be read
immediately after they are written in the same cycle.
**********************************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

struct Instruction* enterIns(struct Instruction*);
void delayFunction(struct Instruction*);
void printChart(struct Instruction*, int);

// Define structure for instruction containing fields for destination register, 2 source registers, and individual
// instruction delay and a variable as pointer to structure for creating a dynamic array of instructions

// An Instuction consists of a destination register (dr) and 2 source registers (sr1 sr2) because were reading assembly
// code. The last variable delay is to track the number of bubbles/stalls to insert before the instruction.
struct Instruction {
	int dr, sr1, sr2, delay;
	struct Instruction  *next_i_ptr;
};

int main() {
	int choice = 0;
	struct Instruction *instruction_set_ptr = NULL;

	do {
		printf("Pipelined instruction performance\n\n");
		printf("1) Enter Instructions\n2) Determine when instructions are fetched\n3) Exit\n\n");
		printf("Enter Selection: ");
		scanf("%d", &choice);

		if (choice == 1) instruction_set_ptr = enterIns(instruction_set_ptr);

		if (choice == 2) delayFunction(instruction_set_ptr);

		if (choice > 3 || choice < 1) printf("Please enter a valid choice 1-3\n");

	} while (choice != 3);
	
	free(instruction_set_ptr);
	printf("Program Terminated Normally");

	return 0;
}

// FUNCTION TO ENTER INSTRUCTIONS
struct Instruction*
enterIns (struct Instruction * iptr)
{
	int num_of_ins = 0;
	char str_instruction[9];
	str_instruction[8] = '0';

	printf("Enter number of instructions: ");
	scanf("%d", &num_of_ins);

	// Allocate array of instructions. Total number of instructions + 1 (instruction 0 used for dependency checking)
	int size = (num_of_ins + 1) * sizeof(struct Instruction);
	iptr = (struct Instruction*)malloc(size);
	if (iptr == NULL) {
		printf("mem failed to allocate");
		exit(0);
	}
	for (int i = 0; i < num_of_ins; i++) { // Set all values of *next_i_ptr in the array of structs
		iptr[i].next_i_ptr = &iptr[i + 1];
	}
	iptr[num_of_ins].next_i_ptr = NULL;
	iptr[0].dr = -1; // All values = -1 to prevent false RAW dependency w/ instruction #2
	iptr[0].sr1 = -1;
	iptr[0].sr2 = -1;
	iptr[0].delay = -1;

	// Prompt user to input each instruction and store the r(register) values in the dyanmic array(iptr)
	for (int i = 1; i <= num_of_ins; i++) {
		printf("%d) ", i);
		if (!scanf("%s", str_instruction)) {
			printf("Invalid instruction, now exitting");
			exit(0);
		}
		printf("\n");

		iptr[i].dr = atoi(&str_instruction[1]);
		iptr[i].sr1 = atoi(&str_instruction[4]);
		iptr[i].sr2 = atoi(&str_instruction[7]);
	}
	return(iptr);
}

// FUNCTION TO CALCULATE DELAY OF SET OF INSTRUCTIONS ON A 5 - STAGE PIPLELINE ARCHITECTURE
void
delayFunction (struct Instruction* iptr)
{
	int total_num_of_cycles = 0, num_of_delays = 0;
	int num_of_ins = 0;

	while (iptr[num_of_ins].next_i_ptr != NULL) {
		num_of_ins++;
	}

	/* For each instruction i from 2 to total number of instructions, initialize delay as 0 and check for dependency
	between instruction (i-2) and i, as well as between instruction (i-1) and i */
	// Set all instruction delays to 0
	for (int i = 0; i < num_of_ins; i++) {
		iptr[i + 1].delay = 0;
	}

	for (int i = 2; i <= num_of_ins; i++) {
		/* If dependency,  set delay appropriately, as well
		as a flag to check for possible overlap between dependent instructions */
		if (iptr[i].sr1 == iptr[i - 1].dr || iptr[i].sr2 == iptr[i - 1].dr) {
			iptr[i].delay = 2;
			//overlap = 1;
			num_of_delays += 2;
		}
		else if (iptr[i].sr1 == iptr[i - 2].dr || iptr[i].sr2 == iptr[i - 2].dr) {
			iptr[i].delay = 1;
			//overlap = 1;
			num_of_delays += 2;
		}
		else {
			iptr[i].delay = 0;
		}

		/* Note: general formula for delay:
		delay=2 if instruction i depends on instruction (i-1)
		delay=1 if instruction i depends on instruction (i-2) and no overlap of dependencies
		delay=0 otherwise
		*/

		/* Calculate individual delay for current instruction */
	}

	total_num_of_cycles = (5 + (num_of_ins - 1) + num_of_delays);
	printf("\nTotal number of cycles: %d\n\n", total_num_of_cycles);

	printChart(iptr, num_of_ins);
	return;
}

// FUNCTION TO PRINT OUT CHART OF INSTRUCTIONS WITH CYCLES WHEN EACH INSTRUCTION IS FETCHED
void
printChart(struct Instruction * iptr, int num_of_ins)
{
	int num_of_pipeline_stages = 5, num_of_bubbles = 0, num_of_tabs = 0;
	char stages_str[16] = "IF\tID\tEX\tMM\tWB\n";
	stages_str[15] = '\0';

	// Align each insruction with its cycles according to delay num
	// and print out stages with proper tabbing (IF\tID\tEX\tMM\tWB)
	for (int i = 1; i <= num_of_ins; i++) {
		num_of_bubbles = iptr[i].delay;
		num_of_tabs = i + num_of_bubbles;
		printf("Instruction %d Fetched at Cycle %d\n\n", i, i + num_of_bubbles);
		printf("%d) ", i);
		// Insert bubbles/stalls (represented as tabs)
		for (int j = 0; j < num_of_tabs; j++) {
			printf("\t");
		}
		printf("%s\n", stages_str);
	}
	return;
}



// TODO: Free mem
// TODO: Validate input for instructions



// OLD CODE
//int size = (num_of_ins + 1) * sizeof(struct Instruction);
//struct Instruction* array_ptr = malloc(size);
//int x = sizeof(array_ptr[0]);
//if (array_ptr == NULL) {
//	printf("mem failed to allocate");
//	exit(0);
//}




//for (int b = 1; b <= i; b++) {
//	printf("\t");
//	if (iptr[b].delay != 0) {
//		for (int c = 0; c < iptr[b].delay; c++) {
//			printf("\t");
//		}
//	}
//}