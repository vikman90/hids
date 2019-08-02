# June 23, 2019

CXXFLAGS := -O2 -g -std=c++11 -Iinclude
CXXFLAGS += -Wall -Wextra -pipe
# CXXFLAGS += -fsanitize=address
LFLAGS := -O2 -g
# LFLAGS += -fsanitize=address
LIBS := -lyaml -lcrypto -lcjson

TARGET := bin/agent

AGENT_SOURCES = $(wildcard src/agent/*.cpp)
AGENT_HEADERS = $(wildcard src/agent/*.h)
AGENT_OBJECTS = $(AGENT_SOURCES:.cpp=.o)

LOGCOLLECTOR_SOURCES = $(wildcard src/logcollector/*.cpp)
LOGCOLLECTOR_HEADERS = $(wildcard src/logcollector/*.h)
LOGCOLLECTOR_OBJECTS = $(LOGCOLLECTOR_SOURCES:.cpp=.o)

SHARED_SOURCES = $(wildcard src/shared/*.cpp)
SHARED_HEADERS = $(wildcard include/*.h)
SHARED_OBJECTS = $(SHARED_SOURCES:.cpp=.o)

OBJECTS = $(AGENT_OBJECTS) $(LOGCOLLECTOR_OBJECTS) $(SHARED_OBJECTS)

.PHONY: all clean

all: bin $(TARGET)

bin/agent: $(AGENT_OBJECTS) $(LOGCOLLECTOR_OBJECTS) $(SHARED_OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

$(OBJECTS): $(SHARED_HEADERS)
$(AGENT_OBJECTS): $(AGENT_HEADERS)
$(LOGCOLLECTOR_OBJECTS): $(LOGCOLLECTOR_HEADERS)

bin:
	mkdir $@

clean:
	$(RM) $(TARGET) $(OBJECTS)
