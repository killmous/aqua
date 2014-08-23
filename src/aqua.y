%{
#include <stdio.h>
#include "node.h"

NBlock* programBlock;

extern int yylex();
extern void yyerror(const char* s);

%}

%union {
    NBlock* block;
    NStatement* stmt;
    NExpression* expr;
    NIdentifier* id;
    std::string* string;
    int token;
}

%token <string> IDENTIFIER INTEGER
%token <token> ARROW FATARROW

%type <block>   program declaration_list
%type <stmt>    declaration
%type <expr>    numeric expr
%type <id>      ident

%start program

%%

ident
  : IDENTIFIER
    { $$ = new NIdentifier(*$1); delete $1; }
  ;

numeric
  : INTEGER
    { $$ = new NInteger(atol($1->c_str())); delete $1; }
  ;

expr
  : ident
    { $$ = $<expr>1; }
  | numeric
  ;

type_signature
  : type
  | type_signature ARROW type
  ;

type
  : ident
  | '[' ident ']'
  ;

declaration
  : ident ':' type_signature '=' expr ';'
    { $$ = new NDeclaration($1, $5); }
  ;

declaration_list
  : declaration
    { $$ = new NBlock(); $$->addStatement($1); }
  | declaration_list declaration
    { $1->addStatement($2); }
  ;

program
  : declaration_list
    { programBlock = $1; }
  ;


%%

#include <stdio.h>

extern int yylineno;

void yyerror(const char *s)
{
    fflush(stdout);
    printf("%s in line %d\n", s, yylineno);
}