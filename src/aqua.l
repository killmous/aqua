D       [0-9]

%{
extern void yyerror(const char *s);
#include "node.h"
#include "aqua.tab.h"

void count();
#define SAVE_TOKEN (yylval.string = new std::string(yytext, yyleng))
#define TOKEN(t) (yylval.token = t)
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
">"                     {               return TOKEN(TCGT);     }
"<"                     {               return TOKEN(TCLT);		}
"extern"                {               return EXTERN;          }
"true"                  {               return TRUE;            }
"false"                 {               return FALSE;           }
{D}+                    {   SAVE_TOKEN; return INTEGER;         }
{D}+\.{D}*              {   SAVE_TOKEN; return DOUBLE;          }
{D}*\.{D}+              {   SAVE_TOKEN; return DOUBLE;          }
[a-zA-Z_][a-zA-Z0-9]*   {   SAVE_TOKEN; return IDENTIFIER;      }
'(\\.|[^\\'])'          {   SAVE_TOKEN; return CHAR_LITERAL;    }

[ \t\n]                 /*  ignore whitespace               */
.                       {   yyerror("lex error");            }

%%

#ifndef yywrap
int yywrap() { return 1; }
#endif