#ifndef SEQUENCEFILESCANNER_H
#define SEQUENCEFILESCANNER_H

#include <istream>

#if ! defined (yyFlexLexerOnce)
#include "FlexLexer.h"
#endif

#undef YY_DECL
#define YY_DECL int cpts571::SequenceFileScanner::yylex()

namespace cpts571 {

class SequenceFileScanner : public yyFlexLexer {
public:
  SequenceFileScanner(std::istream *in);

  int yylex();

 private:
};

}

#endif /* SEQUENCEFILESCANNER_H */
