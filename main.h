#ifndef SRC_GREP_MAIN_H_
#define SRC_GREP_MAIN_H_

#define _GNU_SOURCE

#include <err.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RE_ERROR_BUF 512

typedef struct s_flags {
  bool i;
  bool v;
  bool c;
  bool l;
  bool n;
  bool h;
  bool s;
  bool f;
  bool o;
} s_flags;

void usage();
void* grep_calloc(size_t nmemb, size_t size);
void add_pattern(char*** patterns, int* pattern_count, int* patterns_size,
                 char* pattern, int len);
int process_file(char* filename, bool multiple_files, regex_t* patterns,
                 int pattern_count, s_flags flags);
bool process_line(char* filename, bool multiple_files, char* line,
                  regex_t* patterns, int pattern_count, int line_num,
                  bool muted, s_flags flags);
s_flags parse_flags(int argc, char* argv[], char*** patterns,
                    int* pattern_count, int* patterns_size);
void find_pattern_if_not_exists(int argc, char* argv[], char*** patterns,
                                int* pattern_count, int* patterns_size);
void read_patterns_from_file(char* filename, char*** patterns,
                             int* pattern_count, int* patterns_size,
                             s_flags flags);
regex_t* compile_patterns(char** patterns, int pattern_count, s_flags flags);

#endif  // SRC_GREP_MAIN_H_
