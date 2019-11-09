/**********************************************************************************************************************
Br&on Cobb
Assignment 3 - Pipelining
November 7, 2019
Comp Org 222 9:30am

This program calculates the performance of a program with dependent arithmetic instructions by simulating the
execution on a simple 5-stage pipeline architecture without forwarding capabilities. Registers can be read
immediately after they are written in the same cycle.

r0=r1+r2
r5=r6+r0
r3=r4+r0

This breaks the calculate delay function. Program thinks that there needs to be a delay for ins 3 when there doesnt
Ins 2 already put 2 bubbles in the pipeline which was enough to bring ins 1 WB in line with ins 3 ID
Line 119
**********************************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct Instruction* enter_instructions(struct Instruction*);
void calculate_delay(struct Instruction*);
void print_chart(struct Instruction*, int);

// An Instuction consists of a destination register (dr) & 2 source registers (sr1 sr2) because we're reading assembly
// code. The variable "delay" is to track the number of bubbles/stalls to insert before the instruction. The pointer
// points to the next instruction in the dynamic array.
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
		printf("Enter Selection:\n");
		scanf("%d", &choice); // Input has to be of the form "r#=r#+r#"

		if (choice == 1) instruction_set_ptr = enter_instructions(instruction_set_ptr);

		if (choice == 2) calculate_delay(instruction_set_ptr);

		if (choice > 3 || choice < 1) printf("Please enter a valid choice 1-3\n");

	} while (choice != 3);
	
	free(instruction_set_ptr);
	printf("Program Terminated Normally");

	return 0;
}

// Choice 1. Enter instructions, allocate array of structs, & store instructions in array
struct Instruction*
enter_instructions (struct Instruction * iptr)
{
	int num_of_ins = 0;
	char str_instruction[9];
	str_instruction[8] = '0';

	printf("Enter number of instructions:\n");
	scanf("%d", &num_of_ins);

	// Allocate array of instructions. Total # of instructions + 1 (instruction 0 used for dependency checking)
	int size = (num_of_ins + 1) * sizeof(struct Instruction);
	iptr = (struct Instruction*)malloc(size);
	if (iptr == NULL) {
		printf("Failed to allocate memory\n Error number %d\nNow Exitting program", errno);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < num_of_ins; i++) { // Set all values of *next_i_ptr in the array of structs
		iptr[i].next_i_ptr = &iptr[i + 1];
	}
	iptr[num_of_ins].next_i_ptr = NULL;
	iptr[0].dr = -1; // All values = -1 to prevent false RAW dependency w/ instruction #2
	iptr[0].sr1 = -1;
	iptr[0].sr2 = -1;
	iptr[0].delay = -1;

	// Prompt user to input instructions & store the r(register) values in the dyanmic array(iptr)
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

// Calculate delay of each instruction based on the dependencies on preivous instructions & call print_chart
void
calculate_delay (struct Instruction* iptr)
{
	int total_num_of_cycles = 0, num_of_delays = 0, num_of_ins = 0;
	const int NUM_OF_PIPELINE_STAGES = 5;

	while (iptr[num_of_ins].next_i_ptr != NULL) {
		num_of_ins++;
	}

	// Set all instruction delays to 0
	for (int i = 0; i < num_of_ins; i++) {
		iptr[i + 1].delay = 0;
	}

	// Calculate delay for each instruction by looking at the previous instruction
	// & the instruciton before the previous one
	for (int i = 2; i <= num_of_ins; i++) {
		if (iptr[i].sr1 == iptr[i - 1].dr || iptr[i].sr2 == iptr[i - 1].dr) {
			iptr[i].delay = 2;
			num_of_delays += 2;
		}
		else if (iptr[i].sr1 == iptr[i - 2].dr || iptr[i].sr2 == iptr[i - 2].dr) {
			iptr[i].delay = 1;
			num_of_delays += 2;
		}
	}

	total_num_of_cycles = (NUM_OF_PIPELINE_STAGES + (num_of_ins - 1) + num_of_delays);
	printf("\nTotal number of cycles: %d\n\n", total_num_of_cycles);
	print_chart(iptr, num_of_ins);
	return;
}

// Each instruction will be alligned in the chart according to the current cycle * the number of bubbles.
// The ID (decode) stage depends on the values already written to the registers. Writting to registers is
// performed by the WB (Write Back) stage.
void
print_chart(struct Instruction * iptr, int num_of_ins)
{
	int total_num_of_bubbles = 0, num_of_tabs = 0;
	char stages_str[16] = "IF\tID\tEX\tMM\tWB\n";
	stages_str[15] = '\0';

	for (int i = 1; i <= num_of_ins; i++) {
		total_num_of_bubbles += iptr[i].delay;
		num_of_tabs = i + total_num_of_bubbles;
		printf("Instruction %d is Fetched at Cycle %d\n\n", i, i + total_num_of_bubbles);
		printf("%d) ", i);
		for (int j = 0; j < num_of_tabs; j++) {
			printf("\t");
		}
		printf("%s\n", stages_str);
	}
	printf("\n");
	return;
}


// TODO: Validate input for instructions