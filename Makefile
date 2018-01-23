TARGET	= main
CC			= gcc
CFLAGS	= -g -Wall
LDFLAGS	= -lm

main:
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) $(LDFLAGS)

clean:
	$(RM) main
