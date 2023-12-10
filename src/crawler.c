#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crawler.h"
#include "curl.h"
#include "hashtable.h"
#include "bag.h"
#include "pagedir.h"
#include "url.h"

/**
 * Parses command-line arguments, placing the corresponding values into the pointer arguments seedURL,
 * pageDirectory and maxDepth. argc and argv should be passed in from the main function.
 */
static void parseArgs(const int argc, char *argv[], char **seedURL, char **pageDirectory, int *maxDepth) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <seedURL> <pageDirectory> <maxDepth>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    *seedURL = argv[1];
    *pageDirectory = argv[2];
    *maxDepth = atoi(argv[3]);

    if (*maxDepth <= 0) {
        fprintf(stderr, "Error: maxDepth must be a positive integer\n");
        exit(EXIT_FAILURE);
    }
}

static void pageScan(webpage_t *page, bag_t *pagesToCrawl, hashtable_t *pagesSeen) {
    if (page->html == NULL) {
        fprintf(stderr, "Error: HTML content is NULL\n");
        return;
    }

    char *html = page->html;

    // Iterate through the HTML to find links
    char *linkStart = html;
    char *linkEnd;

    while ((linkStart = strstr(linkStart, "<a href=\""))) {
        // Move pointer to the start of the URL
        linkStart += strlen("<a href=\"");

        // Find the end of the URL
        linkEnd = strchr(linkStart, '\"');

        // Check if linkEnd is NULL (i.e., if '\"' is not found)
        if (!linkEnd) {
            // Invalid format, move to the next occurrence
            linkStart++;
            continue;
        }

        // Extract the URL
        char *url = strndup(linkStart, linkEnd - linkStart);

        // Normalize the URL
        char *normalizedURL = normalizeURL(page->url, url);

        // Print the extracted and normalized URLs for debugging
        // printf("Extracted URL: %s, Normalized URL: %s\n", url, normalizedURL);

        // Check if the normalized URL is internal and not seen before
        if (isInternalURL(page->url, normalizedURL) && !hashtable_find(pagesSeen, normalizedURL)) {
            // Insert the URL into the hashtable and bag
            hashtable_insert(pagesSeen, normalizedURL, NULL);

            // Create a dynamically allocated webpage_t for the new URL
            webpage_t *newWebpage = malloc(sizeof(webpage_t));
            if (newWebpage == NULL) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }

            // Initialize the new webpage
            newWebpage->url = normalizedURL;
            newWebpage->html = NULL;  // You might want to fetch the HTML later
            newWebpage->length = 0;
            newWebpage->depth = page->depth + 1;

           // Insert the dynamically allocated webpage into the bag
            addToBag(pagesToCrawl, newWebpage);
        }

        // Free memory for URL
        free(url);
        // free(normalizedURL);

        // Move to the next occurrence
        linkStart = linkEnd + 1;
    }
}

void fetchWebpage(webpage_t *webpage) {
    webpage->html = download(webpage->url, &webpage->length);
}

// Global counter for generating unique document IDs
static int documentCounter = 0;

// Function to generate a unique document ID
static int generateDocumentID(void) {
    return documentCounter++;
}

/**
 * Crawls webpages given a seed URL, a page directory, and a max depth.
 */
static void crawl(char *seedURL, char *pageDirectory, const int maxDepth) {
    // Initialize libcurl
    // CURL *curl = curl_easy_init();
    // if (!curl) {
    //     fprintf(stderr, "libcurl initialization failed\n");
    //     exit(EXIT_FAILURE);
    // }

    // Initialize the page directory
    // if (!pagedir_init(pageDirectory)) {
    //     fprintf(stderr, "Failed to initialize the page directory\n");
    //     exit(EXIT_FAILURE);
    // }

    // Initialize hashtable and bag
    hashtable_t *pagesSeen = hashtable_new(100);  // Choose an appropriate size
    bag_t *pagesToCrawl = initializeBag();

    // Initialize webpage structure for seedURL
    webpage_t seedPage = {seedURL, NULL, 0, 0};  // Depth starts from 0

    // Add seedURL to the bag and hashtable
    addToBag(pagesToCrawl, &seedPage);

    while (!isEmpty(pagesToCrawl)) {
        // Extract a webpage (URL, depth) item from the bag
        webpage_t *currentPage = (webpage_t *) removeFromBag(pagesToCrawl);

        // Use libcurl to retrieve the webpage for that URL
        fetchWebpage(currentPage);

        // Pause for one second
        sleep(1);

        // Save the webpage to the pageDirectory with a unique document ID
        pagedir_save(currentPage, pageDirectory, generateDocumentID());

        // If the webpage depth is less than maxDepth, explore the webpage to find links
        if (currentPage->depth < maxDepth) {
            pageScan(currentPage, pagesToCrawl, pagesSeen);
        }

        // Cleanup memory for the current page
        // free(currentPage->url);
        // if (currentPage) {
        //     if(currentPage->html) free(currentPage->html);
        // }
    }

    // Cleanup hashtable and bag
    hashtable_delete(pagesSeen, NULL);
    freeBag(pagesToCrawl);
}

int main(const int argc, char *argv[]) {
    char *seedURL;
    char *pageDirectory;
    int maxDepth;

    // Parse command-line arguments
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    // Start crawling
    crawl(seedURL, pageDirectory, maxDepth);
    return 0;
}
