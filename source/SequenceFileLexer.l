%{
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

[ \r\t\f\v]+   { /* Ignore these white spaces */ }
">"              TOKEN(token::START_SEQUENCE);
[a-zA-Z0-9:_.-]+ TOKEN(token::WORD);
[\n]+            TOKEN(token::NL);
.                { }
