/* 
 * crawler.c --- 
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
#include "webpage.h"
#include "queue.h"
#include "hash.h"

//make sure to link .a file in make for .h files

// Making a simple helper function to check if if the current element is in the hashtable
bool searchURL(void* elementp, const void* searchkeyp) {
	return strcmp((char*)elementp, (const char*)searchkeyp) == 0;
}

int main (void) {

	// Opening a new queue and hastable
	queue_t *qp = qopen();
	hashtable_t *htp = hopen(100);
	webpage_t *queued_wp = NULL;
	
	// Checking for proper initialization of que and hashtable
	if (qp == NULL || htp == NULL) return EXIT_FAILURE;

	// Creating and putting a seed url into the hashtable so that it is considered visted and left alone
	char *seed_url_str = malloc(strlen("https://thayer.github.io/engs50/") + 1);
	// Checking for proper Malloc allocation
	if (seed_url_str == NULL) return EXIT_FAILURE;
	strcpy(seed_url_str, "https://thayer.github.io/engs50/");
	// Creating and storing seed url into the first hash element
	hput(htp, seed_url_str, seed_url_str, strlen(seed_url_str));

	//create new webpage
	webpage_t *new = webpage_new("https://thayer.github.io/engs50/", 0, NULL);

	
	//exit on failrure to create
	if (!webpage_fetch(new)) {
		printf("failed to fetch");
		exit(EXIT_FAILURE);
	}

	//loop through webpage for all urls
	int i = 0;
	char *url;

	while ((i = webpage_getNextURL(new, i, &url)) > 0) {
		//check if internal or external
		if (IsInternalURL(url)) {

			// Checking if the internal URL is in the hashtable (Checking if we have seen the URL yet)
			if ( hsearch(htp, searchURL, url, strlen(url)) == NULL ) {

					// This block executes when we did not find the internal URL in the list of URL we have seen already (hashtable)
					// Put the URL into the hashtable of seen URLs
					hput(htp, url, url, strlen(url));

					// Now to create the webpage for the new unvisited internal URL
					webpage_t *wp = webpage_new(url, 0, NULL);

					// Now to append the new webpage into our queue for processing later
					qput(qp, wp);
					
				} else {

					// In this case we have already seen this URL already so we disregard it and continue
					free(url);
				}
			
		} else {

			// The URL is external so we just free it and do nothing with it
			free(url); //make sure to free each url
		}
	}

	//free whole webpage
	webpage_delete(new);

	// Now to iterate through the queue by getting entries and then freeing the memmory of the element
	while ( (queued_wp = (webpage_t*)qget(qp)) != NULL) {
		
		// Printing the URLs that were put in the queue
		printf("Internal URL: %s\n", webpage_getURL(queued_wp));

		// Now to free the weppage structure from the queue
		webpage_delete(queued_wp);
	}

	// Freeing the que structure not that the internal elements have been cleared
	qclose(qp);

	// Lets now clear the elements of the hash by using happly() with free_element() AND then free the internal structure
	happly(htp, free);
	hclose(htp);
	
	exit(EXIT_SUCCESS);
	
	return(0);
	
}
