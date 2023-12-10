#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"

hashtable_t *hashtable_new(const int size) {
    if (size <= 0) {
        return NULL;
    }

    hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
    if (ht == NULL) {
        return NULL;
    }

    ht->size = size;
    ht->table = (Node **)calloc(size, sizeof(Node *));
    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }

    return ht;
}

static unsigned int hash(const char *key, const int size) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash % size;
}

static Node *createNode(const char *key, void *value) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL) {
        return NULL;
    }
    newNode->key = strdup(key);
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

bool hashtable_insert(hashtable_t *ht, const char *key, void *item) {
    if (ht == NULL || key == NULL || item == NULL) {
        return false;
    }

    unsigned int index = hash(key, ht->size);

    Node *current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return false;
        }
        current = current->next;
    }

    Node *newNode = createNode(key, item);
    if (newNode == NULL) {
        return false;
    }

    newNode->next = ht->table[index];
    ht->table[index] = newNode;
    return true;
}

void *hashtable_find(hashtable_t *ht, const char *key) {
    if (ht == NULL || key == NULL) {
        return NULL;
    }

    unsigned int index = hash(key, ht->size);
    Node *current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void hashtable_print(hashtable_t *ht, FILE *fp, void (*itemprint)(FILE *fp, const char *key, void *item)) {
    if (ht == NULL || fp == NULL || itemprint == NULL) {
        return;
    }

    for (int i = 0; i < ht->size; i++) {
        Node *current = ht->table[i];
        while (current != NULL) {
            itemprint(fp, current->key, current->value);
            current = current->next;
        }
    }
}

void hashtable_iterate(hashtable_t *ht, void *arg, void (*itemfunc)(void *arg, const char *key, void *item)) {
    if (ht == NULL || itemfunc == NULL) {
        return;
    }

    for (int i = 0; i < ht->size; i++) {
        Node *current = ht->table[i];
        while (current != NULL) {
            itemfunc(arg, current->key, current->value);
            current = current->next;
        }
    }
}

static void freeNode(Node *node, void (*itemdelete)(void *)) {
    if (node == NULL) {
        return;
    }

    freeNode(node->next, itemdelete);
    if (itemdelete != NULL) {
        itemdelete(node->value);
    }
    free(node->key);
    free(node);
}

void hashtable_delete(hashtable_t *ht, void (*itemdelete)(void *)) {
    if (ht == NULL) {
        return;
    }

    for (int i = 0; i < ht->size; i++) {
        freeNode(ht->table[i], itemdelete);
    }

    free(ht->table);
    free(ht);
}
