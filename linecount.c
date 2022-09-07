#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_A_EXT 128 /* Maximum number of file extentions */
#define MAX_L_EXT 25  /* Maximum number of characters in file exention */

typedef struct {
    int ext_count; /* Number of file extentions in 'extentions' */
    int lines[MAX_A_EXT]; /* Store lines of code for each extentions */
    bool nocolor; /* If true, will not have color in terminal output */
    unsigned int args_parssed; /* Keeping count of how many valid arguments that
                                  has been passed to the application */
    char *extentions[]; /* Store file extentions to search */
} linecount_s;

static void die(const char *fmt, ...);
static void print_help(char *ex_name);

int main(int argc, char **argv)
{
    if (argc < 2)
        die("%s: not enough arguments\n", argv[0]);

    linecount_s lc;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "-c") == 0) {
            lc.nocolor = true;
            lc.args_parssed++;
        }
    }

    if (argc >= (int) lc.args_parssed) {
        die("%s: no file extentions provided list\n", argv[0]);
    }

    return 0;
}

static void
die(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    exit(EXIT_FAILURE);
}

static void
print_help(char *ex_name) {
    fprintf(stdout,
            "Usage of %s:\n",
            ex_name);
}
