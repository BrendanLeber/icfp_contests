#CC := clang++
CC := g++
SRCDIR := src
BUILDDIR := build

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

#CFLAGS := -std=c++14 -stdlib=libstdc++ -pedantic -Weverything -Weffc++ -Wno-c++98-compat -g -O0
CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -g -O0 -DTRACE
#CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -O2
#CFLAGS := -std=c++14 -pedantic -Wall -Weffc++ -Wno-c++98-compat -O2 -DPROFILE -pg

LIB := -L lib
INC := -I include


all: bin/um


bin/um: $(BUILDDIR)/um.o
	@echo "Linking um..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<


checks: $(SRCDIR)/%.$(SRCEXT)
	clang-tidy $^ -config='' -- $(CFLAGS)


format: $(SRCDIR)/%.$(SRCEXT)
	clang-format -i -style=file $^


clean:
	@echo "Cleaning...";
	$(RM) -r $(BUILDDIR) bin/um


# execute: $(TARGET)
# 	@echo "Executing endo.dna..."
# 	unzip -p endo.zip endo.dna | $(TARGET) > endo.rna

# Tests
test: $(BUILDDIR)/t12-moves
	$(BUILDDIR)/t12-moves

tests: $(BUILDDIR)/t01-asnat $(BUILDDIR)/t02-pattern $(BUILDDIR)/t03-quote $(BUILDDIR)/t04-protect \
	$(BUILDDIR)/t05-consts $(BUILDDIR)/t06-templates
	$(BUILDDIR)/t01-asnat
	$(BUILDDIR)/t02-pattern
	$(BUILDDIR)/t03-quote
	$(BUILDDIR)/t04-protect
	$(BUILDDIR)/t05-consts
	$(BUILDDIR)/t06-templates

$(BUILDDIR)/t01-asnat: test/t01-asnat.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t02-pattern: test/t02-pattern.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t03-quote: test/t03-quote.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t04-protect: test/t04-protect.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t05-consts: test/t05-consts.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t06-templates: test/t06-templates.cpp $(BUILDDIR)/dna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t11-pixels: test/t11-pixels.cpp $(BUILDDIR)/rna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

$(BUILDDIR)/t12-moves: test/t12-moves.cpp $(BUILDDIR)/rna.o
	$(CC) $(CFLAGS) $(INC) -I $(SRCDIR) $(LIB) $^ -o $@

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
