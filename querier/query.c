/* 
 * query.c --- 
 * 
 * Author: Eve H. Belding
 * Created: 10-29-2026
 * Version: 1.0
 * 
 * Description: Current issues: the function is scanning the buffer and using information it shouldn't (invalid query and characters following special characters; will probably switch to fgets
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int main(void) {

	char *user = NULL;
	char userChar;
	int i = 0;
	bool done = false;
	bool spec = false;

	do {
		spec = false;
		
		//initial malloc to restart word
		user = malloc(sizeof(char));
		if (user == NULL) {
			printf("Malloc Failed\n");
			return(1);
		}

		//gather user and resize string
		printf(">");
		i = 0;
		//check for end
		if (scanf("%c", &userChar) == EOF) {
			done = true;
			continue;
		}
		
		while (userChar != '\n' && !spec) {
			//check for special character
			if (userChar != ' ' && isalpha(userChar) == 0) {
				spec = true;
			}
			if (userChar != ' ') {
				userChar = tolower(userChar);
			}
			user[i] = userChar;
			i++;
			user = realloc(user, (i+1)*sizeof(char));
			if (user == NULL) {
				printf("Realloc Failed\n");
				return(1);
			}
			scanf("%c", &userChar);
		}

		//once word is finished, add null terminator and print to check
		user[i] = '\0';
		
		if (spec) {
			printf("[invalid query]\n");
		} else {
			//how to remove excess spaces from the user input
			char *word = strtok(user, " ");
			while (word != NULL) {
				printf("%s ", word);
				word = strtok(NULL, " ");
			}
			printf("\n");
		}
		
		free(user);
		user = NULL;
	} while(!done); //exit when EOF
		
	return(0);
}
