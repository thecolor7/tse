/* 
 * test.c --- 
 * 
 * Author: Eve H. Belding
 * Created: 10-19-2026
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
#include "webpage.h"
#include "pageio.h"

int main(void) {
	webpage_t *loaded = NULL;
	webpage_t *loaded2 = NULL;
	
	loaded = pageload(1, pages);
	pagesave(loaded, 2, pages);
	loaded2 = pageload(2, pages);


	//add checks for loaded = loaded2

	
}
