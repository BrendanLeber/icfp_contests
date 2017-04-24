#CC := clang++
CC := g++
SRCDIR := src
BUILDDIR := build

TARGET := bin/execute

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

#CFLAGS := -std=c++14 -stdlib=libstdc++ -pedantic -Weverything -Weffc++ -Wno-c++98-compat -g -O0
CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -g -O0 -DTEST_PATTERN
#CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -O2 -DPROFILE -pg

LIB := -L lib
INC := -I include


$(TARGET): $(OBJECTS)
	@echo "Linking..."
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIB)


$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<


checks: $(SRCDIR)/%.$(SRCEXT)
	clang-tidy $^ -config='' -- $(CFLAGS)


format: $(SRCDIR)/%.$(SRCEXT)
	clang-format -i -style=file $^


clean:
	@echo "Cleaning...";
	$(RM) -r $(BUILDDIR) $(TARGET)


execute: $(TARGET)
	@echo "Executing endo.dna..."
	unzip -p endo.zip endo.dna | $(TARGET) > endo.rna

# Tests
tests: $(BUILDDIR)/t01-asnat $(BUILDDIR)/t02-pattern
	$(BUILDDIR)/t01-asnat
	$(BUILDDIR)/t02-pattern

$(BUILDDIR)/t01-asnat: $(BUILDDIR)/execute_functions.o
	$(CC) $(CFLAGS) test/t01-asnat.cpp $(INC) -I $(SRCDIR) $(LIB) $^ -o $(BUILDDIR)/t01-asnat

$(BUILDDIR)/t02-pattern: $(BUILDDIR)/execute_functions.o
	$(CC) $(CFLAGS) test/t02-pattern.cpp $(INC) -I $(SRCDIR) $(LIB) $^ -o $(BUILDDIR)/t02-pattern

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
