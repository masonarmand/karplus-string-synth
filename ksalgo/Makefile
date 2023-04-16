CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c89 -I./include -lm
AR = ar

LIBRARY = libksalgo.a
SOURCES = src/ksalgo.c
OBJECTS = $(SOURCES:.c=.o)

all: $(LIBRARY)

$(LIBRARY): $(OBJECTS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(LIBRARY)

