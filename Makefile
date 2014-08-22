CC := clang++ -std=c++11
YACC := bison
LEX := flex
SRCDIR := src/
BINDIR := bin/
GENDIR := gen/

INCLUDEFLAGS := -I$(SRCDIR) -I$(GENDIR)

all:
	$(LEX) -o $(GENDIR)aqua.yy.c $(SRCDIR)aqua.l
	$(YACC) -v -o $(GENDIR)aqua.tab.c --defines=$(GENDIR)aqua.tab.h $(SRCDIR)aqua.y
	$(CC) $(INCLUDEFLAGS) -c $(GENDIR)aqua.yy.c -o $(BINDIR)aqua.yy.o
	$(CC) $(INCLUDEFLAGS) -c $(GENDIR)aqua.tab.c -o $(BINDIR)aqua.tab.o
	$(CC) $(INCLUDEFLAGS) $(SRCDIR)main.cpp $(BINDIR)aqua.yy.o $(BINDIR)aqua.tab.o -o $(BINDIR)aqua

clean:
	rm $(GENDIR)*
	rm $(BINDIR)*