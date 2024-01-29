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
	char* cmd;		   // arg 1
	char* arg;		   // arg 2, if not an option
	char* opt_n;	   // -n [string]
	char* opt_due;     // -due [string]
	char* opt_due_day; // -due-day [string]
	int opt_i;         // -i
} cmdinfo;

// Typedef struct tm from time.h library to just tm
typedef struct tm tm;

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
		cmd -> opt_due = NULL;
		cmd -> opt_due_day = NULL;
		cmd -> opt_i = 0;
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
			for(int i = index; i < argc; i ++) {
				if(!strcmp(argv[i], "-n")) {
					// -n [note]
					if (i == argc - 1) {
						printf("todo: no argument provided for -n [note]\n");
						exit(1);
					}

					cmd -> opt_n = argv[++ i];
				} else if (!strcmp(argv[i], "-due")) {
					// -due [date]
					if (i == argc - 1) {
						printf("todo: no argument provided for -due [date]\n");
						exit(1);
					}

					cmd -> opt_due = argv[++ i];
				} else if (!strcmp(argv[i], "-due-day")) {
					// -due-day [day]
					if (i == argc - 1) {
						printf("todo: no argument provided for -due-day [day]\n");
						exit(1);
					}

					cmd -> opt_due_day = argv[++ i];
				} else if (!strcmp(argv[i], "-i")) {
					cmd -> opt_i = 1;
				}
			}
		}

		return cmd;
	} else {
		return NULL;
	}
}

/*
 * Returns the current time as a tm struct
 */
tm* get_curr_time() {
	// Get current time
	time_t curr_time = time(NULL);
	// Convert to tm* and return
	return localtime(&curr_time);
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
 * If info is 1, print extra information.
 *
 * Returns nothing.
 */
void list_tasks(int info) {
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
		// For next 5 lines, if info = 1, print extra information
		// Otherwise skips to next task
		for (int i = 0; i < 5; i ++) {
			length = getline(&buffer, &size, file);

			if (info == 1) {
				switch (i) {
					case 0:
						printf(" - created: %s", buffer);
						break;
					case 1:
						if (strlen(buffer) > 1) {
							printf(" - note:    %s", buffer);
						}

						break;
					case 2:
						if (strlen(buffer) > 1) {
							printf(" - due:     %s", buffer);
						}

						break;
				}
			}
		}
		// Increase the current task#
		index ++;
	}

	fclose(file);
}

int main(int argc, char* argv[]) {
	// Get command info given argc and argv
	cmdinfo* cmd = get_cmd_info(argc, argv);

	if (cmd != NULL) {
		if (!strcmp(cmd -> cmd, "ls")) {
			// todo ls
			// List all tasks
			list_tasks(cmd -> opt_i);
		} else {
			// If first argument provided does not match with a valid command,
			// create task with arg1 (cmd -> cmd) as the name
			//
			// ex. todo "physics homework"
			todo_add(cmd -> cmd, cmd -> opt_n, cmd -> opt_due);
		}
		// cmd was manually allocated, so we must free it
		free(cmd);
	} else {
		// No command provided
		printf("todo: no action supplied\n");
	}
}
