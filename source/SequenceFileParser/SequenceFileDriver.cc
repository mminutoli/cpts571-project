#include <fstream>
#include <iostream>

#include "SequenceFileParser/SequenceFileDriver.h"


namespace cpts571 {

void
SequenceFileDriver::Parse() {
  if (!inputFile_.good()) {
    std::cerr << "Error opening the file" << std::endl;
    exit(-1);
  }

  SequenceFileScanner scanner(&inputFile_);
  SequenceFileParser parser(scanner, *(this));

  std::cout << "Ready to parse" << std::endl;

  if (parser.parse() == -1)
    std::cerr << "Parsing failed" << std::endl;
}

}
