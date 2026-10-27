/* 
 * indexio.h --- 
 * 
 * Author: Matthew Morales
 * Created: 10-26-2026
 * Version: 1.0
 */

#pragma once
#include "hash.h"

/*
 * indexsave - saves a given hashtable index to a indexnm file
 * 
 * The indx file will contain one line for each word in the index with the following format:
 * <word> <docID1> <count1> <docID2> <count2> ….<docIDN> <countN>
 *
 * Returns:
 * 0 if success, or non-zero on failure
 *
 */
int indexsave(hashtable_t *index, char *indexnm);

/*
 * indexload - loads an index from a file named indexnm
 *
 * Creates a new hashtable structure and populates it with the words, DocIDs, and counts found in the specified file.
 *
 * Returns:
 * A pointer to the new hashtable_t, or NULL if failed
 *
 */

hashtable_t* indexload(char *indexnm);
