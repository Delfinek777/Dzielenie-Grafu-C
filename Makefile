CC = gcc
#CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lm

# Lista wszystkich plików obiektowych
OBJS = main.o dzielenie.o pliki.o

# Nazwa wynikowego programu
TARGET = program

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Reguła dla main.o
main.o: main.c dzielenie.h pliki.h
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła dla dzielenie.o
dzielenie.o: dzielenie.c dzielenie.h
	$(CC) $(CFLAGS) -c $< -o $@

# Reguła dla pliki.o
pliki.o: pliki.c pliki.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)