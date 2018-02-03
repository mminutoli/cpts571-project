#ifndef SEQUENCEFILEDRIVER_H
#define SEQUENCEFILEDRIVER_H

#include <fstream>
#include <iostream>

#include "SequenceFileParser/SequenceFileScanner.h"
#include "SequenceFileParser.tab.hh"
#include "Sequence.h"

namespace cpts571 {

class SequenceFileDriver {
 public:
  SequenceFileDriver(const std::string & iFileName)
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
 
  void StartSecondSequence() { currentSequence_ = &sequence2_; }

  void Parse();

  void Print() {
    std::cout << sequence1_ << " "
              << sequence2_ << std::endl;
  }

 private:
  std::ifstream inputFile_;
  Sequence * currentSequence_;
  Sequence sequence1_;
  Sequence sequence2_;
};

}

#endif /* SEQUENCEFILEDRIVER_H */
