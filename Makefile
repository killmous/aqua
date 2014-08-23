CC := g++ -std=c++11
YACC := bison
LEX := flex

SRCDIR := src/
BINDIR := bin/
GENDIR := gen/

CFLAGS := `llvm-config --cxxflags`
CFLAGS += -I$(SRCDIR) -I$(GENDIR)
LDFLAGS := `llvm-config --cxxflags --ldflags --libs`

all:
	$(LEX) -o $(GENDIR)aqua.yy.c $(SRCDIR)aqua.l
	$(YACC) -v -o $(GENDIR)aqua.tab.c --defines=$(GENDIR)aqua.tab.h $(SRCDIR)aqua.y
	$(CC) $(CFLAGS) -c $(GENDIR)aqua.yy.c -o $(BINDIR)aqua.yy.o
	$(CC) $(CFLAGS) -c $(GENDIR)aqua.tab.c -o $(BINDIR)aqua.tab.o
	$(CC) $(CFLAGS) -c $(SRCDIR)main.cpp -o $(BINDIR)main.o
	$(CC) $(CFLAGS) -c $(SRCDIR)codegen.cpp -o $(BINDIR)codegen.o
	$(CC) $(BINDIR)main.o $(BINDIR)codegen.o $(BINDIR)aqua.yy.o $(BINDIR)aqua.tab.o $(LDFLAGS) -o $(BINDIR)aqua

clean:
	rm $(GENDIR)*
	rm $(BINDIR)*