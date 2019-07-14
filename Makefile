# June 23, 2019

CFLAGS := -O2 -g -std=c99 -Iinclude
CFLAGS += -Wall -Wextra -pipe
# CFLAGS += -fsanitize=address
LFLAGS := -O2 -g
# LFLAGS += -fsanitize=address
LIBS := -lyaml -lssl -lcrypto -lcjson

AGENT = bin/agent
MANAGER = bin/manager

TARGET := $(AGENT) $(MANAGER)

HEADERS = $(wildcard include/*.h)

AGENT_SOURCES = $(wildcard src/agent/*.c)
AGENT_OBJECTS = $(AGENT_SOURCES:.c=.o)

FIM_SOURCES = $(wildcard src/fim/*.c)
FIM_OBJECTS = $(FIM_SOURCES:.c=.o)

LOGCOLLECTOR_SOURCES = $(wildcard src/logcollector/*.c)
LOGCOLLECTOR_OBJECTS = $(LOGCOLLECTOR_SOURCES:.c=.o)

MANAGER_SOURCES = $(wildcard src/manager/*.c)
MANAGER_OBJECTS = $(MANAGER_SOURCES:.c=.o)

SHARED_SOURCES = $(wildcard src/shared/*.c)
SHARED_OBJECTS = $(SHARED_SOURCES:.c=.o)

OBJECTS = $(AGENT_OBJECTS) $(FIM_OBJECTS) $(LOGCOLLECTOR_OBJECTS) $(MANAGER_OBJECTS) $(SHARED_OBJECTS)

.PHONY: all clean

all: bin $(TARGET)

$(AGENT): $(AGENT_OBJECTS) $(FIM_OBJECTS) $(LOGCOLLECTOR_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

$(MANAGER): $(MANAGER_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(LFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(HEADERS)

bin:
	mkdir $@

clean:
	$(RM) $(TARGET) $(OBJECTS)
