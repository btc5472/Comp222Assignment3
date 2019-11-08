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

struct Instruction * enterIns(struct Instruction*, int*);
void delayFunction(struct Instruction*, int);
void printChart();

// Define structure for instruction containing fields for destination register, 2 source registers, and individual
// instruction delay and a variable as pointer to structure for creating a dynamic array of instructions

// An Instuction consists of a destination register (dr) and 2 source registers (sr1 sr2) because were reading assembly
// code. The last variable i_delay is to track the number of bubbles/stalls to insert before the instruction.
struct Instruction {
	int dr, sr1, sr2, i_delay;
};

int main() {
	int choice = 0;
	int num_of_instructions = 0;
	struct Instruction *instruction_set_ptr = NULL;

	do {
		printf("Pipelined instruction performance\n\n");
		printf("1) Enter Instructions\n2) Determine when instructions are fetched\n3) Exit\n\n");
		printf("Enter Selection: ");
		scanf("%d", &choice);

		if (choice == 1) instruction_set_ptr = enterIns(instruction_set_ptr, &num_of_instructions);

		if (choice == 2) delayFunction(instruction_set_ptr, num_of_instructions);

		if (choice > 3 || choice < 1) printf("Please enter a valid choice 1-3\n");

	} while (choice != 3);
	
	printf("Program Terminated Normally");

	return 0;
}

// FUNCTION TO ENTER INSTRUCTIONS
struct Instruction *
enterIns (struct Instruction * iptr, int * num_of_ins)
{
	/* Declare local variables, including an array of characters to store user input */
	char str_instruction[9];
	str_instruction[8] = '0';

	/* Prompt for total number of instructions */
	printf("Enter number of instructions: ");
	scanf("%d", num_of_ins);

	/* Allocate memory to hold a set of instructions based on total number of instructions+1
	(instruction 0 used for dependency checking)*/
	int size = (*num_of_ins + 1) * sizeof(struct Instruction);
	iptr = (struct Instruction*)malloc(size);
	if (iptr == NULL) {
		printf("mem failed to allocate");
		exit(0);
	}

	/* Initialize instruction 0's destination register to -1 to prevent false RAW dependency w/ instruction 2 */
	iptr[0].dr = -1;

	/* For each instruction, prompt for user input with instruction number, such as: 1)
	and read instruction as a string and store at proper field of appropriate index within dynamic array of instructions */
	for (int i = 1; i <= *num_of_ins; i++) {
		printf("%d) ", i);
		if (!scanf("%s", str_instruction)) {
			printf("Invalid instruction, now exitting");
			exit(0);
		}
		printf("\n");

		// parse string to take out r values
		iptr[i].dr = atoi(&str_instruction[1]);
		iptr[i].sr1 = atoi(&str_instruction[4]);
		iptr[i].sr2 = atoi(&str_instruction[7]);
	}
	return(iptr);
}

// FUNCTION TO CALCULATE DELAY OF SET OF INSTRUCTIONS ON A 5 - STAGE PIPLELINE ARCHITECTURE
void
delayFunction (struct Instruction* iptr, int num_of_ins)
{
	/* Declare local variables */

	/* For each instruction i from 2 to total number of instructions, initialize delay as 0 and check for dependency
	between instruction (i-2) and i, as well as between instruction (i-1) and i */
	for (int i = 0; i < num_of_ins; i++) {
		printf("%d", iptr[i].dr);
	}

	{ /* begin for-loop */

	/* If dependency,  set delay appropriately, as well
	as a flag to check for possible overlap between dependent instructions */

	/* Note: general formula for delay:
	delay=2 if instruction i depends on instruction (i-1)
	delay=1 if instruction i depends on instruction (i-2) and no overlap of dependencies
	delay=0 otherwise
	*/

	/* Calculate individual delay for current instruction */

	} /* end for-loop */

	/* print chart */
	printChart(iptr, num_of_ins);
	return;
}

// FUNCTION TO PRINT OUT CHART OF INSTRUCTIONS WITH CYCLES WHEN EACH INSTRUCTION IS FETCHED
void
printChart(struct Instruction * iptr, int num_of_ins)
{
	int num_of_cycles = 0, num_of_pipeline_stages = 0;
	char stages_str[16] = "IF\tID\tEX\tMM\tWB\n";
	stages_str[15] = '0';

	// Align each insruction with its cycles according to delay num
	// and print out stages with proper tabbing (IF\tID\tEX\tMM\tWB)
	num_of_cycles = num_of_pipeline_stages + num_of_ins - 1;
	printf("Total number of cycles: %d\n\n", num_of_cycles);
	for (int c = 1; c <= num_of_cycles; c++) {
		printf("Instruction %d Fetched at Cycle %d\n", num_of_cycles, num_of_cycles);
		printf("%d) ", num_of_cycles);
		// Insert bubbles/stalls (represented as tabs)
		for (int b = 0; b < iptr[b + 1].i_delay; b++) {
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