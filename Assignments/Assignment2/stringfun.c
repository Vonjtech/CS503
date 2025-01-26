#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SZ 50

// Function prototypes
int setup_buff(char *buff, const char *user_str, int buff_size);
int count_words(const char *buff, int buff_size, int str_len);
void reverse_string(char *buff, int str_len);
void print_words(const char *buff, int str_len);
void print_help();
void print_buff(const char *buff, int buff_size);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "error: Incorrect usage. Use -h for help.\n");
        exit(1);
    }

    char *option = argv[1];
    char *user_str = argv[2];

    char *buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        fprintf(stderr, "error: Memory allocation failed.\n");
        exit(2);
    }

    int str_len = setup_buff(buff, user_str, BUFFER_SZ);
    if (str_len < 0) {
        fprintf(stderr, "error: Provided input string is too long.\n");
        free(buff);
        exit(3);
    }

    if (option[0] == '-' && option[1] != '\0') {
        switch (option[1]) {
            case 'h':
                print_help();
                break;
            case 'c':
                printf("Word Count: %d\n", count_words(buff, BUFFER_SZ, str_len));
                break;
            case 'r':
                reverse_string(buff, str_len);
                printf("Reversed String: %.*s\n", str_len, buff);
                break;
            case 'w':
                print_words(buff, str_len);
                break;
            case 'x':
                if (argc != 5) {
                    fprintf(stderr, "error: Incorrect usage. Provide two additional strings.\n");
                    free(buff);
                    exit(3);
                }
                printf("Not Implemented!\n");
                break;
            default:
                fprintf(stderr, "error: Unknown option. Use -h for help.\n");
                free(buff);
                exit(1);
        }
    } else {
        fprintf(stderr, "error: Invalid command format.\n");
        free(buff);
        exit(1);
    }

    print_buff(buff, BUFFER_SZ);
    free(buff);
    return 0;
}

int setup_buff(char *buff, const char *user_str, int buff_size) {
    char *b = buff;
    const char *s = user_str;
    int count = 0;
    int space_flag = 0;

    while (*s != '\0' && count < buff_size) {
        if (*s == ' ' || *s == '\t') {
            if (!space_flag && count > 0) {
                *b++ = ' ';
                count++;
                space_flag = 1;
            }
        } else {
            *b++ = *s;
            count++;
            space_flag = 0;
        }
        s++;
    }

    if (count >= buff_size)
        return -1; // Buffer overflow

    while (count < buff_size) {
        *b++ = '.';
        count++;
    }

    return count;
}

int count_words(const char *buff, int buff_size, int str_len) {
    int count = 0;
    int in_word = 0;
    const char *p = buff;

    for (int i = 0; i < str_len; i++) {
        if (*p != ' ' && *p != '.') {
            if (!in_word) {
                count++;
                in_word = 1;
            }
        } else {
            in_word = 0;
        }
        p++;
    }
    return count;
}

void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;

    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void print_words(const char *buff, int str_len) {
    printf("Word Print\n----------\n");
    int count = 1;
    const char *p = buff;
    const char *start = NULL;

    for (int i = 0; i < str_len; i++) {
        if (*p != ' ' && *p != '.') {
            if (start == NULL)
                start = p;
        } else {
            if (start != NULL) {
                printf("%d. %.*s (%ld)\n", count++, (int)(p - start), start, p - start);
                start = NULL;
            }
        }
        p++;
    }

    if (start != NULL)
        printf("%d. %.*s (%ld)\n", count, (int)(p - start), start, p - start);
}

void print_help() {
    printf("Usage: stringfun -[h|c|r|w|x] \"sample string\" [other_options]\n");
    printf("  -h    Prints help\n");
    printf("  -c    Counts words in the string\n");
    printf("  -r    Reverses the string\n");
    printf("  -w    Prints words with lengths\n");
    printf("  -x    Replace first occurrence of a word with another\n");
}

void print_buff(const char *buff, int buff_size) {
    printf("Buffer: %.*s\n", buff_size, buff);
}
