/*
 * todo.c
 *
 * A basic command line program that creates and manages tasks
 * with information such as notes and due dates. All task data
 * is stored in file .todo
 *
 * Written by Luke Dykstra 1/29/24
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "parse.h"

// Struct that holds information about the command and arguments
// provided when the program is run from the command line
typedef struct cmdinfo {
	char* cmd;		       // arg 1
	char* arg;		      // arg 2, if not an option
	char* opt_n;	     // -n [note]
	char* opt_due;      // -due [due date]
	char* opt_due_day; // -due-day [day]
	int opt_i;        // -i
	char* opt_c;     // -c [category]
	char* opt_cr;   // -cr [creation date]
	int opt_com;   // -com
} cmdinfo;

// Typedef struct tm from time.h library to just tm
typedef struct tm tm;

// Global var to hold current task index used in get_next_task()
int task_index = 0;

/* 
 * Given argc and argv from main, create a struct that holds
 * parsed information about the arguments from the command line.
 *
 * /!\ The data is not copied, they are the same pointers provided in argv /!\
 *
 * Returns pointer to a cmdinfo struct that holds the information,
 * or NULL if no command was provided
 */
cmdinfo* get_cmd_info(int argc, char* argv[]) {
	if (argc > 1) {
		// Create new cmdinfo struct to store info
		cmdinfo* cmd = malloc(sizeof(cmdinfo));
		// Default all options to NULL or 0
		cmd -> arg = NULL;
		cmd -> opt_n = NULL;
		cmd -> opt_due = NULL;
		cmd -> opt_due_day = NULL;
		cmd -> opt_i = 0;
		cmd -> opt_c = NULL;
		cmd -> opt_com = 0;
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
						free(cmd);
						exit(1);
					}

					cmd -> opt_n = argv[++ i];
				} else if (!strcmp(argv[i], "-due")) {
					// -due [date]
					if (i == argc - 1) {
						printf("todo: no argument provided for -due [date]\n");
						free(cmd);
						exit(1);
					}

					cmd -> opt_due = argv[++ i];
				} else if (!strcmp(argv[i], "-due-day")) {
					// -due-day [day]
					if (i == argc - 1) {
						printf("todo: no argument provided for -due-day [day]\n");
						free(cmd);
						exit(1);
					}

					cmd -> opt_due_day = argv[++ i];
				} else if (!strcmp(argv[i], "-i")) {
					// -i
					cmd -> opt_i = 1;
				} else if (!strcmp(argv[i], "-c")) {
					// -c [category]
					if (i == argc - 1) {
						printf("todo: no argument provided for -c [category]\n");
						free(cmd);
						exit(1);
					}

					cmd -> opt_c = argv[++ i];
				} else if (!strcmp(argv[i], "-cr")) {
					// -cr [creation date]
					if (i == argc - 1) {
						printf("todo: no argument provided for -cr [date created]\n");
						free(cmd);
						exit(1);
					}

					cmd -> opt_cr = argv[++ i];	
				} else if (!strcmp(argv[i], "-com")) {
					// -com
					cmd -> opt_com = 1;
				}
			}
		}

		return cmd;
	} else {
		return NULL;
	}
}

/*
 * Returns the current time as a tm struct.
 */
tm* get_curr_time() {
	// Get current time
	time_t curr_time = time(NULL);
	// Convert to tm* and return
	return localtime(&curr_time);
}

/*
 * Given a non-specific date string (ex. "Tomorrow", "Tuesday", etc...), return a
 * specific, normalized system date.
 *
 * If input date is valid, returns a string; otherwise returns NULL.
 */
char* norm_date(char* datein) {
	int tlength = strlen(datein) * 2 + 2;
	parray tokens;
	parse(&tokens, datein);

	for (int i = 0; i < tlength; i ++) {
		if (!strcmp(tokens[i], "END")) { break; }
		printf("%d: %s\n", i, tokens[i]);
	}

	return "";
}

/*
 * Creates a task given the message / task name, note, and due date.
 * Set note, due date, or category to NULL if none provided.
 *
 * Returns nothing.
 */
