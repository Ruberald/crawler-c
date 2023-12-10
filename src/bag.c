#include <stdio.h>
#include <stdlib.h>

#include "bag.h"

// Initialize an empty bag
bag_t *initializeBag() {
    bag_t *bag = malloc(sizeof(bag_t));
    if (bag == NULL) {
        fprintf(stderr, "Memory allocation for bag failed\n");
        exit(EXIT_FAILURE);
    }

    bag->front = bag->rear = NULL;
    return bag;
}

// Check if the bag is empty
int isEmpty(bag_t *bag) {
    return bag->front == NULL;
}

// Add data to the rear of the bag
void addToBag(bag_t *bag, void *data) {
    node_t *newNode = malloc(sizeof(node_t));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation for bag node failed\n");
        exit(EXIT_FAILURE);
    }

    newNode->data = data;
    newNode->next = NULL;

    if (isEmpty(bag)) {
        bag->front = bag->rear = newNode;
    } else {
        bag->rear->next = newNode;
        bag->rear = newNode;
    }
}

// Remove and return data from the front of the bag
void *removeFromBag(bag_t *bag) {
    if (isEmpty(bag)) {
        fprintf(stderr, "Error: Bag is empty\n");
        exit(EXIT_FAILURE);
    }

    node_t *frontNode = bag->front;
    void *data = frontNode->data;

    bag->front = frontNode->next;
    free(frontNode);

    // If bag is empty after removal, update rear pointer
    if (isEmpty(bag)) {
        bag->rear = NULL;
    }

    return data;
}

// Free the memory used by the bag
void freeBag(bag_t *bag) {
    while (!isEmpty(bag)) {
        removeFromBag(bag);
    }
    free(bag);
}

