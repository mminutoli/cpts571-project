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

#include <fstream>
#include <cassert>

#include "cpts571/SequenceParserDriver.h"
#include "cpts571/SuffixTreeDriver.h"

namespace cpts571 {

void
SuffixTreeDriver::Parse(const std::string &IF, const std::string &AF) {
  SequenceParserDriver SPD(IF);

  auto sequences = std::move(SPD.Parse());
  assert(sequences.size() > 0);
  sequence_ = std::move(sequences[0]);

  std::ifstream alphabetFile(AF.c_str());

  if (!alphabetFile.good()) return;
}

}
