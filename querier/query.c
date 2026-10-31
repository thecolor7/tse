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
#include "queue.h"

//structure for how this stores documents and functions to manage
typedef struct doc {
	int docId;
	int docRank;
	char docUrl[200];
} doc_t;

doc_t* createDoc(int docID, int rank) {
	char file[200];
	FILE *fp;
  doc_t *newDoc = malloc(sizeof(doc_t));
  if (newDoc == NULL) return NULL;

  newDoc->docId = docID;
  newDoc->docRank = rank;
	//get the url
	sprintf(file, "../crawler/pages/%d", docID);
	fp = fopen(file, "r");
	// MATTHEW EDIT: Added for edge case
	if (fp != NULL) {
		fscanf(fp, "%s", newDoc->docUrl);
		fclose(fp);
	} else {
		strcpy(newDoc->docUrl, "URL_NOT_FOUND");
	}
	return newDoc;
}

//frees document_t
void freeDoc(void *document) {
  if (document != NULL) {
		// MATTHEW EDIT: Removed since I don't think we need to cast document at this point? Leaving commented just in case and chaning free(doc); -> free(document);
		//doc_t *doc = (doc_t *)document;
    free(document);
  }
}

void printDoc(doc_t *document) {
	printf("rank:%d: doc:%d: %s\n", document->docRank, document->docId, document->docUrl);
}

//given a keyword and page, find the count for that page
int searchFile(char *key, int id) {
	FILE *fp;
	char word[100];
	int count = 0;
	int file = 0;
	
	//name and open
	fp = fopen("../indexer/rankTest.txt", "r");
	// MATTHEW EDIT: Added just as a safeguard
	if (fp == NULL) return 0;
	
	//scan file for info - ONLY WORKS FOR ONE PAGE
	while (fscanf(fp, "%s", word) != EOF) {
		if (strcmp(word, key) == 0) {
			while (fscanf(fp, "%d %d", &file, &count) == 2) {
				if (file == id) {
					fclose(fp);
					return(count);
				}
				else if (file > id) {
					break;
				}
			}
		}
	}
	
	fclose(fp);
	return(0);
}

