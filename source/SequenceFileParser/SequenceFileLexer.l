%{
#include <SequenceFileParser/SequenceFileScanner.h>

// using token = cpts571::SequenceFileParser::token;

#define TOKEN(x) yylval->sval = new std::string(yytext); return x

// #define yyterminate() return(token::END)

%}

%option debug
%option nodefault
%option yyclass="SequenceFileScanner"
%option noyywrap
%option c++

%%

[ \r\t\f\v ]+ { /* Ignore these white spaces */ }
">"           TOKEN(token::START_SEQUENCE);
[ acgtACGT ]+ TOKEN(token::SEGMENT);
.             {}
