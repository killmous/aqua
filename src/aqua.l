D       [0-9]

%{
extern void yyerror(const char *s);
#include "node.h"
#include "aqua.tab.h"

void count();
#define SAVE_TOKEN (yylval.string = new std::string(yytext, yyleng))
%}

%option noyywrap
%option yylineno

%%

"->"                    {               return ARROW;           }
"=>"                    {               return FATARROW;        }
":"                     {               return ':';             }
";"                     {               return ';';             }
","                     {               return ',';             }
"="                     {               return '=';             }
"("                     {               return '(';             }
")"                     {               return ')';             }
"["                     {               return '[';             }
"]"                     {               return ']';             }
"{"                     {               return '{';             }
"}"                     {               return '}';             }
{D}+                    {   SAVE_TOKEN; return INTEGER;         }
{D}+\.{D}*              {   SAVE_TOKEN; return DOUBLE;          }
{D}*\.{D}+              {   SAVE_TOKEN; return DOUBLE;          }
[a-zA-Z_][a-zA-Z0-9]*   {   SAVE_TOKEN; return IDENTIFIER;      }

[ \t\n]                 /*  ignore whitespace               */
.                       {   yyerror("lex error");            }

%%

#ifndef yywrap
int yywrap() { return 1; }
#endif