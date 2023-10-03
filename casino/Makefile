
CC = g++-7
SRC_DIR = src/
INCLUDE_DIR = include/

OUTFILE = casino

COMPILE_FLAGS = -lncurses -std=c++11 -Wall -Wextra -O2 -pedantic

# -------------------------------------------------

all: bin compile build

run: all
	./$(OUTFILE)

bin:
	mkdir bin

compile:
	@rm bin/*.o; \
	cd $(SRC_DIR); \
	$(CC) -c *.cpp -I../include; \
	mv *.o ../bin; \
	cd -;

build:
	$(CC) bin/*.o $(COMPILE_FLAGS) -o $(OUTFILE)

clean:
	rm -rf bin

.PHONY: all compile build clean
