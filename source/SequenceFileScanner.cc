#include "cpts571/SequenceFileScanner.h"

namespace cpts571 {

SequenceFileScanner::SequenceFileScanner(std::istream *in)
    : yyFlexLexer(in), yylval(nullptr) {}

int
SequenceFileScanner::yylex(SequenceFileParser::semantic_type *lval) {
  yylval = lval;
  return yylex();
}

}
