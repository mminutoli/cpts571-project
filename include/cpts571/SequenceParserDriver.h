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


#ifndef SEQUENCE_PARSER_DRIVER_H
#define SEQUENCE_PARSER_DRIVER_H

#include <string>
#include <vector>

#include "cpts571/SequenceFileScanner.h"
#include "cpts571/Sequence.h"

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

class SequenceParserDriver {
 public:
  SequenceParserDriver(const std::string & inputFileName)
      : inputFileName_(inputFileName)
  {}

  std::vector<Sequence> Parse();

  void AddSequence(Sequence & s) { sequences_.emplace_back(std::move(s)); }

 private:
  std::string inputFileName_;
  std::vector<Sequence> sequences_;
};

}

#endif  // SEQUENCE_PARSER_DRIVER_H
