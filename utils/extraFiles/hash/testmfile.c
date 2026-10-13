/* 
 * testmfile.c --- 
 * 
 * Author: Matthew Morales
 * Created: 10-10-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

// Loading Necessary Libraries and Header Files
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hash.h"
#include "queue.h"

// Necessary Struct
typedef struct car {
    struct car *next;
    char plate[50];
    double price;
    int year;
} car_t;

// Necessary Functions
car_t *make_c(const char *platep, double price, int year) {
    car_t *cp;
    
    if (!(cp = (car_t*)malloc(sizeof(car_t)))) {
        printf("[Error: malloc failed allocating car]\n");
        return NULL;
    }
    
    cp->next = NULL;
    strcpy(cp->plate, platep);
    cp->price = price;
    cp->year = year;
    return cp;
}

// Search function to match plates
bool search_plate(void *elementp, const void *searchkeyp) {
    car_t *car = (car_t *)elementp;
    const char *key = (const char *)searchkeyp;
    return (strcmp(car->plate, key) == 0);
}

int main(void) {
    int failures = 0;
    int testResult = 0;
    hashtable_t *hashTable1 = NULL;
    hashtable_t *hashTable2 = NULL;
		
    printf("--- Starting Hash Module Tests ---\n\n");

    // Test 1: Null Input Verification
    printf("Test 1: Checking Behavior of hput() and hsearch() with NULL inputs\n");
    testResult = hput(NULL, NULL, "key", 3);
    void *searchResult = hsearch(NULL, search_plate, NULL, 0);
    
    if (testResult == 1 && searchResult == NULL) {
        printf("SUCCESS: Functions tested safely rejected NULL  inputs.\n\n");
    } else {
        printf("FAILED: Functions did not return the expected outputs with NULL inputs.\n\n");
        failures++;
    }

    // Test 2: Initializing a Hashtable
    printf("Test 2: Checking for  proper Initialization of Hashtable\n");
    hashTable1 = hopen(5);
    if (hashTable1 != NULL) {
        printf("SUCCESS: hopen(5) created a hash table of length 5.\n\n");
    } else {
        printf("FAILED: hopen(5) returned NULL, failed to create hashtable properly.\n\n");
        failures++;
    }

    // Test 3: Testing Null inputs to hclose()
    printf("Test 3: Testing null inputs to hclose()\n");
    hclose(NULL);
    printf("SUCCESS: hclose(NULL) executed safely without a segmentation fault.\n\n");

    // Test 4: Testing hclose() with valid inputs
    printf("Test 4: Testing hclose() with valid inputs\n");
    hclose(hashTable1);
    hashTable1 = NULL;
    printf("SUCCESS: hclose() closed the hash table successfully.\n\n");

    // Test 5: Testing the ability for multiple hashtables can exist at one time
    printf("Test 5: Testing that multiple hashtables can exist\n");
    hashTable1 = hopen(5);
    hashTable2 = hopen(5);
    if (hashTable1 != NULL && hashTable2 != NULL && hashTable1 != hashTable2) {
        printf("SUCCESS: Multiple independent hash tables opened successfully.\n\n");
    } else {
        printf("FAILED: Multiple hash tables could not be initialized properly.\n\n");
        failures++;
    }

    // Test 6: Closing second hashtable
    printf("Test 6: Closing Second Hastable\n");
    hclose(hashTable2);
    hashTable2 = NULL;
    printf("SUCCESS: hashTable2 successfully closed while hashTable1 remains initialized.\n\n");

    // Test 7: Populating the hashtable
    printf("Test 7: Populating the Hashtable\n");
    car_t *MattCar = make_c("MattCar_Plate", 1000.0, 2026);
    car_t *EveCar  = make_c("EveCar_Plate", 1000.0, 2026);
    car_t *GenCar  = make_c("GenCar_Plate", 1000.0, 2026);
    
    int p1 = hput(hashTable1, MattCar, MattCar->plate, strlen(MattCar->plate));
    int p2 = hput(hashTable1, EveCar, EveCar->plate, strlen(EveCar->plate));
    int p3 = hput(hashTable1, GenCar, GenCar->plate, strlen(GenCar->plate));

    if (p1 == 0 && p2 == 0 && p3 == 0) {
        printf("SUCCESS: MattCar, EveCar, and GenCar successfully added using hput().\n\n");
    } else {
        printf("FAILED: hput() failed to insert cars properly.\n\n");
        failures++;
    }

    // Test 8: Inserting Test_Car to test if a second queue is generated as expected
    printf("Test 8: Inserting Test_Car with different liscence plate \n");
    car_t *Test_Car = make_c("dart100...0", 500.0, 2026);
    int p4 = hput(hashTable1, Test_Car, Test_Car->plate, strlen(Test_Car->plate));
    if (p4 == 0) {
        printf("SUCCESS: Test_Car successfully added to hashtable.\n\n");
    } else {
        printf("FAILED: hput() failed to insert Test_Car.\n\n");
        failures++;
    }

    // Test 9: Testing hsearch() in the Hash Table
    printf("Test 9: Searching the Hash Table for test cars liscence plate\n");
    car_t *foundCar = (car_t *)hsearch(hashTable1, search_plate, "dart100...0", strlen("dart100...0"));
    if (foundCar != NULL && strcmp(foundCar->plate, "dart100...0") == 0 && foundCar->price == 500.0) {
        printf("SUCCESS: hsearch() successfully found Test_Car using the search key.\n\n");
    } else {
        printf("FAILED: hsearch() could not find Test_Car.\n\n");
        failures++;
    }

    // Test 10: Testing data accessing and closing hash
    printf("Test 10: Data Accessing\n");
    if (foundCar != NULL) {
        printf("    Retrieved Data -> Plate: %s | Price: $%.2f | Year: %d\n", foundCar->plate, foundCar->price, foundCar->year);
    }
    
    // Free the dynamically allocated car structs to pass Valgrind
    free(MattCar);
    free(EveCar);
    free(GenCar);
    free(Test_Car);
    
    // Close the hashtable structure
    hclose(hashTable1);
    printf("SUCCESS: Cars have been freed and hash table closed.\n\n");

    // Return Logic
    if (failures == 0) {
        printf("SUCCESS: All tests passed!\n\n");
        return EXIT_SUCCESS;
    } else {
        printf("FAILED: %d tests Failed\n\n", failures);
        return EXIT_FAILURE;
    }
}
