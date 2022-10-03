CC = clang

ifdef DEBUG
DFLAG = -DDEBUG
CFLAGS = -std=c99 -g -Wall -fPIC
else
DFLAG = 
CFLAGS = -std=c99 -fPIC
endif

IFLAGS = -I./inc

LFLAGS = -L./

SRCS = $(wildcard ./src/*.c)

TEST_SRCS = $(wildcard ./test/*.c)

OBJ = $(SRCS:.c=.o)

TEST_OBJ = $(TEST_SRCS:.c=.o)

.PHONY: all clean checkdyn checkstatic

all: libtaipower.so libtaipower.a checkdyn checkstatic

libtaipower.so: $(OBJ)
	$(CC) $(CFLAGS) $(IFLAGS) $(DFLAG) -shared $^ -o $@

libtaipower.a: $(OBJ)
	ar rcs $@ $^

checkdyn: $(TEST_OBJ) libtaipower.so
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) $(DFLAG) -ltaipower -o $@ $^ && LD_LIBRARY_PATH=./ ./checkdyn

checkstatic: $(TEST_OBJ) libtaipower.a
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) $(DFLAG) -o $@ $^ && ./checkstatic

.c.o:
	$(CC) $(CFLAGS) $(IFLAGS) $(DFLAG) -c $< -o $@

clean:
	rm -f check* $(OBJ) $(TEST_OBJ) libtaipower.so libtaipower.a
