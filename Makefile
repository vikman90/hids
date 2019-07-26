# June 23, 2019

CXXFLAGS := -O2 -g -std=c++11
CXXFLAGS += -Wall -Wextra -pipe
# CXXFLAGS += -fsanitize=address
LFLAGS := -O2 -g
# LFLAGS += -fsanitize=address
LIBS := -lyaml -lcrypto -lcjson

TARGET := bin/agent

AGENT_SOURCES = $(wildcard src/agent/*.cpp)
AGENT_HEADERS = $(wildcard src/agent/*.h)
AGENT_OBJECTS = $(AGENT_SOURCES:.cpp=.o)

SHARED_SOURCES = $(wildcard src/shared/*.cpp)
SHARED_HEADERS = $(wildcard src/shared/*.h)
SHARED_OBJECTS = $(SHARED_SOURCES:.cpp=.o)

HEADERS = $(AGENT_HEADERS) $(SHARED_HEADERS)
OBJECTS = $(AGENT_OBJECTS) $(SHARED_OBJECTS)

.PHONY: all clean

all: bin $(TARGET)

bin/agent: $(AGENT_OBJECTS) $(SHARED_OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(HEADERS)

bin:
	mkdir $@

clean:
	$(RM) $(TARGET) $(OBJECTS)
