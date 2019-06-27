# June 23, 2019

CFLAGS := -O2 -g
CFLAGS += -Wall -Wextra -pipe -fsanitize=address
LFLAGS := -O2 -g
LFLAGS += -fsanitize=address

TARGET := agent

SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJECTS): $(HEADERS)

clean:
	$(RM) $(TARGET) $(OBJECTS)
