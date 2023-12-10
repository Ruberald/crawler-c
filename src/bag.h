#ifndef BAG_H
#define BAG_H
#include <stdio.h>
#include <stdlib.h>

// Define the structure for a node in the bag
typedef struct node_t {
    void *data;           // Pointer to data
    struct node_t *next;  // Pointer to the next node
} node_t;

// Define the bag structure
typedef struct bag_t {
    node_t *front;  // Pointer to the front of the bag
    node_t *rear;   // Pointer to the rear of the bag
} bag_t;

// Initialize an empty bag
bag_t *initializeBag();

// Check if the bag is empty
int isEmpty(bag_t *bag);

// Add data to the rear of the bag
void addToBag(bag_t *bag, void *data);

// Remove and return data from the front of the bag
void *removeFromBag(bag_t *bag);

// Free the memory used by the bag
void freeBag(bag_t *bag);

#endif 
