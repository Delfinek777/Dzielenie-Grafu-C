CC = gcc
#CFLAGS = -Wall -Wextra -O2

OBJS = main.o dzielenie.o

program: $(OBJS)
	$(CC) $(CFLAGS) -o program $(OBJS)

main.o: main.c dzielenie.h
	$(CC) $(CFLAGS) -c main.c

dzielenie.o: dzielenie.c dzielenie.h
	$(CC) $(CFLAGS) -c dzielenie.c

clean:
	rm -f $(OBJS) program
