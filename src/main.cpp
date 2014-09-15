#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "codegen.h"
#include "aqua.tab.h"

extern NodeList* topLevelNodeList;
extern FILE* yyin;

bool verifyArgs(int argc, char **argv);

int main(int argc, char **argv) {
    if(!verifyArgs(argc,argv)) {
        printf("Usage incorrect\n");
        exit(-1);
    }
    // open a file handle to a particular file:
    FILE *myfile = fopen(argv[1], "r");
    // make sure it's valid:
    if (!myfile) {
        printf("Failure to open %s.\n", argv[1]);
        return -1;
    }
    // set lex to read from it instead of defaulting to STDIN:
    yyin = myfile;
    yyparse();

    CodeGenContext context;
    context.generateCode(*topLevelNodeList);
}

bool verifyArgs(int argc, char **argv) {
    if(argc < 2) {
        return false;
    }
    return true;
}