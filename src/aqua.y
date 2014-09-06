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
    TypeSignature* typesig;
    ExpressionList* exprlist;
    std::string* string;
    int token;
}

%token <string> IDENTIFIER INTEGER DOUBLE CHAR_LITERAL
%token <token> ARROW FATARROW EXTERN

%type <block>   program declaration_list
%type <stmt>    declaration
%type <expr>    numeric expr
%type <id>      ident type
%type <typesig> type_signature
%type <exprlist> args

%start program

%%

ident
  : IDENTIFIER
    { $$ = new NIdentifier(*$1); delete $1; }
  ;

numeric
  : INTEGER
    { $$ = new NInteger(atol($1->c_str())); delete $1; }
  | DOUBLE
    { $$ = new NDouble(atof($1->c_str())); delete $1; }
  ;

expr
  : ident
    { $$ = $<expr>1; }
  | numeric
  | CHAR_LITERAL
    { $$ = new NChar($1->c_str()[1] == '\\' ? $1->c_str()[2] : $1->c_str()[1]); }
  | ident '(' args ')'
    { $$ = new NMethodCall($1, *$3); }
  ;

args
  : /* blank */
    { $$ = new ExpressionList(); }
  | expr
    { $$ = new ExpressionList(); $$->push_back($1); }
  | args ',' expr
    { $1->push_back($3); $$ = $1; }
  ;

type
  : ident
  | '[' ident ']'
    { $2->name = "[" + $2->name + "]"; $$ = $2; }
  ;

type_signature
  : type
    { $$ = new TypeSignature(); $$->push_back($1); }
  | type_signature ARROW type
    { $1->push_back($3); $$ = $1; }
  ;

declaration
  : ident ':' type '=' expr ';'
    { $$ = new NVariableDeclaration($1, $3, $5); }
  | expr ';'
    { $$ = new NExpressionStatement($1); }
  | EXTERN ident ':' type_signature ';'
    { $$ = new NFunctionDefinition($2, *$4); }
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