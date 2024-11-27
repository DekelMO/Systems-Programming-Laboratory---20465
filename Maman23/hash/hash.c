#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 29
typedef struct HashNode {
    const char *filename;
    int count;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[TABLE_SIZE];
} HashTable;

/* Create a new hash node*/
HashNode* createNode(const char* filename) {
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    newNode->filename = filename;  
    newNode->count = 1;
    newNode->next = NULL;
    return newNode;
}

/* Initialize the hash table*/
void initHashTable(HashTable* table) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
}

/* Insert or update a node in the hash table*/
void insert(HashTable* table, int index, const char* filename) {
    HashNode* current = table->buckets[index];
    HashNode* preCurrent = NULL;
    HashNode* newNode;
    /* Check if the file is already in the list*/
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            current->count++;
            return;
        }
        preCurrent = current;
        current = current->next;
    }

    /* Create a new node and add it to the list*/
    newNode = createNode(filename);
    if(preCurrent == NULL)
        table->buckets[index] = newNode;
    else
        preCurrent->next =newNode;
}
void printHashTable(HashTable* table) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        if (current != NULL) {
            printf("%d appears in", i);
            while (current != NULL) {
                printf(" file %s - %d time%s", current->filename, current->count, (current->count > 1) ? "s" : "");
                current = current->next;
                if(current != NULL)
                    printf(",");
            }
            printf("\n");
        }
    }
}

/* Free the hash table*/
void freeHashTable(HashTable* table) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* toFree = current;
            current = current->next;
            free(toFree);
        }
    }
}




int main(int argc, char *argv[]) {
    int i;
    int number;
    HashTable table;
    initHashTable(&table);
    if (argc < 2) {
        fprintf(stderr, "Error: No input files provided.\nUsage: %s file1 file2 ... fileN\n", argv[0]);
        return 1;
    }
    for (i = 1; i < argc; i++) {
        FILE* file = fopen(argv[i], "r");
        if (file == NULL) {
            fprintf(stderr, "Could not open file %s\n", argv[i]);
            freeHashTable(&table);
            return 1;
        }

        while (fscanf(file, "%d", &number) != EOF) {
            if (number >= 0 && number < TABLE_SIZE) {
                insert(&table, number, argv[i]);
            } else {
                fprintf(stderr, "Invalid number %d in file %s\n", number, argv[i]);
            }
        }

        fclose(file);
    }

    printHashTable(&table);
    freeHashTable(&table);

    return 0;
}

