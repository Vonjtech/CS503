#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Function prototypes
void display_help();
int calculate_word_count(const char *input);
void reverse_input_string(char *input);
void display_words_and_lengths(const char *input);

int main() {
    char option[3];
    char input_string[1000];

    // Get input string from user
    printf("Enter input string: ");
    fgets(input_string, sizeof(input_string), stdin);
    input_string[strcspn(input_string, "\n")] = 0;  // Remove newline

    // Get option from user
    printf("Enter option (-h, -c, -r, or -w): ");
    fgets(option, sizeof(option), stdin);
    option[strcspn(option, "\n")] = 0;  // Remove newline

    // Process the command-line option
    if (strcmp(option, "-h") == 0) {
        display_help();
    } else if (strcmp(option, "-c") == 0) {
        int word_count = calculate_word_count(input_string);
        printf("Total Words: %d\n", word_count);
    } else if (strcmp(option, "-r") == 0) {
        reverse_input_string(input_string);
        printf("Reversed String: %s\n", input_string);
    } else if (strcmp(option, "-w") == 0) {
        display_words_and_lengths(input_string);
    } else {
        fprintf(stderr, "Error: Invalid option. Use -h for help.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Display help information for the program
void display_help() {
    printf("Text Line Processor Utility\n");
    printf("Usage: Enter an input string, then choose an option -[h|c|r|w]\n");
    printf("Options:\n");
    printf("  -h    Display this help message\n");
    printf("  -c    Count the number of words in the input string\n");
    printf("  -r    Reverse the characters in the input string\n");
    printf("  -w    Display words and their respective lengths in the input string\n");
}

// Calculate the number of words in the input string
int calculate_word_count(const char *input) {
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        if (isspace(input[i])) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            word_count++;
        }
    }

    return word_count;
}

// Reverse the input string in place
void reverse_input_string(char *input) {
    size_t length = strlen(input);
    for (size_t i = 0; i < length / 2; i++) {
        char temp = input[i];
        input[i] = input[length - i - 1];
        input[length - i - 1] = temp;
    }
}

// Display each word and its length from the input string
void display_words_and_lengths(const char *input) {
    const char *start = input;
    while (*start != '\0') {
        while (isspace(*start)) {
            start++;
        }

        const char *end = start;
        while (*end != '\0' && !isspace(*end)) {
            end++;
        }

        if (end > start) {
            printf("Word: '%.*s', Length: %ld\n", (int)(end - start), start, end - start);
        }
        start = end;
    }
}
