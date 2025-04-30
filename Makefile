CC = gcc
#CFLAGS = -Wall -Wextra -std=c11 -O2 -g
LDFLAGS = -lm

# Lista wszystkich plików obiektowych do głównego programu
OBJS = main.o dzielenie.o pliki.o
TARGET = program

.PHONY: all clean test

# Budowanie głównego programu
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c dzielenie.h pliki.h
	$(CC) $(CFLAGS) -c $< -o $@

dzielenie.o: dzielenie.c dzielenie.h
	$(CC) $(CFLAGS) -c $< -o $@

pliki.o: pliki.c pliki.h
	$(CC) $(CFLAGS) -c $< -o $@

# Budowanie testów
test: test.o dzielenie.o pliki.o
	$(CC) $(CFLAGS) -o test test.o dzielenie.o pliki.o $(LDFLAGS)

# Czyszczenie
clean:
	del /Q *.o $(TARGET) test wynik.bin wynik.txt 2>nul || exit 0
