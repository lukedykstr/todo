/*
 * todo.c
 *
 * A basic command line program that creates and manages tasks
 * with information such as notes and due dates. All task data
 * is stored in todo.txt.
 *
 * Written by Luke Dykstra 1/29/24
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"

// Struct that holds information about the command and arguments
// provided when the program is run from the command line
typedef struct cmdinfo {
	char* cmd;   // arg 1
	char* arg;   // arg 2, if not an option
	char* opt_n; // -n [string]
	char* opt_d; // -d [string
} cmdinfo;

/* 
 * Given argc and argv from main, create a struct that holds
 * parsed information about the arguments from the command line
 *
 * Returns pointer to a cmdinfo struct that holds the information,
 * or NULL if no command was provided
 */
cmdinfo* get_cmd_info(int argc, char* argv[]) {
	if (argc > 1) {
		// Create new cmdinfo struct to store info
		cmdinfo* cmd = malloc(sizeof(cmdinfo));
		// Default all options to NULL
		cmd -> arg = NULL;
		cmd -> opt_n = NULL;
		cmd -> opt_d = NULL;
		// Get the command name
		cmd -> cmd = argv[1];
		// Get any options provided
		if (argc > 2) {
			int index = 2;
			// If first argument is not an option, add it to cmd -> arg
			if (argv[2][0] != '-') {
				cmd -> arg = argv[2];
				index ++;
			}
			// Check all arguments provided to see if they are a valid option
			for(int i = index; i < argc - 1; i ++) {
				if(!strcmp(argv[i], "-n")) {
					// -n [note]
					cmd -> opt_n = argv[++ i];
				} else if (!strcmp(argv[i], "-d")) {
					// -d [date]
					cmd -> opt_d = argv[++ i];
				}
			}
		}

		return cmd;
	} else {
		return NULL;
	}
}

/*
 * Creates a task given the message / task name, note, and due date.
 * Set note or due date to NULL if none provided.
 *
 * Returns nothing
 */
void todo_add(char* msg, char* note, char* due) {
	FILE* file = fopen("todo.txt", "a+");

	if (file == NULL) {
		printf("todo: failure to create or open todo file\n");
		exit(1);
	}
	// Get the current time as a string
	time_t curr_time = time(NULL);
	char* timestr = ctime(&curr_time);

	// Task format:
	// name
	// time created
	// (note) OR \n if none
	// (due date) OR \n if none
	// end
	fputs(msg, file);
	fputs("\n", file);
	fputs(timestr, file);

	if (note != NULL) {
		fputs(note, file);
	}

	fputs("\n", file);

	if (due != NULL) {
		fputs(due, file);
	}

	fputs("\n", file);
	fputs("end\n", file);
	fclose(file);

	printf("task created\n");
}

/*
 * List all tasks stored in todo.txt.
 *
 * Returns nothing.
 */
void list_tasks() {
	FILE* file = fopen("todo.txt", "r+");

	if (file == NULL) {
		printf("todo: failure to create or open todo file\n");
		exit(1);
	}
	// Index of the task to print (not line number, but task #)
	int index = 1;
	// Buffer string to store next line of file
	char* buffer = NULL;
	size_t size = 0;
	// Length of the next line
	int length;

	// Get the next line of the file
	length = getline(&buffer, &size, file);
	// Repeat while next line exists (EOF not reached)
	while (length != -1) {
		// Print task# and task message stored in buffer
		printf("%d: %s", index, buffer);
		// Skip task date, note, due date, end, and move to next task
		// In total, skip 5 lines
		for (int i = 0; i < 5; i ++) {
			length = getline(&buffer, &size, file);
		}
		// Increase the task#
		index ++;
	}

	fclose(file);
}

int main(int argc, char* argv[]) {
	// Get command info given argc and argv
	cmdinfo* cmd = get_cmd_info(argc, argv);

	if (cmd != NULL) {
		if (!strcmp(cmd -> cmd, "all")) {
			// todo all
			// List all tasks
			list_tasks();
		} else {
			// If first argument provided does not match with a valid command,
			// create task with arg1 (cmd -> cmd) as the name
			//
			// ex. todo "physics homework"
			todo_add(cmd -> cmd, cmd -> opt_n, cmd -> opt_d);
		}
		// cmd was manually allocated, so we must free it
		free(cmd);
	} else {
		// No command provided
		printf("todo: no action supplied\n");
	}
}
