#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "vector.h"

typedef struct cmdinfo {
	char* cmd;
	char* arg;
	char* opt_n;
	char* opt_d;
} cmdinfo;

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
				// -n [note_string]
				if(!strcmp(argv[i], "-n")) {
					cmd -> opt_n = argv[++ i];
				} else if (!strcmp(argv[i], "-d")) {
					cmd -> opt_d = argv[++ i];
				}
			}
		}

		return cmd;
	} else {
		return NULL;
	}
}

void todo_add(char* msg, char* note, char* due) {
	FILE* file = fopen("todo.txt", "a+");

	if (file == NULL) {
		printf("todo: failure to create or open todo file\n");
		exit(1);
	}

	time_t curr_time = time(NULL);
	char* timestr = ctime(&curr_time);

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

int main(int argc, char* argv[]) {
	cmdinfo* cmd = get_cmd_info(argc, argv);

	if (cmd != NULL) {
		if (!strcmp(cmd -> cmd, "all")) {

		} else {
			todo_add(cmd -> cmd, cmd -> opt_n, cmd -> opt_d);
		}
	} else {
		printf("todo: no action supplied\n");
	}
}