void todo_add(char* msg, char* note, char* due, char* cat) {
	FILE* file = fopen(".todo", "a+");

	if (file == NULL) {
		printf("todo: failure to create or open .todo file\n");
		fclose(file);
		exit(1);
	}

	if (msg[0] == '-') {
		printf("todo: task name cannot start with '-'\n");
		fclose(file);
		exit(1);
	}

	if (atoi(msg) != 0) {
		printf("todo: task name cannot be a number\n");
		fclose(file);
		exit(1);
	}

	// Get the current time as a string
	time_t curr_time = time(NULL);
	char* timestr = ctime(&curr_time);

	// Task format:
	// name
	// time created
	// (category) OR \n if none
	// (note)     OR \n if none
	// (due date) OR \n if none
	fputs(msg, file);
	fputs("\n", file);
	fputs(timestr, file);

	if (cat != NULL) {
		fputs(cat, file);
	}

	fputs("\n", file);

	if (note != NULL) {
		fputs(note, file);
	}

	fputs("\n", file);

	if (due != NULL) {
		fputs(due, file);
	}

	fputs("\n", file);

	fclose(file);

	printf("task created\n");
}

/*
 * Deletes a task given either its index or name as a string.
 * Checks if input string can be converted to a valid integer, if so,
 * it will delete the task at that index. Otherwise, it will delete 
 * the task with that name.
 *
 * Returns the deleted task as a string.
 */
char* task_delete(char* instr) {
	FILE* file = fopen(".todo", "r+");

	if (file == NULL) {
		printf("todo: failure to create or open .todo file\n");
		exit(1);
	}

	FILE* temp = fopen(".tdtemp", "w+");

	if (temp == NULL) {
		printf("todo: failure to create or open temp file\n");
		exit(1);
	}

	// If instr represents a valid integer, trgt_index will be a value
	// other than 0
	int trgt_index = atoi(instr);
	// String to store deleted task info
	char* deltsk = malloc(1);

	char* line = NULL;
	size_t size = 0;
	int length;
	int curr_index = 1;
	
	length = getline(&line, &size, file);

	while (length != -1) {
		// Remove newline at the end of line so we can compare it
		line[strlen(line) - 1] = '\0';

		// If trgt_index is not 0 and current index is not target index, keep this task
		// OR if trgt_index is 0 and current task name is not target task name (instr), keep this task
		// Keep the task by appending it to the temp file, delete it by skipping it
		if ((trgt_index != 0 && curr_index != trgt_index) ||
				(trgt_index == 0 && strcmp(line, instr))) {
			line[strlen(line)] = '\n';

			for(int i = 0; i < 5; i ++) {
				fputs(line, temp);
				length = getline(&line, &size, file);
			}
		} else {
			line[strlen(line)] = '\n';
			// Skip this task, store to deleted task info string
			for(int i = 0; i < 5; i ++) {
				deltsk = realloc(deltsk, strlen(deltsk) + strlen(line));
				strcat(deltsk, line);
				length = getline(&line, &size, file);
			}
		}
		
		// Increment current task index
		curr_index ++;
	}
	
	fclose(file);
	fclose(temp);
	
	// Delete old todo file, make temp the new todo file
	remove(".todo");
	rename(".tdtemp", ".todo");

	printf("task deleted\n");

	return deltsk;
}

/*
 * List all tasks with given category, creation date, or due date.
 * Set these arguments to NULL to ignore and list all.
 *
 * If info is 1, print extra information.
 * If com is 1, print completed tasks.
 *
 * Returns nothing.
 */
