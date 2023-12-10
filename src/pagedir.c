#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "pagedir.h"

bool pagedir_init(const char *pageDirectory) {
    // Check if the directory already exists
    if (access(pageDirectory, F_OK) == 0) {
        printf("Page directory already exists: %s\n", pageDirectory);
        return true;
    }

    // Create the directory if it doesn't exist
    if (mkdir(pageDirectory, 0755) != 0) {
        fprintf(stderr, "Error creating directory: %s\n", pageDirectory);
        return false;
    }

    printf("Initialized page directory: %s\n", pageDirectory);
    return true;
}

void pagedir_save(const webpage_t *page, const char *pageDirectory, const int documentID) {
    // Generate a unique document ID
    // int documentID = generateDocumentID(); // Remove this line

    // Create a file name using the document ID
    char fileName[100];  // Adjust the size as needed
    snprintf(fileName, sizeof(fileName), "%s/page_%d.html", pageDirectory, documentID);

    // Open the file for writing
    FILE *file = fopen(fileName, "w");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for writing: %s\n", fileName);
        return;
    }

    // Write the webpage content to the file
    if (page->html != NULL) {
        fwrite(page->html, 1, page->length, file);
    } else {
        fprintf(stderr, "Error: HTML content is NULL for URL: %s\n", page->url);
    }

    // Close the file
    fclose(file);

    printf("Webpage saved to file: %s\n", fileName);
}
