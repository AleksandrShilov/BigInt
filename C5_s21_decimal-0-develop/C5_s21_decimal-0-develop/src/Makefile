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

all: clean s21_decimal.a test gcov_report

main: main.o s21_decimal.o secondary_functions.o
	$(CC) $^ -lm -o main.out && ./main.out

main.o: main.c
	$(CC) $(CFLAGS) $^

s21_decimal.a: s21_decimal.o secondary_functions.o
	ar rc s21_decimal.a s21_decimal.o secondary_functions.o

s21_decimal.o: s21_decimal.c
	$(CC) -c $^

secondary_functions.o: secondary_functions.c
	$(CC) -c $^
 
gcov_report: test
	gcovr -r . --html --html-details -o gcov_report.html
	open gcov_report.html

test: s21_decimal_test.o test.o secondary_functions_test.o
	$(CC) $^ -o test.out $(LFLAGS) && ./test.out

s21_decimal_test.o: s21_decimal.c
	$(CC) $(CFLAGS) $(CFLAGS2) -c $^ -o $@

secondary_functions_test.o: secondary_functions.c
	$(CC) $(CFLAGS) $(CFLAGS2) -c $^ -o $@

test.o: test.c
	$(CC) $(CFLAGS2) -c $^ -o $@

check_python: main.c s21_decimal.c s21_decimal.h secondary_functions.c secondary_functions.h test.c 
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extensions=c $^

check_leaks:
	$(LEAKS)

check_cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem s21_decimal.c secondary_functions.c test.c

clean:
	$(WASH)
