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
#include <webpage.h>
//make sure to link .a file in make for .h files

int main (void) {

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
			printf("Internal url : %s\n", url);
		} else {
			printf("External url : %s\n", url);
		}
		free(url); //make sure to free each url
	}

	//free whole webpage
	webpage_delete(new);
	exit(EXIT_SUCCESS);
	
	return(0);
	
}
