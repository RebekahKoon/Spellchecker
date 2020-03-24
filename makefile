CC = gcc
CFLAGS = -g -Wall -std=c99

all : tests spellChecker

tests : tests.o hashMap.o CuTest.o
	$(CC) $(CFLAGS) -o $@ $^

spellChecker : spellChecker.o hashMap.o
	$(CC) $(CFLAGS) -o $@ $^

tests.o : tests.c CuTest.h hashMap.h

hashMap.o : hashMap.h hashMap.c

CuTest.o : CuTest.h CuTest.c

spellChecker.o : spellChecker.c hashMap.h

memCheckTests :
	valgrind --tool=memcheck --leak-check=yes tests

memCheckSpellChecker :
	valgrind --tool=memcheck --leak-check=yes spellChecker

clean :
	-rm *.o
	-rm tests
	-rm spellChecker
