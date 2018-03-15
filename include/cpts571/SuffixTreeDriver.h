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


#ifndef SUFFIX_TREE_DRIVER_H
#define SUFFIX_TREE_DRIVER_H

#include <string>
#include <fstream>
#include <iostream>

#include "cpts571/Alignment.h"
#include "cpts571/SequenceFileScanner.h"
#include "cpts571/Sequence.h"

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

struct SuffixTreeDriverConfiguration {
  std::string IFileName;
  std::string AlphabetFileName;
};

class SuffixTreeDriver {
 public:
  SuffixTreeDriver(const SuffixTreeDriverConfiguration & C)
      : sequence_()
  {
    Parse(C.IFileName, C.AlphabetFileName);
    std::cout << sequence_ << std::endl;
  }

private:
  void Parse(const std::string &IF, const std::string &AF);

  Sequence sequence_;
};

}

#endif  // SUFFIX_TREE_DRIVER_H
