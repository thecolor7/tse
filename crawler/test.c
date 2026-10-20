/* 
 * test.c --- tests the functionality of pageload and pagesave 
 * 
 * Author: Eve H. Belding
 * Created: 10-19-2026
 * Version: 1.0
 * 
 * Description: goes back and forth between the two functions and check for the same results
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include "webpage.h"
#include "pageio.h"

int main(void) {
	webpage_t *loaded = NULL;
	webpage_t *loaded2 = NULL;
	
	//translate between loading and saving a page
	loaded = pageload(1, "pages");
	printf("loaded url = %s\n", webpage_getURL(loaded));
	fflush(stdout);
	pagesave(loaded, 2, "pages");
	loaded2 = pageload(2, "pages");

	//boolean values for checks
	bool depths = (webpage_getDepth(loaded) == webpage_getDepth(loaded2));
	bool lens = (webpage_getHTMLlen(loaded) == webpage_getHTMLlen(loaded2));
	bool url = false;
	bool html = false;
	if (strcmp(webpage_getURL(loaded), webpage_getURL(loaded2)) == 0) {
		url = true;
	}
	if (strcmp(webpage_getHTML(loaded), webpage_getHTML(loaded2)) == 0) {
		html = true;
	}

	//check that all values match and only return success for all correct
	if (depths && lens && url && html) {
		printf("The two webpages match!\n");
		webpage_delete(loaded);
		loaded = NULL;
		webpage_delete(loaded2);
		loaded2 = NULL;
		exit(EXIT_SUCCESS);
		//prints for what failed
	} else {
		if (!depths) {
			printf("depths failed\n");
		}
		if (!lens) {
			printf("lens failed\n");
		}
		if (!url) {
			printf("url failed\n");
		}
		if (!html) {
			printf("html failed\n");
		}
	}
	webpage_delete(loaded);
	loaded = NULL;
	webpage_delete(loaded2);
	loaded2 = NULL;
	exit(EXIT_FAILURE);
}
