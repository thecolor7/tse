/* 
 * test_indexio.c --- 
 * 
 * Author: Matthew Morales
 * Created: 10-26-2026
 * Version: 1.0
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "queue.h"
#include "indexio.h"

// Redefining neccesary structures
typedef struct document {
	int id;
	int count;
} document_t;

typedef struct word {
	char *word;
	queue_t *docs;
} word_t;

// Defining helper to make freeing document structs inside the queues easier
void freeDoc(void *document) {
	// Checking valid initialized input pointer
	if (document != NULL) {
		document_t *doc = (document_t *)document;
		free(doc);
	}
}

// Helper function to make freeing the word struct, the string, and document queue
void freeWord(void *word) {
	// Checking valid initialized input pointer
	if (word != NULL) {
		word_t *currentWord = (word_t *)word;
		// checking valid pointer before freeing all allocated memmory
		if (currentWord->docs != NULL) {
			qapply(currentWord->docs, freeDoc);
			qclose(currentWord->docs);
		}
		free(currentWord->word);
		free(currentWord);
	}
}

int main(int argc, char *argv[]) {
	// Checking proper arguments for the test file
	if (argc != 3) {
		printf("USAGE: ./test_indexio <source_file> <dest_file>\n\n");
		return 1;
	}
	
	char *source_file = argv[1];
	char *dest_file = argv[2];
	
	// Attempting to load source_file/indexnm file
	printf("Loading index from %s...\n\n", source_file);

	// Attempting to use indexload function
	hashtable_t *index = indexload(source_file);

	// checking if the indexload() function call failed or not
	if (index == NULL) {
		printf("ERROR...Test failed: Unable to load index from %s\n\n", source_file);
		return 1;
	}
	
	// Trying to save the newly loaded index using indexsave()
	printf("Saving reconstructed index to %s...\n", dest_file);

	// Applying indexsave()
	int save_status = indexsave(index, dest_file);
  
	if (save_status != 0) {
		printf("ERROR...Test Failed: Failed to save index to %s.\n\n", dest_file);
		// freeing memmory before terminating test file
		happly(index, freeWord);
		hclose(index);
		return 1;
	}
	
	// Final Memmory Deallocating before ending test
	printf("Deallocating memory...\n\n");
	happly(index, freeWord);
	hclose(index);
	printf("Test complete...now run 'diff %s %s' in the terminal to verify data integrity.\n\n", source_file, dest_file);
	return 0;
}
