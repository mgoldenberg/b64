CC = gcc
CFLAGS = -g -Wall
INCLUDE = include/
TARGET = b64

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -I $(INCLUDE) -o $(TARGET) main.c lib/$(TARGET).c

clean:
	$(RM) $(TARGET)
