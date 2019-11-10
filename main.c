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
#include <errno.h>
#include <ctype.h>

struct Instruction* enter_instructions(struct Instruction*);
void calculate_delay(struct Instruction*);
void print_chart(struct Instruction*, int);
void validate_input(char*);

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
		if (!scanf("%d", &choice)) { // Input must be of form "r#=r#+r#" or "R#=R#+R#". Registers are r0 through r9.
			printf("Count not read user input. Please enter an number 1-3. Now exitting.");
			exit(0);
		}

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
enter_instructions(struct Instruction * i_s_ptr)
{
	int num_of_ins = 0;
	char instruction_str[9];
	instruction_str[8] = '\0';

	printf("Enter number of instructions:\n");
	if (!scanf("%d", &num_of_ins)) {
		printf("Count not read user input. Please enter an number. Now exitting.");
		exit(0);
	}

	// Allocate array of instructions. Total # of instructions + 1 (instruction 0 used for dependency checking)
	int size = (num_of_ins + 1) * sizeof(struct Instruction);
	i_s_ptr = (struct Instruction*)malloc(size);
	if (i_s_ptr == NULL) {
		printf("Failed to allocate memory\n Error number %d\nNow Exitting program", errno);
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < num_of_ins; i++) { // Set all values of *next_i_ptr in the array of structs
		i_s_ptr[i].next_i_ptr = &i_s_ptr[i + 1];
	}
	i_s_ptr[num_of_ins].next_i_ptr = NULL;
	i_s_ptr[0].dr = -1; // All values = -1 to prevent false RAW dependency w/ instruction #2
	i_s_ptr[0].sr1 = -1;
	i_s_ptr[0].sr2 = -1;
	i_s_ptr[0].delay = -1;

	// Prompt user to input instructions & store the r(register) values in the dyanmic array(i_s_ptr)
	for (int i = 1; i <= num_of_ins; i++) {
		printf("%d) ", i);
		if (!scanf("%s", instruction_str)) {
			printf("Could not read user input, now exitting.");
			exit(0);
		}
		validate_input(instruction_str);
		printf("%s\n", instruction_str);

		i_s_ptr[i].dr = atoi(&instruction_str[1]);
		i_s_ptr[i].sr1 = atoi(&instruction_str[4]);
		i_s_ptr[i].sr2 = atoi(&instruction_str[7]);
	}
	return(i_s_ptr);
}

// Choice 2. Calculate delay of each instruction based on the dependencies on preivous instructions &
// where the 2 previous instructions ID & WB cylces are. Then call print_chart.
void
calculate_delay(struct Instruction* i_s_ptr)
{
	int total_num_of_cycles = 0, num_of_delays = 0, num_of_ins = 0, overlap = 2;
	const int NUM_OF_PIPELINE_STAGES = 5;

	while (i_s_ptr[num_of_ins].next_i_ptr != NULL) {
		num_of_ins++;
	}

	for (int i = 0; i < num_of_ins; i++) { // Set all instruction delays to 0
		i_s_ptr[i + 1].delay = 0;
	}

	// Calculate delay for each instruction by looking at the previous instruction & the instruciton
	// before the previous one. "overlap" variable keeps track of how many overlaps there are
	// (or cycles in) between the ID and WB stages. 
	for (int i = 2; i <= num_of_ins; i++) {
		if (i_s_ptr[i].sr1 == i_s_ptr[i - 1].dr || i_s_ptr[i].sr2 == i_s_ptr[i - 1].dr) {
			i_s_ptr[i].delay = 2;
			num_of_delays += 2;
			overlap = 0;
		}
		else if (i_s_ptr[i].sr1 == i_s_ptr[i - 2].dr || i_s_ptr[i].sr2 == i_s_ptr[i - 2].dr) {
			if (overlap == 2) {
				i_s_ptr[i].delay = 1;
				num_of_delays += 1;
				overlap = 1;
			}
		}
		else {
			overlap = 2;
		}
	}

	total_num_of_cycles = (NUM_OF_PIPELINE_STAGES + (num_of_ins - 1) + num_of_delays);
	printf("\nTotal number of cycles: %d\n\n", total_num_of_cycles);
	print_chart(i_s_ptr, num_of_ins);
	return;
}

// Each instruction will be alligned in the chart according to the current cycle & the number of bubbles.
// The ID (decode) stage depends on the values already written to the registers. Writting to registers is
// performed by the WB (Write Back) stage.
void
print_chart(struct Instruction * i_s_ptr, int num_of_ins)
{
	int total_num_of_bubbles = 0, num_of_tabs = 0;
	char stages_str[16] = "IF\tID\tEX\tMM\tWB\n";
	stages_str[15] = '\0';

	for (int i = 1; i <= num_of_ins; i++) {
		total_num_of_bubbles += i_s_ptr[i].delay;
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

void validate_input(char* i_str)
{
	if ((i_str[0] != 'r' && i_str[0] != 'R') ||
		(i_str[3] != 'r' && i_str[3] != 'R') ||
		(i_str[6] != 'r' && i_str[6] != 'R'))
	{
		printf("All register values need to be represented as 'r' or 'R'.\n");
		printf("Input needs to be of the form 'r#=r#+r#' or 'R#=R#+R#'. Now exitting program.\n");
		exit(0);
	}

	if (isdigit(i_str[1]) == 0 ||
		isdigit(i_str[4]) == 0 ||
		isdigit(i_str[7]) == 0)
	{
		printf("There is suppose to be a single digit number following all 'r' values.\n");
		printf("Input needs to be of the form 'r#=r#+r#' or 'R#=R#+R#'. Now exitting program.\n");
		exit(0);
	}
	return;
}