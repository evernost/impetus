CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

TARGET = solver

all: $(TARGET)

$(TARGET): solver.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
