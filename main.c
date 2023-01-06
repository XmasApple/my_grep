#include "main.h"

void usage() {
  printf("Usage: grep [OPTION]... PATTERNS [FILE]...\n");
  printf("Try 'grep --help' for more information.\n");
}

void* safe_calloc(size_t nmemb, size_t size) {
  void* ptr;
  if ((ptr = calloc(nmemb, size)) == NULL) err(2, "calloc");
  return (ptr);
}

void add_pattern(char*** patterns, int* pattern_count, int* patterns_size,
                 char* pattern, int len) {
  if (pattern[len - 1] == '\n') {
    pattern[len - 1] = '\0';
    len--;
  }
  if (*pattern_count >= *patterns_size) {
    *patterns_size *= 2;
    (*patterns_size) += 1;
    char** new_patterns =
        safe_calloc(*patterns_size, *patterns_size * sizeof(char**));
    memcpy(new_patterns, *patterns, *pattern_count * sizeof(char**));
    free(*patterns);
    *patterns = new_patterns;
  }
  (*patterns)[*pattern_count] = malloc(len + 1 * sizeof(char));
  memcpy((*patterns)[*pattern_count], pattern, len);
  (*patterns)[*pattern_count][len] = '\0';
  (*pattern_count)++;
}

int process_file(char* filename, bool multiple_files, regex_t* patterns,
                 int pattern_count, s_flags flags) {
  int matches = 0;
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    if (!flags.s)
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  } else {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_num = 1;
    bool muted = flags.c | flags.l;
    while ((read = getline(&line, &len, file)) != -1)
      matches += process_line(filename, multiple_files, line, patterns,
                              pattern_count, line_num++, muted, flags);
    if (muted) {
      if (flags.l) {
        if (matches > 0) printf("%s\n", filename);
      } else {
        if (multiple_files && !flags.h) printf("%s:", filename);
        if (flags.c && !flags.l) printf("%d", matches);
        printf("\n");
      }
    }
    free(line);
    fclose(file);
  }

  return matches;
}

bool process_line(char* filename, bool multiple_files, char* line,
                  regex_t* patterns, int pattern_count, int line_num,
                  bool muted, s_flags flags) {
  bool r = false;
  regmatch_t match;
  if (flags.o) {
    muted |= flags.v;
    while (strlen(line) > 0) {
      bool match_found = false;
      for (int i = 0; i < pattern_count; i++) {
        if (regexec(&patterns[i], line, 1, &match, 0) == 0) {
          if (!muted) {
            if (multiple_files && !flags.h) printf("%s:", filename);
            if (flags.n) printf("%d:", line_num);
            printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                   &line[match.rm_so]);
          }
          r = match_found = true;
          line += match.rm_eo;
          break;
        }
      }
      if (!match_found) break;
    }
  } else {
    for (int i = 0; i < pattern_count; i++) {
      r |= !regexec(&patterns[i], line, 1, &match, 0);
    }
    if (r ^ flags.v) {
      if (!muted) {
        if (multiple_files && !flags.h) printf("%s:", filename);
        if (flags.n) printf("%d:", line_num);
        if (line[strlen(line) - 1] != '\n')
          printf("%s\n", line);
        else
          printf("%s", line);
      }
    }
  }
  return r ^ flags.v;
}

s_flags parse_flags(int argc, char* argv[], char*** patterns,
                    int* pattern_count, int* patterns_size) {
  char opt;
  s_flags flags = {false, false, false, false, false,
                   false, false, false, false};
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        add_pattern(patterns, pattern_count, patterns_size, optarg,
                    strlen(optarg));
        break;
      case 'i':
        flags.i = true;
        break;
      case 'v':
        flags.v = true;
        break;
      case 'c':
        flags.c = true;
        break;
      case 'l':
        flags.l = true;
        break;
      case 'n':
        flags.n = true;
        break;
      case 'h':
        flags.h = true;
        break;
      case 's':
        flags.s = true;
        break;
      case 'f':
        flags.f = true;
        read_patterns_from_file(optarg, patterns, pattern_count, patterns_size,
                                flags);
        break;
      case 'o':
        flags.o = true;
        break;
      default:
        usage();
        break;
    }
  }
  return flags;
}

void read_patterns_from_file(char* filename, char*** patterns,
                             int* pattern_count, int* patterns_size,
                             s_flags flags) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    if (!flags.s)
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
  } else {
    char* line;
    size_t len;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1)
      add_pattern(patterns, pattern_count, patterns_size, line, strlen(line));
    free(line);
  }
  fclose(file);
}

void find_pattern_if_not_exists(int argc, char* argv[], char*** patterns,
                                int* pattern_count, int* patterns_size) {
  if (*pattern_count == 0 && optind < argc) {
    add_pattern(patterns, pattern_count, patterns_size, argv[optind],
                strlen(argv[optind]));
  } else if (*pattern_count == 0) {
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] != '-') {
        add_pattern(patterns, pattern_count, patterns_size, argv[i],
                    strlen(argv[i]));
        break;
      }
    }
    if (*pattern_count == 0) {
      usage();
      exit(EXIT_FAILURE);
    }
  }
}

regex_t* compile_patterns(char** patterns, int pattern_count, s_flags flags) {
  int reti;
  char re_error[RE_ERROR_BUF];
  regex_t* compiled_patterns = safe_calloc(pattern_count, sizeof(regex_t));
  for (int i = 0; i < pattern_count; i++) {
    int cflags = REG_EXTENDED;
    if (flags.i) cflags |= REG_ICASE;
    reti = regcomp(&compiled_patterns[i], patterns[i], cflags);
    if (reti) {
      regerror(reti, &compiled_patterns[i], re_error, RE_ERROR_BUF);
      errx(2, "%s", re_error);
    }
  }
  return compiled_patterns;
}

int main(int argc, char* argv[]) {
  int patterns_size = 1;
  char** patterns = malloc(patterns_size * sizeof(*patterns));
  int pattern_count = 0;
  s_flags flags =
      parse_flags(argc, argv, &patterns, &pattern_count, &patterns_size);
  find_pattern_if_not_exists(argc, argv, &patterns, &pattern_count,
                             &patterns_size);
  regex_t* r_pattern = compile_patterns(patterns, pattern_count, flags);

  int file_count = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' &&
        !(argv[i - 1][0] == '-' &&
          ((argv[i - 1][1] == 'e' || argv[i - 1][1] == 'f') &&
           argv[i - 1][2] == 0)) &&
        strcmp(argv[i], patterns[0])) {
      file_count++;
    }
  }
  bool multiple_files = file_count > 1;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-' &&
        !(argv[i - 1][0] == '-' &&
          ((argv[i - 1][1] == 'e' || argv[i - 1][1] == 'f') &&
           argv[i - 1][2] == 0)) &&
        strcmp(argv[i], patterns[0])) {
      process_file(argv[i], multiple_files, r_pattern, pattern_count, flags);
    }
  }

  for (int i = 0; i < pattern_count; i++) {
    free(patterns[i]);
    regfree(&r_pattern[i]);
  }
  free(patterns);
  free(r_pattern);

  return 0;
}
