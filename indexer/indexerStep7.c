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
#include "queue.h"
#include "indexio.h"

// STEP 7 Edit, global sum is no longer needed
//constant value for summing the word counts
//static int globalSum = 0;

//"document_t" sturcture, holds a document id and a count for how many times a word appeared; each word in the index has a
//queue that will hold these 
typedef struct document {
	int id;
	int count;
} document_t;

//creates a new document_t
document_t* createDoc(int docID) {
	document_t *newDoc = malloc(sizeof(document_t));
	if (newDoc == NULL) return NULL;

	newDoc->id = docID;
	newDoc->count = 1;
	return newDoc;
}

//frees document_t
void freeDoc(void *document) {
	if (document != NULL) {
		document_t *doc = (document_t *)document;
		free(doc);
	}
}

//"word_t" structure, holds a word and a queue of document_t structures; the index hashtable will hold this
typedef struct word {
	char *word;
	queue_t *docs;
} word_t;

//creates a new word_t
word_t* createWord(char *word) {
	word_t *newWord = malloc(sizeof(word_t));
	if (newWord == NULL) return NULL;

	char *wordCopy = malloc(strlen(word) + 1); //+1 for the null terminator
	if (wordCopy == NULL) {
		free(newWord);
		return NULL;
	}			

	strcpy(wordCopy, word);
	
	newWord->word = wordCopy;
	newWord->docs = qopen();
	return newWord;
}

//frees word_t
void freeWord(void *word) {
	if (word != NULL) {
		word_t *currentWord = (word_t *)word;

		if (currentWord->docs != NULL) {
			qapply(currentWord->docs, freeDoc);
			qclose(currentWord->docs);
		}
		free(currentWord->word);
		free(currentWord);
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

//Made a simple helper function to determine if a document is in the docs queue
bool searchDoc(void* elementp, const void* docAddress) {
	if (elementp == NULL || docAddress == NULL) {
		return false;
	}
	int docID = *(const int*)docAddress;
	document_t *currentDoc = (document_t*)elementp;

	return currentDoc->id == docID;
}

// STEP 7: Summing helper functions are also not needed for this step
//adds the count of a word to the total word count; used in the sumwords function
//void wordSum(void* elementp) {
//document_t *currentDoc = (document_t*)elementp;
//
//globalSum += currentDoc->count;
//return;
//}

//applies the wordSum function to each document_t in the docs queue from the word_t it is given
//void sumwords(void* elementp) {
//word_t *currentWord = (word_t*)elementp;
 
//qapply(currentWord->docs, wordSum);
//return;
//}

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

int main(int argc, char *argv[]) {
	// Commenting out to include this functionality in the new main for loop
	//webpage_t *page1 = NULL;
	//int docID = 1;
	//page1 = pageload(1, "../crawler/pages");
	//if (page1 == NULL) {
	//printf("page1 failed to load\n");
	//return(1);
	//}

	// Adding some argument management just to ensure only three arguments for page directory and indexnm destination file
	if (argc != 3) {
		printf("USAGE: ./indexer <pagedir> <indexnm>\n\n");
		return 1;
	}

	char *pagedir = argv[1];
	char *indexnm = argv[2];

	// Using stat to chck if pagedir exists
	struct stat dir_stat;
	if (stat(pagedir, &dir_stat) != 0){
		printf("ERROR: Directory '%s' does not exist or is not a valid directory.\n\n", pagedir);
		return 1;
	}	
	
	hashtable_t *index = hopen(100);
	//checks for proper initialization of hashtable, could potentially be combined with previous check
	if (index == NULL) return 1;

	// New Main For Loop
	int docID = 1;
	webpage_t *page;
	
	while ((page = pageload(docID,pagedir)) != NULL) {

		// Defining interation variables
		int i = 0;
		char *current;
		void *match;
		void *docMatch;
	
		while ((i = webpage_getNextWord(page, i, &current)) != -1) {
			if (NormalizeWord(current) == 0) {

				//searches if the current word is already in the index (hashtable)
				if ((match = hsearch(index, searchWord, current, strlen(current))) == NULL) {
					
					//creates a copy of the word
					char *keyCopy = malloc(strlen(current) + 1); //+1 for the null terminator
					if (keyCopy != NULL) {
						strcpy(keyCopy, current);
					}

					//this case runs if the word has not been found, makes new word and document 
					word_t *currentWord = createWord(keyCopy);
					document_t *currentDoc = createDoc(docID);
					
					qput(currentWord->docs, currentDoc);
					hput(index, (void*)currentWord, keyCopy, strlen(current));
					free(keyCopy);
				} else {
				 
					word_t *currentWord = (word_t*)match;
					if ((docMatch = qsearch(currentWord->docs, searchDoc, &docID)) == NULL) {

						// This will run when there is not a match for the current document in the queue so we will create a new one
						document_t *currentDoc = createDoc(docID);
						qput(currentWord->docs, currentDoc);
					} else {
						
						document_t *currentDoc = (document_t*)docMatch;
						currentDoc->count +=1;
					}
				}
			}
			free(current);
		}
		
		webpage_delete(page);
		// Now that we do not have a endID we will increment the docID to properly number page files as we iterate until failure
		docID++;
	}

	// Saving the index to the file using the input arguments
	if (indexsave(index,indexnm) != 0) {
		printf("ERROR: Failed to save index to '%s'.\n\n",indexnm);
	}
	
	//Note needed in step 7
	//globalSum = 0;
	//happly(index, sumwords);
	//printf("This is the result for the hashtable summation: %d\n", globalSum);

	happly(index, freeWord);
	hclose(index);
	

	return(0);
}
