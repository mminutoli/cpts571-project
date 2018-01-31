%skeleton "lalr1.cc"
%language "c++"
%debug
%defines
%locations
%define api.namespace { cpts571 }
%define parser_class_name { SequenceFileParser }


%code requires {

#include <sstream>
#include <string>
#include <iostream>

namespace cpts571 {
  class SequenceFileScanner;
  class SequenceFileDriver;
}

}

%lex-param { SequenceFileScanner & scanner }
%parse-param { SequenceFileScanner & scanner }

%lex-param { SequenceFileDriver & driver }
%parse-param { SequenceFileDriver & driver }

%code {

#include "SequenceFileParser/SequenceFileDriver.h"

/* static int yylex(IA::AlternativesParser::semantic_type * yylval, */
/*                  IA::AlternativesParser::location_type *, */
/*                  IA::AlternativesScanner & scanner, */
/*                  IA::AlternativesDriver & driver); */

}

%start sequences

%union {
  std::string * sval;
  unsigned int eval;
}


%token  END 0

%%

sequences : END
