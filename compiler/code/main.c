#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "build_assembly.h"
#include "program_tree.h"

const char* FLAGS[] = {"-f", "-o", "-v"};
const char* KEYWORDS[] = {"print", "printf"};
const int LINE_CHAR_LIMIT = 190;
const char* VERSION_NUMBER = "0.0.0";

// Display welcome message
void display_welcome_message() {
    void print_char(int n, char c) {
        for (int i = 0; i < n; i++) {
            printf("%c", c);
        }
    }

    char* welcome_message = "Welcome to the MathLang compiler! Developed by Tayven Stover. Version: %s";
    int message_length = strlen(welcome_message);
    int version_length = strlen(VERSION_NUMBER);
    int padding = (message_length + version_length + 36) - 2; // Idk why -2 but it works

    print_char(padding, '*');
    printf("\n");
    print_char(10, '*');
    print_char(8, ' ');
    printf(welcome_message, VERSION_NUMBER);
    print_char(8, ' ');
    print_char(10, '*');
    printf("\n");
    print_char(padding, '*');
}

// Function to add a string to the dynamic string array
char** add_string(char** string_array, int* size, int* capacity, const char* new_string) {
    // Check if the string_array is full and needs resizing
    if (*size >= *capacity) {
        *capacity *= 2;  // Double the capacity
        string_array = realloc(string_array, *capacity * sizeof(char*));
        if (string_array == NULL) {
            printf("Memory reallocation failed, exiting...\n");
            exit(1);
        }
    }

    // Allocate memory for the new string and add it to the string_array
    string_array[*size] = malloc((strlen(new_string) + 1) * sizeof(char));
    if (string_array[*size] == NULL) {
        printf("Memory allocation for string failed, exiting...\n");
        exit(1);
    }
    strcpy(string_array[*size], new_string);

    // Increment the size
    (*size)++;

    return string_array;
}

// Reads the command line args and extracts all flag values
char** get_flags(char* argv[], int argc) {
    bool contains_in_file = false;
    bool contains_out_file = false;
    bool contains_version = false;

    // Dynamically allocate memory for flags to persist after function ends
    char** flags = malloc(2 * sizeof(char*));
    flags[0] = NULL;
    flags[1] = NULL;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], FLAGS[0]) == 0) && (argc > i + 1)) {
            flags[0] = argv[i + 1];
            contains_in_file = true;
            i++;
            continue;
        } else if ((strcmp(argv[i], FLAGS[1]) == 0) && (argc > i + 1)) {
            flags[1] = argv[i + 1];
            contains_out_file = true;
            i++;
            continue;
        } else if (strcmp(argv[i], FLAGS[2]) == 0) {
            printf("MathLang compiler version: %s\n", VERSION_NUMBER);
            contains_version = true;
            continue;
        }
    }

    if (!contains_in_file) {
        if (contains_version) {
            free(flags); // Free allocated memory before exiting
            exit(0);
        } else {
            printf("No input file specified, exiting...\n");
            free(flags); // Free allocated memory before exiting
            exit(1);
        }
    }

    if (!contains_out_file) {
        // Allocate enough memory for the concatenated output file name
        size_t in_file_len = strlen(flags[0]);
        size_t suffix_len = strlen("_buildfile");
        flags[1] = malloc(in_file_len + suffix_len + 1); // +1 for null terminator

        if (flags[1] == NULL) {
            printf("Memory allocation failed, exiting...\n");
            free(flags);
            exit(1);
        }

        // Copy the input file name to the new buffer and concatenate the suffix
        strcpy(flags[1], flags[0]);
        strcat(flags[1], "_buildfile");
    }

    return flags;
}

// Verify the input file exists
bool file_exists(char* in_file) {
    return access(in_file, F_OK) != -1;
}

// Reads in the input file for further processing
char** read_input_file(char* in_file, int* line_array_size) {
    // Check if the input file exists
    if (!file_exists(in_file)) {
        printf("No such file or directory found, exiting...\n");
        exit(1);
    }

    FILE *fptr;
    int line_num = 1;
    int capacity = 2;
    int size = 0;
    char** line_array = malloc(capacity * sizeof(char*));

    // Open a file in read mode
    fptr = fopen(in_file, "r");

    // Store the content of the file
    char line_in[LINE_CHAR_LIMIT + 2];

    // Read the content and print it
    while(fgets(line_in, LINE_CHAR_LIMIT + 2, fptr)) {

        // Get the length of the string
        int line_length = strlen(line_in);

        // Check if the line is too long
        if (line_length > LINE_CHAR_LIMIT) {
            printf("Line #%d too long, exiting...\n", line_num);

            printf("%s\n", line_in);

            fclose(fptr);
            exit(1);
        }

        // Add strings to the list
        line_array = add_string(line_array, &size, &capacity, line_in);

        line_num++;
    }

    // Close the file
    fclose(fptr);

    // Update the size of the line_array
    *line_array_size = size;
    return line_array;
}

int main(int argc, char* argv[]) {
    char** flags;
    char* in_file;
    char* out_file;

    // Check for any arguments
    if (argc < 2) {
        display_welcome_message();
        exit(1);
    } else {
        flags = get_flags(argv, argc);

        in_file = flags[0];
        out_file = flags[1];
    }

    // Read the input file
    int line_array_size = 0;
    char** line_array = read_input_file(in_file, &line_array_size);

    // Free allocated memory before exiting
    free(flags);
    free(in_file);
    free(out_file);
    free(line_array);

    return 0;
}
