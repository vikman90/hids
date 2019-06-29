# June 23, 2019

CFLAGS := -O2 -g
CFLAGS += -Wall -Wextra -pipe
# CFLAGS += -fsanitize=address
LFLAGS := -O2 -g
# LFLAGS += -fsanitize=address
LIBS := -lyaml

TARGET := bin/agent

SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: bin $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(HEADERS)

bin:
	mkdir $@

clean:
	$(RM) $(TARGET) $(OBJECTS)
