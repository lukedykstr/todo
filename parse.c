#include <string.h>

// Parse array data type (just an array of strings)
typedef char parray[250][250];

/*
 * Parses the given string into the given parray with delimiters
 * space, /, :, and -
 *
 * Returns nothing.
 */
void parse(parray* tokens, char* strin) {
	// Current word being parsed
	char word[strlen(strin) + 1];
	// Length of the current word being parsed
	int wlength = 0;
	// Current index in tokens array
	int curr_index = 0;
	
	for (int i = 0; i < strlen(strin); i ++) {
		if (strin[i] == ' ' || strin[i] == '/' || strin[i] == ':' || strin[i] == '-') {
			if (strlen(word) > 0) {
				// Add current word to array
				strcpy((*tokens)[curr_index ++], word);
			}

			if (strin[i] != ' ') {
				// Add delimiter to array
				(*tokens)[curr_index][0] = strin[i];
				(*tokens)[curr_index ++][1] = '\0';
			}

			// Reset word
			wlength = 0;
		} else {
			// Append current char to word
			word[wlength] = strin[i];
			word[wlength + 1] = '\0';
			wlength ++;
		}
	}
	
	// If current word isn't empty, add it to tokens
	if (wlength > 0) {
		strcpy((*tokens)[curr_index ++], word);
	}
	
	// Add "END" to end of tokens array
	strcpy((*tokens)[curr_index], "END");
}

/*
 * Given an array of strings, its length, and a target string,
 * return the index of the target string in the array.
 *
 * If target string is not found, returns -1.
 */
int array_find(char* words[], int length, char* word) {
	for(int i = 0; i < length; i ++) {
		if (!strcmp(words[i], word)) { return i; }
	}

	return -1;
}
