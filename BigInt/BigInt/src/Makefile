CC=gcc
CFLAGS=-c -std=c11 -Wall -Werror -Wextra -lm
CFLAGS2=-fprofile-arcs -ftest-coverage
WASH=rm -rf *.o *.a *.out *html *css *gcno *gcda CPPLINT.cfg
OS=$(shell uname)
ifeq ($(OS), Linux)
	LFLAGS = -lcheck -lm -pthread -lgcov --coverage -lrt -lsubunit
	LEAKS=CK_FORK=no valgrind --leak-check=full --track-origins=yes ./test.out
else
	LFLAGS = -lcheck -lm -pthread -lgcov --coverage
	LEAKS=CK_FORK=no leaks --atExit -- ./test.out
endif

all: clean my_decimal.a test gcov_report

main: main.o my_decimal.o secondary_functions.o
	$(CC) $^ -lm -o main.out && ./main.out

main.o: main.c
	$(CC) $(CFLAGS) $^

my_decimal.a: my_decimal.o secondary_functions.o
	ar rc my_decimal.a my_decimal.o secondary_functions.o

my_decimal.o: my_decimal.c
	$(CC) -c $^

secondary_functions.o: secondary_functions.c
	$(CC) -c $^
 
gcov_report: test
	gcovr -r . --html --html-details -o gcov_report.html
	open gcov_report.html

test: my_decimal_test.o test.o secondary_functions_test.o
	$(CC) $^ -o test.out $(LFLAGS) && ./test.out

my_decimal_test.o: my_decimal.c
	$(CC) $(CFLAGS) $(CFLAGS2) -c $^ -o $@

secondary_functions_test.o: secondary_functions.c
	$(CC) $(CFLAGS) $(CFLAGS2) -c $^ -o $@

test.o: test.c
	$(CC) $(CFLAGS2) -c $^ -o $@

check_python: main.c my_decimal.c my_decimal.h secondary_functions.c secondary_functions.h test.c 
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extensions=c $^

check_leaks:
	$(LEAKS)

check_cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem my_decimal.c secondary_functions.c test.c

clean:
	$(WASH)
