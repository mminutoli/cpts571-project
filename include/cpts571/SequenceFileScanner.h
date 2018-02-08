//===------------------------------------------------------------*- C++ -*-===//
//
// Copyright 2017 Marco Minutoli <mminutoli@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
//===----------------------------------------------------------------------===//


#ifndef SEQUENCEFILESCANNER_H
#define SEQUENCEFILESCANNER_H

#include <istream>

#if ! defined (yyFlexLexerOnce)
#include "FlexLexer.h"
#endif

#undef YY_DECL
#define YY_DECL int cpts571::SequenceFileScanner::yylex()

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

class SequenceFileScanner : public yyFlexLexer {
public:
  SequenceFileScanner(std::istream *in);

  int yylex(SequenceFileParser::semantic_type *lval);
  int yylex();

 private:
  SequenceFileParser::semantic_type * yylval;
};

}

#endif /* SEQUENCEFILESCANNER_H */
