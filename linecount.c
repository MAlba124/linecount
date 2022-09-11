#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_A_EXT 512 /* Maximum number of file extensions */
#define MAX_L_EXT 64  /* Maximum number of characters in file exention */
#define DELIM ","     /* Delim for strtok file extensions */
#define C_RED "\033[0;31m"
#define C_RESET "\033[0m"

typedef struct {
    char extensions[MAX_A_EXT][MAX_L_EXT];
    unsigned int ext_count; /* Number of file extensions in 'extentions' */
    unsigned long lines[MAX_A_EXT]; /* Store lines of code for each extensions */
    unsigned int args_parssed; /* Keeping count of how many valid arguments that
                                  has been passed to the application */
} LineCount;

/* Function definitions */
static void die(const char *fmt, ...);
static inline void print_help(char *ex_name);
static inline unsigned short not_yet_added(LineCount *lc, const char *ext);
static inline void zero_counter(LineCount *lc);
static void count_lines(LineCount *lc, char *path);
static int traverse_dir(LineCount *lc, char *path);
static inline unsigned short has_extension(char *file, char *ext);

int main(int argc, char **argv)
{
    if (argc < 2)
        die(C_RED"%s"C_RESET": not enough arguments\n", argv[0]);

    unsigned int i, valid_args = 0;
    char *ext = NULL;
    LineCount lc;
    lc.args_parssed = 0;
    for (i = 1; i < (unsigned int) argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "-e") == 0 && (unsigned int) argc >= i+1) {
            ext = argv[i+1];
            valid_args++;
            i++; /* Skip one iteration */
        } else {
            fprintf(stdout, C_RED"%s"C_RESET": invalid argument: %s\n", argv[0], argv[i]);
        }
    }

    if (valid_args < 1)
        die(C_RED"%s"C_RESET": not enough arguments\n", argv[0]);

    if (ext == NULL) /* If no extensions provided, exit */
        die(C_RED"%s"C_RESET": no extensions provided\n", argv[0]);


    char *token;
    token = strtok(ext, DELIM);
    i = 0;
    lc.ext_count = 0;
    /* Tokenize the extensions and save them in array */
    while (token != NULL) {
        memset(lc.extensions[i], 0, MAX_L_EXT);
        if (not_yet_added(&lc, token)) { /* Eliminate duplicants */
            strcpy(lc.extensions[i], token); /* Copy the token/extentions from token
                                            to the extensions array */
            i++;
            lc.ext_count++;
        }
        token = strtok(NULL, DELIM);
    }

    zero_counter(&lc);

    count_lines(&lc, ".");

    unsigned long long total_loc = 0;
    puts("EXT\tLOC\n───────────────────────");
    for (i = 0; i < lc.ext_count; i++) {
        fprintf(stdout, "%s\t%ld\n", lc.extensions[i], lc.lines[i]);
        total_loc += lc.lines[i];
    }
    puts("───────────────────────");
    fprintf(stdout, "TOTAL\t%lld\n", total_loc);

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

static inline void
print_help(char *ex_name) {
    fprintf(stdout,
            "Usage of %s:\n" \
            "\t-e <file extensions separated with ','\n" \
            "\t\texample: -e c,h,cpp,py,js\n" \
            "\t -h, --help (display this message)\n",
            ex_name);
}

static inline unsigned short
not_yet_added(LineCount *lc, const char *ext)
{
    unsigned int i;
    for (i = 0; i < lc->ext_count; i++) {
        if (strcmp(lc->extensions[i], ext) == 0)
            return 0;
    }

    return 1;
}

static inline void
zero_counter(LineCount *lc)
{
    unsigned int i;
    for (i = 0; i < lc->ext_count; i++) {
        lc->lines[i] = 0;
    }
}

static void
count_lines(LineCount *lc, char *path)
{
    int r;
    if ((r = traverse_dir(lc, path)) != 0)
        die("error occured while counting%s", "\n");
}

static int
traverse_dir(LineCount *lc, char *path)
{
    int c;
    FILE *cfile;
    unsigned int i;
    struct dirent *entry;
    struct stat stat_info;
    char location[1028];
    DIR *cd = opendir(path);
    while ((entry = readdir(cd)) != NULL) {
        if (entry->d_name[0] != '.') { /* Ignore current and parent directory */
            snprintf(location, 1027,
                     "%s/%s", path, entry->d_name);
            if (stat(location, &stat_info) == 0) {
                if (S_ISDIR(stat_info.st_mode)) { /* Check if "file" is dir */
                    traverse_dir(lc, location);
                    continue;
                }
                for (i = 0; i < lc->ext_count; i++) {
                    if (has_extension(location, lc->extensions[i])) {
                        /* File has valid extension, start counting the lines */
                        cfile = fopen(location, "r"); /* Open the file */
                        if (cfile == NULL) { /* Exit if failed to open file */
                            fprintf(stderr,
                                    C_RED"Failed to open file: "C_RESET"%s\n",
                                    location);
                            break;
                        }
                        while ((c = getc(cfile)) != EOF) {
                            if (c == '\n')
                                lc->lines[i]++;
                        }
                        fclose(cfile); /* Close the file */
                        break;
                    }
                }
            } else {
                perror("stat");
            }
        }
    }
    closedir(cd);

    return 0;
}

/*
 * Returns 1 if the file variable has the file extension in the ext variable
 */
static inline unsigned short
has_extension(char *file, char *ext)
{
    int f_len, e_len, i, ec = 0;
    f_len = strlen(file);
    e_len = strlen(ext);
    if (file[(i = f_len-e_len-1)] != '.') return 0;
    for (i = i+1; i < f_len; i++) {
        if (file[i] != ext[ec]) return 0;
        ec++;
    }

    return 1;
}
