#ifndef SEQUENCEFILEDRIVER_H
#define SEQUENCEFILEDRIVER_H

#include <fstream>

#include "SequenceFileParser/SequenceFileScanner.h"
#include "SequenceFileParser.tab.hh"

namespace cpts571 {

class SequenceFileDriver {
 public:
  SequenceFileDriver(const std::string & iFileName)
      : inputFile_(iFileName.c_str()) {}

  void Parse();
 private:
  std::ifstream inputFile_;
};

}

#endif /* SEQUENCEFILEDRIVER_H */
