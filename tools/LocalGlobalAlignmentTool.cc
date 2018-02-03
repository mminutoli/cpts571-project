#include <iostream>
#include "SequenceFileParser/SequenceFileDriver.h"


int main(int argc, char ** argv) {
  cpts571::SequenceFileDriver driver(argv[1]);

  std::cout << "File loading : " << argv[1] << std::endl;
  driver.Parse();
  driver.Print();
  return 0;
}