int main(void) {

	// MATTHEW EDIT: Commenting out now unused variables
	//char *user = NULL;
	//char **search = NULL;
	//char userChar;
	int i = 0;
	int j = 0;
	//int min, count;
	//bool done = false;
	//bool spec = false;
	//doc_t *newDoc;
	char buffer[1000];

	// MATTHEW EDIT: To avoid hardcoded page limit we'll  scan the total number of documents crawled in index
	int maxDocs = 0;
	while(1) {
		char path[100];
		sprintf(path, "../crawler/pages/%d", maxDocs + 1);
		FILE *fp = fopen(path, "r");
		if (fp){
			fclose(fp);
			maxDocs++;
		} else {
			break;
		}
	}

	// Creating main loop to stay in when the waiting for user input

	while (1)  {
		printf("> ");

		// Adding a check for a termination on EOF (Ctrl-D) input from user
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
			break;
		}

		// At this point we know the buffer is not a valid termination so we will check for valid characters and convert to lowercase
		bool inv_char = false;
		for (i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++) {
			// Here we check is the character we are looking at from the buffer is either not an letter and not a space meaning it is not a valid character we are to consider and does not apply tolower()
			if (!isalpha(buffer[i]) && !isspace(buffer[i])) {
				inv_char = true;
				break;
			}
			buffer[i] = tolower(buffer[i]);
		}

		// Not to catch that we were passed an invalid query
		if (inv_char) {
			printf("\n[invalid query]\n\n");
			continue;
		}

		// Now to use strtok() to break up the main buffer into a string of all the words passed in the query to run our checks and later search for words
		char *words[100];
		int wordCount = 0;
		char *token = strtok(buffer, " \t\n");

		// We will not iterate through the buffer incrementing word count and refreching tokens until we reach the end saving only the words we want
		while (token != NULL) {
			// Now to do our initial character length check to keep workds greater then 3 character in addition to the or operators for precedance later since or is less then 2 characters
			if (strlen(token) >= 3 || strcmp(token, "or") == 0 ) {
				words[wordCount++] = token;
			}
			token = strtok(NULL, " \t\n");
		}

		// check to ignore empty inputs
		if (wordCount == 0) {
			continue;
		}

		// Not to check proper syntax starting with a starting or ending use of AND and OR operators
		if (strcmp(words[0], "and") == 0 || strcmp(words[0], "or") == 0 || strcmp(words[wordCount-1], "and") == 0 || strcmp(words[wordCount-1], "or") == 0) {
			printf("\n[invalid query]\n\n");
			continue;
		}

		bool syntax_error = false;
		// Now we will iterate through to check if we have an adjacent operator syntax error
		for(i=0; i < wordCount-1; i++) {
			if ( (strcmp(words[i], "and") == 0 || strcmp(words[i], "or") == 0) && (strcmp(words[i+1], "and") == 0 || strcmp(words[i+1], "or") == 0) ) {
				syntax_error = true;
				break;
			}
		}

		if (syntax_error) {
			printf("\n[invalid query]\n\n");
			continue;
		}

		// Now to print clean and validated query as requested
		for (i=0; i < wordCount; i++) {
			printf("%s ", words[i]);
		}
		printf("\n");

		// Now that we know the number of documents we can create an array of documents to hold documents for ranking output
		queue_t *qp = qopen();
		int resultCount = 0;

		// main loop that will run all of the query logic
		for(int page = 1; page <= maxDocs; page++) {
			int total_score = 0;
			int and_score = -1;

			// Notes on this loop, as this is quite confusing: The loop will loop through all the crawled documents where it will calculate the final rank of that document before continuing to the next one; Within the loop total_score and the and_score are used to track the word occurances in this order, it will count word accurances and accumulate them into the and score until an OR is encountered, which means that the sequences of ands has been completed and the score will be added to the total score and start on the next and sequence according to the following intent --> (word1 AND word2 AND word3) OR (word4 AND word5) where the program will have two contributin totals (total1) OR (total2) which are distinguished by the seperating OR operator
			for (int w = 0; w < wordCount; w++) {
				if (strcmp(words[w], "and") == 0) {
					continue; // Simply skip over AND's
				} else if (strcmp(words[w], "or") == 0) {
					// Once we reach the seperating OR we will save the current AND sequence's score to the total; NOTE: and_score has a default value of -1 which means it is a new sequences and is reset every time we have a new valid and sequence score
					if (and_score != -1) {
						total_score += and_score;
					}
					and_score = -1;
				} else {
					// This runs when we are in a current AND sequence, aka, we are at a new word AND we are not at a new OR operator, so we will search and count the occurances and saves the new count ONLY if it is either the start of a new AND sequence OR it is the new lower number of occurances
					int occurances = searchFile(words[w], page);
					if (and_score == -1 || occurances < and_score){
						and_score = occurances;
					}
				}
			}

			// Now to add the and_score at the end of a valid sequence/the appearance of an OR operator to the overall total_score
			if (and_score != -1) {
				total_score += and_score;
			}

			// If we got a total_score greater then 0 then we know the document was a match and we will save it's data into results
			if (total_score > 0) {
				doc_t *match = createDoc(page, total_score);
				qput(qp, match);
				resultCount++;
			}
		}

		// Extracting from our queue to sort
		doc_t *sortedDocs[resultCount];
		for (i=0; i < resultCount; i++) {
			sortedDocs[i] = qget(qp);
		}
		
		// Now to sort our results by rank in descending order
		for (int i=0; i < resultCount-1; i++) {
			for (j = 0; j < resultCount-i-1; j++) {
				if (sortedDocs[j]->docRank < sortedDocs[j+1]->docRank) {
					doc_t *temp = sortedDocs[j];
					sortedDocs[j] = sortedDocs[j+1];
					sortedDocs[j+1] = temp;
				}
			}
		}

		// Now to print final results
		for (i=0; i<resultCount; i++){
			printDoc(sortedDocs[i]);
			freeDoc(sortedDocs[i]);
		}

		// Closing Queue
		qclose(qp);
	}

	printf("\n\nEnding Quiery...\n\n");
	return(0);
}
