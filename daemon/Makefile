.SECONDEXPANSION:

CXX=g++
CFLAGS=-O0 -g -Wall -Wextra
INCLUDES=
LIBS=

SOURCES=main.c

OUTPUT=statusledsd

all: $(OUTPUT)

OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

$(OBJECTS): $$(patsubst %.o,%.c,$$@)
	$(CXX) $(CFLAGS) $(INCLUDES) $< -c -o $@

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CFLAGS) $(LIBS) $^ -o $(OUTPUT)

