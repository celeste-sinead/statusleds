.SECONDEXPANSION:

CXX=g++
CFLAGS=-O0 -g -Wall -Wextra
INCLUDES=
LIBS=

SOURCES=main.cpp \
        cpustat.cpp

OUTPUT=statusledsd

all: $(OUTPUT)

OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

$(OBJECTS): $$(patsubst %.o,%.cpp,$$@)
	$(CXX) $(CFLAGS) $(INCLUDES) $< -c -o $@

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $(OUTPUT)

