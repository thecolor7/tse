/* 
 * pageio.c --- c file which implements pageio.h
 * 
 * Author: Eve H. Belding
 * Created: 10-19-2026
 * Version: 1.0
 * 
 * Description: functions for managing the pages found by crawler
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "webpage.h"
#include "pageio.h"

int32_t pagesave(webpage_t *pagep, int id, char *dirnm) {
  bool writable = false;
  struct stat buffer;
  FILE *fp = NULL;

  //Create filename from integer id
  char filename[10];

  //check that the directory is writable
  if (access(dirnm, W_OK) == 0) {
    writable = true;
  }

  if (writable) {
    //open and check for the file - file needs path to directory as well
    sprintf(filename, "./%s/%d", dirnm, id);

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
    printf("Can't access %s\n", dirnm);
    return(1);
  }
  return(0);
}

webpage_t *pageload(int id, char *dirnm) {
	bool writable = false;
  FILE *fp = NULL;
  //Create filename from integer id
  char filename[10];

	webpage_t *new = NULL;
	char *url = NULL;
	int depth = 0;
	int len = 0;
	char *html = NULL;

  //check that the directory is writable
  if (access(dirnm, W_OK) == 0) {
    writable = true;
  }

  if (writable) {
    //open and check for the file - file needs path to directory as well
    sprintf(filename, "./%s/%d", dirnm, id);

		//no need for stat because reading and not writing
    fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("File %s could not be opened\n", filename);
      return(NULL);
    }

		fscanf(fp, "%s", url);
		fscanf(fp, "%d", &depth);
		fscanf(fp, "%d", &len);

		//how to handle html that might be in a bad format
		html = malloc(len*sizeof(char) + 1);
		if (html == NULL) {
			printf("Malloc failed\n");
			fclose(fp);
			return(NULL);
		}
		
		int i = 0;
		char ch;
		do {
			ch = fgetc(fp);	
			html[i] = ch;
			i++;
		} while (ch != EOF && ch != '\n');
			
		html[i] = '\0';

		//create new webpage
		new = webpage_new(url, depth, html);
		free(html);
		html = NULL;
		fclose(fp);
		return(new); //handles making new NULL if it fails
	}
	return(NULL);
}