void list_tasks(int info, char* cat, char* crdate, char* duedate, int com) {
	FILE* file = fopen(com? ".todo-com" : ".todo", "r+");

	if (file == NULL) {
		printf("todo: failure to create or open %s file\n", com? ".todo-com" : ".todo");
		exit(1);
	}

	char* line = NULL;
	size_t size = 0;
	int length;

	char* tsname;
	char* tscrdate;
	char* tscat;
	char* tsnote;
	char* tsdue;

	int match;
	int index = 1;

	length = getline(&line, &size, file);

	while (length != -1) {
		// Store line info in the corresponding variables
		// Task format:
		// name
		// date created
		// (cat)
		// (note)
		// (due date)
		
		// Get task name, line 1
		tsname = malloc(sizeof(char) * strlen(line) + 1);
		strcpy(tsname, line);
		// Get date created, line 2
		getline(&line, &size, file);
		tscrdate = malloc(sizeof(char) * strlen(line) + 1);
		strcpy(tscrdate, line);
		// Get category, line 3
		getline(&line, &size, file);
		tscat = malloc(sizeof(char) * strlen(line) + 1);
		strcpy(tscat, line);
		// Get note, line 4
		getline(&line, &size, file);
		tsnote = malloc(sizeof(char) * strlen(line) + 1);
		strcpy(tsnote, line);
		// Get due date, line 5
		getline(&line, &size, file);
		tsdue = malloc(sizeof(char) * strlen(line) + 1);
		strcpy(tsdue, line);
		
		// Remove the newline character by replacing it with '\0'
		// For printing and comparision purposes
		tscrdate[strlen(tscrdate) - 1] = '\0';
		tscat[strlen(tscat) - 1] = '\0';
		tsdue[strlen(tsdue) - 1] = '\0';
		
		// By default, print task
		match = 1;
		
		// If a criteria is provided (not NULL), check if it matches with the task info
		// If not, set match to 0 so we can skip printing it
		if (cat != NULL && strcmp(cat, tscat))          { match = 0; }
		if (crdate != NULL && strcmp(crdate, tscrdate)) { match = 0; }
		if (duedate != NULL && strcmp(duedate, tsdue))  { match = 0; }

		if (match) {
			printf("%d: %s", index, tsname);
			// If info option is true, print task category, creation date, due date, and note
			if (info) {
				if (strlen(tscat) > 1) {
					printf("  <%s>\n", tscat);
				}

				printf("  created on %s\n", tscrdate);

				if (strlen(tsdue) > 1) {
					printf("  due on %s\n", tsdue);
				}

				if (strlen(tsnote) > 1) {
					printf("  - %s", tsnote);
				}
			}
		}

		free(tsname);
		free(tscrdate);
		free(tscat);
		free(tsnote);
		free(tsdue);

		length = getline(&line, &size, file);
		index ++;
	}
	
	fclose(file);
}

/*
 * Completes a task given either its name or index as a string.
 * Removes it from the main file and adds it to .todo-com.
 *
 * Returns nothing.
 */
void task_complete(char* instr) {
	FILE* comfile = fopen(".todo-com", "a+");

	if (comfile == NULL) {
		printf("todo: failure to create or open file .todo-com\n");
		exit(1);
	}

	char* deltsk = task_delete(instr);
	fputs(deltsk, comfile);

	char* msgs[] = {"congrats!", "well done!", "good job!", "great work!", "I knew you could do it!", "brilliant!",
		"keep it up!", "grind don't stop.", "you are a beast.", "that's how it's done >:D", "nice :D", "way to go!", "proud of you."};
	// Number of messages
	int msgnum = 13;

	srand(time(NULL));

	char* name = strtok(deltsk, "\n");
	printf("completed - %s | %s\n", name, msgs[rand() % msgnum]);

	free(deltsk);
}

int main(int argc, char* argv[]) {
	// Get command info given argc and argv
	cmdinfo* cmd = get_cmd_info(argc, argv);

	if (cmd != NULL) {
		if (!strcmp(cmd -> cmd, "ls")) {
			// todo ls (-i -c -due -cr -com)
			// List all tasks
			list_tasks(cmd -> opt_i, cmd -> opt_c, cmd -> opt_cr, cmd -> opt_due, cmd -> opt_com);
		} else if (!strcmp(cmd -> cmd, "del")) {
			// todo del [index or name]
			// Delete task
			free(task_delete(cmd -> arg));
		} else if (!strcmp(cmd -> cmd, "com")) {
			// todo com [index or name]
			// Complete task
			task_complete(cmd -> arg);
		} else if (!strcmp(cmd -> cmd, "test")) {
			norm_date(cmd -> arg);
		} else {
			// If first argument provided does not match with a valid command,
			// create task with arg1 (cmd -> cmd) as the name
			//
			// todo [task name] (-c -n -due)
			todo_add(cmd -> cmd, cmd -> opt_n, cmd -> opt_due, cmd -> opt_c);
		}
		// cmd was manually allocated, so we must free it
		free(cmd);
	} else {
		// No command provided
		printf("todo: no action supplied\n");
	}
}
