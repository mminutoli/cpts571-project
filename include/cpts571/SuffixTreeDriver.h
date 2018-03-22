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

#include <chrono>
#include <string>
#include <fstream>
#include <ostream>
#include <iostream>

#include "cpts571/Alignment.h"
#include "cpts571/SequenceFileScanner.h"
#include "cpts571/Sequence.h"
#include "cpts571/SuffixTree.h"

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

struct SuffixTreeDriverConfiguration {
  std::string IFileName;
  std::string AlphabetFileName;
  std::string DotOutput;
  std::string StatsOutput;
  std::string BWTOutput;
  std::string PostOrderOutput;
};

class SuffixTreeDriver {
 public:
  SuffixTreeDriver(const SuffixTreeDriverConfiguration & C)
      : sequence_()
      , config_(C)
  {
    Parse(C.IFileName, C.AlphabetFileName);
    std::string terminal("$");
    sequence_.AppendChunk(terminal.begin(), terminal.end());
  }

  void Exec() {
    auto start = std::chrono::system_clock::now();
    SuffixTree ST(sequence_);
    auto end = std::chrono::system_clock::now();

    if (!config_.DotOutput.empty()) {
      std::ofstream DotS(config_.DotOutput);
      ST.PrintDot(DotS);
    }

    if (!config_.StatsOutput.empty()) {
      std::ofstream statsS(config_.StatsOutput);
      PrintStats(ST, statsS);
    }

    if (!config_.BWTOutput.empty()) {
      std::ofstream BWTS(config_.BWTOutput);
      PrintBWT(ST, BWTS);
    }

    if (!config_.PostOrderOutput.empty()) {
      std::ofstream PostOrderS(config_.PostOrderOutput);
      ST.PostOrderPrint(PostOrderS);
    }

    std::cout
        << "SuffixTree build in : "
        << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
        << " microseconds"
        << std::endl;
}

  void PrintBWT(SuffixTree &ST, std::ostream &OS) {
    auto BTW = ST.BTW(sequence_);
    for (auto c : BTW)
      OS << c << std::endl;
  }

  void PrintStats(SuffixTree &ST, std::ostream &OS) {
    size_t numberOfNodes = 0;
    size_t numberOfLeaves = 0;
    size_t totDepth = 0;
    size_t maxDepth = 0;
    for (auto v : ST) {
      ++numberOfNodes;
      if (v->isLeaf()) ++numberOfLeaves;
      else {
        maxDepth = std::max(maxDepth, v->StringDepth());
        totDepth += v->StringDepth();
      }
    }

    OS << "# Total number of nodes : " << numberOfNodes << "\n"
       << "# Number of leaves : " << numberOfLeaves << "\n"
       << "# Number of internal nodes : " << numberOfNodes - numberOfLeaves << "\n"
       << "# Max Depth : " << maxDepth << "\n"
       << "# Average Depth : " << double(totDepth) / (numberOfNodes - numberOfLeaves)
       << std::endl;
  }

private:
  void Parse(const std::string &IF, const std::string &AF);

  Sequence sequence_;
  SuffixTreeDriverConfiguration config_;
};

}

#endif  // SUFFIX_TREE_DRIVER_H
