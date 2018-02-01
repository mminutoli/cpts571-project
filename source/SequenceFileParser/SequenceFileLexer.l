%{

#include "SequenceFileParser/SequenceFileScanner.h"

using token = cpts571::SequenceFileParser::token;

#define TOKEN(x) std::cout << ""#x << " " << yytext << std::endl; yylval->sval = yytext; return x

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
[a-zA-Z:0-9]+    TOKEN(token::WORD);
.                { std::cout << "here" << std::endl; }
