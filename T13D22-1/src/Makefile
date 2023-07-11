CC = gcc
CFLAGS = -Wall
LIBS = -lcrypto
BUILD_DIR = ../build/
COMMON_SOURCES = logger.c
COMMON_OBJECTS = $(COMMON_SOURCES:.c=.o)

cipher: CFLAGS += -DNO_LOGGING
cipher: $(COMMON_OBJECTS) cipher.o
	$(CC) $(CFLAGS) $(COMMON_OBJECTS) cipher.o -o $(BUILD_DIR)cipher $(LIBS)
	$(BUILD_DIR)cipher
logging_cipher:	$(COMMON_OBJECTS) cipher.o
	$(CC) $(CFLAGS) $(COMMON_OBJECTS) cipher.o -o $(BUILD_DIR)logging_cipher $(LIBS)
	$(BUILD_DIR)logging_cipher
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: logging_cipher

clean:
	rm -f $(COMMON_OBJECTS) cipher.o $(BUILD_DIR)cipher $(BUILD_DIR)logging_cipher