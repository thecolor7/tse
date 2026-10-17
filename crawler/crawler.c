/* 
 * crawlerFinal.c --- 
 * 
 * Author: Matthew Morales
 * Created: 10-11-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include <inttypes.h>
//make sure to link .a file in make for .h files

// Making a simple helper function to check if if the current element is in the hashtable
bool searchURL(void* elementp, const void* searchkeyp) {
	return strcmp((char*)elementp, (const char*)searchkeyp) == 0;
}

//Step 5 - Save one page function (EVE)
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
	bool writable = false;
	struct stat buffer;
	FILE *fp = NULL;

	//Create filename from integer id
	char filename[10];
	
	//check that the directory is writable
	if (access(dirname, W_OK) == 0) {
		writable = true;
	}

	if (writable) {
		//open and check for the file - file needs path to directory as well
		sprintf(filename, "./%s/%d", dirname, id);

		//check in order to no =t write over existing file
		if (stat(filename, &buffer) == 0) {
			printf("file %d exists\n", id);
			return(1);
		}

		fp = fopen(filename, "w");
		if (fp == NULL) {
			printf("File %s could not be opened\n", filename);
			return(1);
		}
		//Print all info to the file
		fprintf(fp, "%s\n", webpage_getURL(pagep));
		fprintf(fp, "%d\n", webpage_getDepth(pagep));
		fprintf(fp, "%d\n", webpage_getHTMLlen(pagep));
		//Seems to not be printing any whitespace - should I be using something else?
		fprintf(fp, "%s\n", webpage_getHTML(pagep));
		fclose(fp);
		//do not try to complete if directory is not accessible
	} else {
		printf("Can't access %s\n", dirname);
		return(1);
	}
	return(0);
}

//Step 6 - something
void expand(int depth, webpage_t *page, hashtable_t *htp, queue_t *qp) {
	//loop through webpage for all urls
	int i = 0;
	char *url;
	
	//while there are more urls to get
	while ((i = webpage_getNextURL(page, i, &url)) > 0) {
		//check if internal or external
		if (IsInternalURL(url)) {
			// Checking if the internal URL is in the hashtable (Checking if we have seen the URL yet)
			if ( hsearch(htp, searchURL, url, strlen(url)) == NULL ) {
					// This block executes when we did not find the internal URL in the list of URL we have seen already (hashtable)
					// Put the URL into the hashtable of seen URLs
					hput(htp, url, url, strlen(url));
					
					// Now to create the webpage for the new unvisited internal URL
					webpage_t *wp = webpage_new(url, depth, NULL);
					if(webpage_fetch(wp)) {
						qput(qp, wp);
					} else {
						printf("Failed to fetch");
						webpage_delete(wp);
				  }
				} else {
					// In this case we have already seen this URL already so we disregard it and continue
					free(url);
				}
		} else {
			// The URL is external so we just free it and do nothing with it
			free(url); //make sure to free each url
		}
	}
}

int main (int argc, char *argv[]) {

	//Step 6: Handling command line calls - Genevieve
	char *seedURL, *pageDir;
	double d;
	uint32_t maxDepth;
	struct stat st;

	//Checks for correct number of arguments
	if (argc != 4) {
		printf("usage: crawler <seedurl> <pagedir> <maxdepth> 3\n");
		exit(EXIT_FAILURE);
	}

	seedURL = argv[1];
	pageDir = argv[2];
	d = strtod(argv[3], NULL);

	//Checks if given directory exists
	if (stat(pageDir, &st) != 0 || !S_ISDIR(st.st_mode)) {
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>2 \n");
		exit(EXIT_FAILURE);
	}

	//Checks if max depth is negative, fails if so
	if (d<0) {
		printf("usage: crawler <seedurl> <pagedir> <maxdepth> 1\n");
		exit(EXIT_FAILURE);
	}

	//Casts depth to an integer
	maxDepth = (uint32_t)d;

	// Opening a new queue and hastable
	queue_t *qp = qopen();
	hashtable_t *htp = hopen(100);
	webpage_t *queued_wp = NULL;

	// Checking for proper initialization of que and hashtable
	if (qp == NULL || htp == NULL) exit(EXIT_FAILURE);

	// Creating and putting a seed url into the hashtable so that it is considered visted and left alone
	char *seed_url_str = malloc(strlen(seedURL) + 1);

	// Checking for proper Malloc allocation
	if (seed_url_str == NULL) {
		qclose(qp);
    qp = NULL;
    hclose(htp);
    htp = NULL;
		exit(EXIT_FAILURE);
	}
  strcpy(seed_url_str, seedURL);

	// Creating and storing seed url into the first hash element
	hput(htp, seed_url_str, seed_url_str, strlen(seed_url_str));

	int depth = 0;
	int id = 1;

	//create seed webpage
	webpage_t *new = webpage_new(seedURL, depth, NULL);

	if(webpage_fetch(new)) {
		qput(qp, new);
	//exit on failrure to create
	} else {
		printf("failed to fetch");
    qclose(qp);
    qp = NULL;
    hclose(htp);
    htp = NULL;
    free(seed_url_str);
    seed_url_str = NULL;
    exit(EXIT_FAILURE);
	}

	// Now to iterate through the queue by getting entries and then freeing the memmory of the element
	while ( (queued_wp = (webpage_t*)qget(qp)) != NULL) {
		depth = webpage_getDepth(queued_wp);
		if (depth < maxDepth) {
			expand(depth + 1, queued_wp, htp, qp);
		}

		//Call for step 5
		pagesave(queued_wp, id, pageDir);

		// Printing the URLs that were put in the queue
		//printf("Internal URL: %s\n", webpage_getURL(queued_wp));
		
		// Now to free the weppage structure from the queue
		webpage_delete(queued_wp);
		id+=1;
	}
	// Freeing the que structure not that the internal elements have been cleared
	qclose(qp);
	// Lets now clear the elements of the hash by using happly() with free_element() AND then free the internal structure
	happly(htp, free);
	hclose(htp);
	exit(EXIT_SUCCESS);
	return(0);
}
