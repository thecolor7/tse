/* 
 * mt_indexer.c --- 
 * 
 * Author: Matthew Morales
 * Created: 11-11-2026
 * Version: 2.0 Mutithreaded
 * 
 */

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pthread.h>
#include "queue.h"
#include "webpage.h"
#include "pageio.h"
#include "lhash.h"
#include "lqueue.h"
#include "indexio.h"

// Key Structs
// Holds a document id and a count for how many times a word appeared
typedef struct document {
	int id;
	int count;
} document_t;

// Holds a word and a queue of document_t structs
typedef struct word {
	char *word;
	queue_t *docs;
} word_t;

// Shared data struct for all worker threads
typedef struct {
	lqueue_t *id_queue; // Queue of document IDs
	lhash_t *index; // Locked hash table for the index
	char *pagedir; // Directory containing crawled pages
	pthread_mutex_t update_lock; // Carrying Mutex for each thread
} shared_data_t;

// Helper Functions

// Creating a new doc
document_t* createDoc(int docID) {
	document_t *newDoc = malloc(sizeof(document_t));
	if (newDoc == NULL) return NULL;
  
	newDoc->id = docID;
	newDoc->count = 1;
	return newDoc;
}

// frees document_t
void freeDoc(void *document) {
	if (document != NULL) {
		document_t *doc = (document_t *)document;
		free(doc);
	}
}

// Creates a new word
word_t* createWord(char *word) {
	word_t *newWord = malloc(sizeof(word_t));
	if (newWord == NULL) return NULL;
  
	char *wordCopy = malloc(strlen(word) + 1);
	if (wordCopy == NULL) {
		free(newWord);
		return NULL;
	}
  
	strcpy(wordCopy, word);
	newWord->word = wordCopy;
	newWord->docs = qopen();
	return newWord;
}

// frees word_t
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

// Made a simple helper function to check if current element is in the hashtable
bool searchWord(void* elementp, const void* searchkeyp) {
	if (elementp == NULL || searchkeyp == NULL) return false;
	word_t *currentWord = (word_t*)elementp;
	return strcmp(currentWord->word, (const char*)searchkeyp) == 0;
}

// Made a simple helper function to determine if a document is in the docs queue
bool searchDoc(void* elementp, const void* docAddress) {
	if (elementp == NULL || docAddress == NULL) return false;
	int docID = *(const int*)docAddress;
	document_t *currentDoc = (document_t*)elementp;
	return currentDoc->id == docID;
}

// normalizing defined as a removing words with less than 3 characters or non alphabetical characters,and making all lowercase only returns 0 for a word made lowercase, used to then only print these words. Deleted words or just null terminators
int NormalizeWord(char *word) {
	// check that word exits
	if (word == NULL) {
		return(1);
	}

	// check length
	if (strlen(word) < 3) {
		word[0] = '\0'; 
		return(1);
	}
	// check for special characters
	for (int i = 0; i < strlen(word); i++) {
		if (!isalpha(word[i])) {
			word[0] = '\0';
			return(1);
		}
	}
	// only if passed convert to lowercase
	for (int j = 0; j < strlen(word); j++) {
		if (word[j] < 97) { // this is ok because non letter hae already been excluded
			word[j] = (word[j]) + ('a' - 'A');
		}
	}
	return(0);
}

