#CC := clang++
CC := g++
SRCDIR := src
BUILDDIR := build

TARGET := build/execute

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

#CFLAGS := -std=c++14 -stdlib=libstdc++ -pedantic -Weverything -Weffc++ -Wno-c++98-compat -g -O0
#CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -g -O0
CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -O2 -DPROFILE -pg

#LIB :=

#INC := -I include


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
test:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o $(BUILDDIR)/tester

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
