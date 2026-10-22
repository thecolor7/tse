/* 
 * indexer.c --- 
 * 
 * Author: Eve H. Belding
 * Created: 10-21-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "webpage.h"
#include "pageio.h"

//normalizing defined as removing words with less than 3 characters or non alphabetical characters, and making all lowercase
//only returns 0 for a word made lowercase, used to then only print these words. Deleted words or just null terminators
int NormalizeWord(char *word) {
	//check that word exists
	if (word == NULL) {
		printf("word uninitialized");
		return(1);
	}

	//check length
	if (strlen(word) < 3) {
		word[0] = '\0'; //effectively deletes the word
		return(1);
	}
	//check for special characters
 	for (int i = 0; i < strlen(word); i++) { 
 		if (!isalpha(word[i])) {
			word[0] = '\0';
			return(1);
 		}
	}
	//only if passed, convert to lowercase
	for (int j = 0; j < strlen(word); j++) {
		if (word[j] < 97) { //this is ok because non letters have already been excluded
			word[j] = (word[j]) + ('a' - 'A');
		}
	}
	return(0);
}

int main(void) {
	webpage_t *page1 = NULL;

	page1 = pageload(1, "../crawler/pages");
	if (page1 == NULL) {
		printf("page1 failed to load\n");
		return(1);
	}

	int i = 0;
	char *current;
	while ((i = webpage_getNextWord(page1, i, &current)) != -1) {
		if (NormalizeWord(current) == 0) {
			printf("%s\n", current);  //I used indexer > normalizedOUtput.txt to get a file and wc gave 141 words
		}
		free(current);
	}

	free(current);
	webpage_delete(page1);
	return(0);
}
