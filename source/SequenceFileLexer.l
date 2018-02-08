%{

#include "cpts571/SequenceFileScanner.h"

using token = cpts571::SequenceFileParser::token;

#define TOKEN(x) do { yylval->sval = new std::string(yytext); return x; } while (0)

#define yyterminate() return(token::END)

%}

%option debug
%option nodefault
%option yyclass="SequenceFileScanner"
%option noyywrap
%option c++

%%

[ \r\t\f\v\n]+   { /* Ignore these white spaces */ }
">"              TOKEN(token::START_SEQUENCE);
[acgtACGT]+      TOKEN(token::SEQUENCE_BIT);
[a-zA-Z0-9:_.-]+ TOKEN(token::WORD);
.                { }
