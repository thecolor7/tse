/* 
 * indexio.c --- 
 * 
 * Author: Matthew Morales
 * Created: 10-26-2026
 * Version: 1.0
 * 
 */

// Including neccesary header files and libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "indexio.h"
#include "hash.h"
#include "queue.h"

// Including neccesary structs
typedef struct document {
    int id;
    int count;
} document_t;

typedef struct word {
    char *word;
    queue_t *docs;
} word_t;

// Global file pointer for use in  happly/qapply helper functions
static FILE *save_file;

// Defining useful Hekper functions for qapply to use to write document ID's and counts 
static void write_doc(void *ep) {
    document_t *doc = (document_t*)ep;
    if (doc != NULL && save_file != NULL) {
        // Space separates each entry per the requirements
        fprintf(save_file, " %d %d", doc->id, doc->count);
    }
}

// Second helper function used by happly to write words and writing to the document queue 
static void write_word(void *ep) {
    word_t *temp_wp = (word_t*)ep;

		// Checking for valid initialized pointers
    if (temp_wp != NULL && save_file != NULL) {

			// Write the word, write document contents, and then start a new line for a new word
			fprintf(save_file, "%s", temp_wp->word);
			qapply(temp_wp->docs, write_doc);
			fprintf(save_file, "\n");
    }
}

// Main Function Implentations

int indexsave(hashtable_t *index, char *indexnm) {

	// Checking for valid input pointers
	if (index == NULL || indexnm == NULL) {
		return 1;
	}

	// Opening the target file we will write contents too
	save_file = fopen(indexnm, "w");
	// Checking if file opened properly
	if (save_file == NULL) {
		return 1;
	}
	
	// Apply the write_word function to every element in the hashtable
	happly(index, write_word);
	
	// Close the file we are writing to
	fclose(save_file);

	// Reseting to reuse later
	save_file = NULL; 
	return 0;
}


hashtable_t* indexload(char *indexnm) {

	// Checking for a valid target file before opening
	if (indexnm == NULL) {
		return NULL;
	}

	// Defining and attempting to open target file for reading
	FILE *fp = fopen(indexnm, "r");
	// Checking if we successfully opened the file
	if (fp == NULL) {
		return NULL;
	}

	// Defining a new hashtable to write data to
	hashtable_t *index = hopen(100);
	// Checking if the hashtable was opened properly
	if (index == NULL) {
		fclose(fp);
		return NULL;
	}

	// Creating a character array to act as a buffer for words in the indexnm file(glorified copy and paste)
	char wordBuf[1000];
  
	//Reading the first string on each line of the indexnm and then copying the contents into our wordbuffer 
	while (fscanf(fp, "%999s", wordBuf) == 1) {
		
		// Making a newWord struct to hold contents of buffer and checking if it memory was allocated properly
		word_t *newWord = malloc(sizeof(word_t));
		if (newWord == NULL) continue;

		// Preallocating memmory for buffer to fit into newWord->word
		newWord->word = malloc(strlen(wordBuf) + 1);
		// Checking for if the memmory was allocated properly
		if (newWord->word == NULL) {
			free(newWord);
			continue;
		}

		// Copy contents of our buffer into newWord->word and opening queue at newWord->docs 
		strcpy(newWord->word, wordBuf);
		newWord->docs = qopen();
		
		int docID, count;
    
		// Now to extract integer pairs (docID and count). 
		// I will now use fscanf("%d %d") which will fail to read an integer, returning 0, since we are using %d here
		// therefore, we will loop while the buffer contains only DocID and count pairs and break into the outter loop
		// when a new string/word appears ready to repeat the process for the new word
		while (fscanf(fp, "%d %d", &docID, &count) == 2) {
			document_t *newDoc = malloc(sizeof(document_t));
			if (newDoc != NULL) {
				newDoc->id = docID;
				newDoc->count = count;
				qput(newWord->docs, newDoc);
			}
		}
		
		// Now to put the fully reconstructed word struct into the hashtable
		hput(index, (void*)newWord, newWord->word, strlen(newWord->word));
	}
	
	fclose(fp);
	return index;
}

