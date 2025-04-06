CC = gcc
#CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lm

OBJS = main.o dzielenie.o

program: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c dzielenie.h
	$(CC) $(CFLAGS) -c $<

dzielenie.o: dzielenie.c dzielenie.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) program


