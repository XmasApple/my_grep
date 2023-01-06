CC=gcc
FLAGS = -Wall -Wextra -Werror -std=c11

all: my_grep

my_grep:
	$(CC) $(FLAGS) -o ./my_grep ./*.c

test: rebuild
	./test_func_grep.sh

leak: rebuild
	./test_leak_grep.sh
	
valgrind: rebuild
	./test_valgrind_grep.sh

check: rebuild
	python3 ../../cpplint.py --extensions=c,h ./*.c ./*.h
	clang-format -i *.c 
	clang-format -i *.h

clean: clean_grep

clean_grep:
	rm -f ./*.o ./*.a ./*.so ./my_grep

rebuild: clean all
