CC := gcc
CFLAGS := -Wall -Wextra -std=c11

TARGET := pneuma
SRC_FILES := src/pneuma.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
