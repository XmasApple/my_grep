### grep Usage

`grep [options] template [file_name]`

### grep Options

| No. | Options | Description |
| ------ | ------ | ------ |
| 1 | -e | pattern |
| 2 | -i | Ignore uppercase vs. lowercase.  |
| 3 | -v | Invert match. |
| 4 | -c | Output count of matching lines only. |
| 5 | -l | Output matching files only.  |
| 6 | -n | Precede each matching line with a line number. |
| 7 | -h | Output matching lines without preceding them by file names. |
| 8 | -s | Suppress error messages about nonexistent or unreadable files. |
| 9 | -f file | Take regexes from a file. |
| 10 | -o | Output the matched parts of a matching line. |



- The programs must be developed in C language of C11 standard using gcc compiler.
- Do not use outdated and legacy language constructions and library functions. Pay attention to the legacy and obsolete marks in the official documentation on the language and the libraries used. Use the POSIX.1-2017 standard.
- When writing code it is necessary to follow the Google style
- The programs must be executable files with command line arguments
- The programs must be built with Makefile with appropriate targets: my_grep
- If third-party libraries are used, there must be build scripts in makefile to connect/load them
- Integration tests must cover all flag variants and input values, based on a comparison with the behavior of real Bash utilities 
- The programs must be developed according to the principles of structured programming
- Code duplication must be avoided, common modules must be reused between the utilities. Common modules can be moved to a separate folder src/common
- You can use standard and non-standard C libraries, or you can use your own developed libraries from other projects
- The statement of the message in the case of an error does not matter
- Input via stdin is not required to be supported

## Working with grep utility

You need to develop the grep utility:
- Support of the following flags: `-e`, `-i`, `-v`, `-c`, `-l`, `-n`, `-h`, `-s`, `-f`, `-o`
- Only pcre or regex libraries can be used for regular expressions
- The source, header and make files must be placed in my_grep/ directory
- The resulting executable file must be placed in the directory my_grep/ and named my_grep


Bonus assignment for extra points. You need to develop the grep utility:
- Support of all flags, including: 
- Support of all flags, including their _pair_ combinations (e.g. `-iv`, `-in`)
- Only pcre or regex libraries can be used for regular expressions

