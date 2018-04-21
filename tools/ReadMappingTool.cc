#include <iostream>

#include "boost/program_options.hpp"

#include "cpts571/ReadMappingDriver.h"

void ParseCmdOptions(int argc, char **argv,
                     cpts571::ReadMappingDriverConfiguration &CFG) {
  namespace po = boost::program_options;

  po::options_description description("Options");
  description.add_options()
      ("help,h", "Print this help message")
      ("gene-file,g", po::value<std::string>(&CFG.GeneFileName)->required(),
       "The input file with the gene.")
      ("reads-file,r", po::value<std::string>(&CFG.ReadsFileName)->required(),
       "The file containing the reads");

  po::variables_map VM;
  try {
    po::store(po::parse_command_line(argc, argv, description), VM);

    if (VM.count("help")) {
      std::cout << argv[0] << " [options]" << std::endl;
      std::cout << description << std::endl;
      exit(0);
    }

    po::notify(VM);
  } catch (po::error &e) {
    std::cerr << "Error: " << e.what() << "\n" << description << std::endl;
    exit(-1);
  }
}


int main(int argc, char **argv) {
  cpts571::ReadMappingDriverConfiguration CFG;

  ParseCmdOptions(argc, argv, CFG);

  cpts571::ReadMappingDriver driver(CFG);
  driver.Exec();
  return 0;
}
