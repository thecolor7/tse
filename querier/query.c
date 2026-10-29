/* 
 * query.c --- 
 * 
 * Author: Eve H. Belding
 * Created: 10-29-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int searchFile(char *key) {
	FILE *fp;
	char word[100];
	int count, fileNum;

	//name and open
	fp = fopen("../indexer/rankTest.txt", "r");

	//scan file for info - ONLY WORKS FOR ONE PAGE
	while (fscanf(fp, "%s %d %d", word, &fileNum, &count) != EOF) {
		if (strcmp(word, key) == 0) {
			fclose(fp);
			return(count);
		}
	}
	
	fclose(fp);
	return(0);
}

int main(void) {

	char *user = NULL;
	char **search = NULL;
	char userChar;
	int i = 0;
	int j = 0;
	int min, count;
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
			free(user);
			user = NULL;
			continue;
		}

		//Build the string for the query input
		while (userChar != '\n') {
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
			
			//how to remove excess spaces from the user input and seperate words into list
			char *word = strtok(user, " ");
			j = 0;
			while (word != NULL) {
				if (strlen(word) >= 3) {
					search = realloc(search, (++j)*sizeof(char *));
					if (search == NULL) {
						printf("Realloc failed\n");
						free(user);
						user = NULL;
						return(1);
					}
					search[j-1] = word;
				}
				word = strtok(NULL, " ");
			}

			//search each word
			min = -1;
			for (int k = 0; k < j; k++) {
				count = searchFile(search[k]);
				printf("%s:%d ", search[k], count);
				if (min == -1 || count < min) {
					 min = count;
				}
			}
			printf("-- %d\n", min);
		}

		//free all
		free(user);
		user = NULL;
		free(search);
		search = NULL;
		
	} while(!done); //exit when EOF
		
	return(0);
}
