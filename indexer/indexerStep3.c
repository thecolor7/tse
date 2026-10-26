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
#include "hash.h"

static int globalSum = 0;

typedef struct {
	char *word;
	int count;
} word_t;

word_t* createWord(const char *word) {
	word_t *newWord = malloc(sizeof(word));
	if (newWord == NULL) return NULL;

	
	char *wordCopy = malloc(strlen(word) + 1); //+1 for the null terminator
	if (wordCopy != NULL) {
					strcpy(wordCopy, word);
	}
				
	newWord->word = wordCopy;
	newWord->count = 1;

	return newWord;
}

void freeWord(word_t *word) {
	if (word != NULL) {
		free(word->word);
		free(word);
	}
}

//Made a simple helper function to check if current element is in the hashtable
bool searchWord(void* elementp, const void* searchkeyp) {
	if (elementp == NULL || searchkeyp == NULL) {
		return false;
	}

	word_t *currentWord = (word_t*)elementp;
	
	return strcmp(currentWord->word, (const char*)searchkeyp) == 0;
}

void wordSum(void* elementp) {
	word_t *currentWord = (word_t*)elementp;
	
	globalSum += currentWord->count;
	return;
}


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
	hashtable_t *htp = hopen(100);
	
	page1 = pageload(1, "../crawler/pages");
	if (page1 == NULL) {
		printf("page1 failed to load\n");
		return(1);
	}
	//checks for proper initialization of hashtable, could potentially be combined with previous check
	if (htp == NULL) return(1);

	
	int i = 0;
	char *current;
	void *match;
	
	while ((i = webpage_getNextWord(page1, i, &current)) != -1) {
		if (NormalizeWord(current) == 0) {
			printf("%s\n", current);  //I used indexer > normalizedOUtput.txt to get a file and wc gave 141 words
			//Places the normalized word in the index hashtable
			if ((match = hsearch(htp, searchWord, current, strlen(current))) == NULL) {
				
				char *keyCopy = malloc(strlen(current) + 1); //+1 for the null terminator
				if (keyCopy != NULL) {
					strcpy(keyCopy, current);
				}
				
				word_t *currentWord = createWord(keyCopy);

				hput(htp,(void*)currentWord, keyCopy, strlen(current));
			} else {
				
				word_t *currentWord = (word_t*)match;
	
				currentWord->count += 1;
			}
		}
		free(current);
	}

	webpage_delete(page1);
	
	globalSum = 0;

	happly(htp, wordSum);
	printf("This is the result for the hashtable summation: %d\n", globalSum);


	return(0);
}
