SRCDIR := src
SRCEXT := cpp

format: $(SRCDIR)/*.$(SRCEXT) $(SRCDIR)/*.hpp include/*.hpp
	clang-format -i -style=file $^

help:
	$(info )
	$(info make [format] = reformat all source code)
	@printf "\n"
