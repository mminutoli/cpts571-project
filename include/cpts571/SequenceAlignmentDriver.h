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


#ifndef SEQUENCE_ALIGNMENT_DRIVER_H
#define SEQUENCE_ALIGNMENT_DRIVER_H

#include <string>
#include <fstream>
#include <iostream>

#include "cpts571/Alignment.h"
#include "cpts571/SequenceFileScanner.h"
#include "cpts571/Sequence.h"

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

struct SequenceAlignmentDriverConfiguration {
  std::string IFileName;
  bool Global;
  std::string CFileName;
};

class SequenceAlignmentDriver {
 public:
  SequenceAlignmentDriver(const SequenceAlignmentDriverConfiguration & C)
      : inputFileName_(C.IFileName)
      , configFileName_(C.CFileName)
      , currentSequence_(&sequence1_)
      , sequence1_()
      , sequence2_()
      , S_{ 1, -2, -5, -2 }
      , isGlobal_(C.Global)
      , actions_()
  {}

  void SequenceName(const std::string & n) {
    currentSequence_->Name(n);
  }

  template <typename Iterator>
  void AppendChunk(const Iterator B, const Iterator E) {
    currentSequence_->AppendChunk(B, E);
  }
 
  void SwitchToLoadSecondSequence() { currentSequence_ = &sequence2_; }

  void AlignSequences();

  void Parse();

  void Print();

private:
  std::string inputFileName_;
  std::string configFileName_;

  Sequence * currentSequence_;
  Sequence sequence1_;
  Sequence sequence2_;

  ScoreTable S_;
  bool isGlobal_;
  std::deque<Action> actions_;
};

}

#endif  // SEQUENCE_ALIGNMENT_DRIVER_H