// Worker Thread Routine
void *indexer_worker(void *arg) {
	shared_data_t *shared = (shared_data_t *)arg;
	
	while (1) {
		// Pull the next document ID from the shared queue
		int *id_ptr = (int *)lqget(shared->id_queue);
    
		// If queue returns NULL, we are done
		if (id_ptr == NULL) {
			break;
		}
		
		int docID = *id_ptr;
		// Free the dynamically allocated integer
		free(id_ptr); 
		
		// Load the webpage
		webpage_t *page = pageload(docID, shared->pagedir);
		if (page == NULL) {
			continue;
		}
		
		// Parse through the words
		int pos = 0;
		char *current;
    
		while ((pos = webpage_getNextWord(page, pos, &current)) != -1) {
			if (NormalizeWord(current) == 0) {
				
				// Activating the mutex lock
				pthread_mutex_lock(&shared->update_lock);

				// Searching and checking for a word match
				void *match = lhsearch(shared->index, searchWord, current, strlen(current));

				// The case that we do not find a word
				if (match == NULL) {
					char *keyCopy = malloc(strlen(current) + 1);
					if (keyCopy != NULL) {
						strcpy(keyCopy, current);
					}

					// Otherwise we found a match
					word_t *currentWord = createWord(keyCopy);
					document_t *currentDoc = createDoc(docID);
					qput(currentWord->docs, currentDoc);
					lhput(shared->index, currentWord, keyCopy, strlen(current));

					// Free dynamically allocated memmory
					free(keyCopy);
					
				} else {
					word_t *currentWord = (word_t*)match;
					void *docMatch = qsearch(currentWord->docs, searchDoc, &docID);
          
					if (docMatch == NULL) {
						document_t *currentDoc = createDoc(docID);
						qput(currentWord->docs, currentDoc);
					} else {
						document_t *currentDoc = (document_t*)docMatch;
						currentDoc->count += 1;
					}
				}
        
				// unlocking the mutex lock
				pthread_mutex_unlock(&shared->update_lock);
			}
			// freeing dynamically allocated memory
			free(current);
		}
		// deleting webpage
		webpage_delete(page);
	}
	return NULL;
}

// Main Loop

int main(int argc, char *argv[]) {

	// Enforcing Proper Usage: ./indexer <pagedir> <indexnm> <num_threads>
	if (argc != 4) {
		printf("USAGE: ./indexer <pagedir> <indexnm> <num_threads>\n");
		return 1;
	}
	
	char *pagedir = argv[1];
	char *indexnm = argv[2];
	int num_threads = atoi(argv[3]);

	// Doing checks on arugments for thread count
	if (num_threads <= 0) {
		printf("ERROR: Number of threads must be >= 1\n");
		return 1;
	}
	
	struct stat dir_stat;
	if (stat(pagedir, &dir_stat) != 0) {
		printf("ERROR: Directory '%s' does not exist and/or is invalid directory\n", pagedir);
		return 1;
	}
	
	// Initialize Shared Data between Threads
	shared_data_t *shared = malloc(sizeof(shared_data_t));
	shared->id_queue = lqopen();
	shared->index = lhopen(100);
	shared->pagedir = pagedir;
	pthread_mutex_init(&shared->update_lock, NULL);

	// Checking for proper initialization
	if (shared->index == NULL || shared->id_queue == NULL) {
		printf("ERROR: Failed to initialize shared data  structures\n");
		return 1;
	}
	
	// Loading the job queue with document IDs
	int docID = 1;
	char filepath[256];
	while (1) {
		snprintf(filepath, sizeof(filepath), "%s/%d", pagedir, docID);
		if (access(filepath, F_OK) == 0) {
			int *id_ptr = malloc(sizeof(int));
			*id_ptr = docID;
			lqput(shared->id_queue, id_ptr);
			docID++;
		} else {
			// If the file ID is missing the crawler directory is empty
			break; 
		}
	}
	
	// Creating threads
	pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
	for (int i = 0; i < num_threads; i++) {
		if (pthread_create(&threads[i], NULL, indexer_worker, (void *)shared) != 0) {
			printf("ERROR: Failed to create thread %d\n", i);
			return 1;
		}
	}
	
	// Waiting for threads to finish their work
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// Saving the index
	// Using pointer cast to extract internal hashtable
	hashtable_t *base_hash_table = *(hashtable_t **)shared->index;
  
	if (indexsave(base_hash_table, indexnm) != 0) {
		printf("ERROR: Failed saving the index to '%s'.\n", indexnm);
	}
	
	// Free all allocated data before terminating
	lhapply(shared->index, freeWord);
	lhclose(shared->index);
	lqclose(shared->id_queue);
	pthread_mutex_destroy(&shared->update_lock);
	free(threads);
	free(shared);
	
	return 0;
}
