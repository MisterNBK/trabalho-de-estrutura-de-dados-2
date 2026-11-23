CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined
CPPFLAGS = -Iinclude

TARGET = bin/estacionamento
SOURCES = apps/app.c src/avl.c src/logica.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/avl.h include/logica.h
	mkdir -p bin
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
