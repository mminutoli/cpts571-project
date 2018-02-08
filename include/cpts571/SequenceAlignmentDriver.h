#ifndef SEQUENCE_ALIGNMENT_DRIVER_H
#define SEQUENCE_ALIGNMENT_DRIVER_H

#include <fstream>
#include <iostream>

#include "cpts571/Alignment.h"
#include "cpts571/SequenceFileScanner.h"
#include "cpts571/Sequence.h"

#include "SequenceFileParser.tab.hh"

namespace cpts571 {

class SequenceAlignmentDriver {
 public:
  SequenceAlignmentDriver(const std::string & iFileName)
      : inputFile_(iFileName.c_str())
      , currentSequence_(&sequence1_)
      , sequence1_()
      , sequence2_()
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
  std::ifstream inputFile_;

  Sequence * currentSequence_;
  Sequence sequence1_;
  Sequence sequence2_;

  ScoreTable S_;

  std::deque<Action> actions_;
};

}

#endif  // SEQUENCE_ALIGNMENT_DRIVER_H
