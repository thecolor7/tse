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
	fscanf(fp, "%s", newDoc->docUrl);
	fclose(fp);
	
  return newDoc;
}

//frees document_t
void freeDoc(void *document) {
  if (document != NULL) {
    doc_t *doc = (doc_t *)document;
    free(doc);
  }
}

void printDoc(doc_t *document) {
	printf("rank:%d:doc:%d : %s\n", document->docRank, document->docId, document->docUrl);
}


//given a keyword and page, find the count for that page
int searchFile(char *key, int id) {
	FILE *fp;
	char word[100];
	int count = 0;
	int file = 0;

	//name and open
	fp = fopen("../indexer/rankTest.txt", "r");

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

	char *user = NULL;
	char **search = NULL;
	char userChar;
	int i = 0;
	int j = 0;
	int min, count;
	bool done = false;
	bool spec = false;
	doc_t *newDoc;
	
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
			queue_t *qp = qopen();
			for (int page = 1; page <= 7; page++) { //need to fix to be more universal (how do I find number of pages)
				min = -1;
				for (int k = 0; k < j; k++) {
					count = searchFile(search[k], page);
					if (min == -1 || count < min) {
						min = count;
					}
				}
				newDoc = createDoc(page, min);
				qput(qp, newDoc);
				printDoc(newDoc);
				freeDoc(newDoc);
			}
			qclose(qp);
		}

		//free all
		free(user);
		user = NULL;
		free(search);
		search = NULL;
		
	} while(!done); //exit when EOF
		
	return(0);
}
