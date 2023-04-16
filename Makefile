CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c89
LDFLAGS = -lSDL2 -lSDL2main -lm
KSA_LIB_DIR = ./ksalgo
KSA_INCLUDE_DIR = ./ksalgo/include

EXECUTABLE = karplus_strong_example
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(KSA_LIB_DIR)/libksalgo.a
	$(CC) -o $@ $(OBJECTS) -L$(KSA_LIB_DIR) -lksalgo $(LDFLAGS)

$(KSA_LIB_DIR)/libksalgo.a:
	$(MAKE) -C $(KSA_LIB_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -I$(KSA_INCLUDE_DIR) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	$(MAKE) -C $(KSA_LIB_DIR) clean

