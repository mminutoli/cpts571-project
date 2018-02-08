#include <iostream>

#include "cpts571/SequenceAlignmentDriver.h"


int main(int argc, char ** argv) {
  cpts571::SequenceAlignmentDriver driver(argv[1]);

  driver.Parse();
  driver.AlignSequences();
  driver.Print();

  return 0;
}
