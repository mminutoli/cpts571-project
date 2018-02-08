#include "boost/program_options.hpp"

#include "cpts571/SequenceAlignmentDriver.h"

void ParseCmdOptions(int argc, char **argv,
                     cpts571::SequenceAlignmentDriverConfiguration &CFG) {
  namespace po = boost::program_options;

  bool local = false;
  bool global = false;

  po::options_description description("Options");
  description.add_options()
      ("help,h", "Print this help message")
      ("input-file,i", po::value<std::string>(&CFG.IFileName)->required(),
       "The input file with the edge-list.")
      ("local,l", po::bool_switch(&local)->default_value(false),
       "Flag for local alignment")
      ("global,g", po::bool_switch(&global)->default_value(false),
       "Flag for global alignment (Default)")
      ("config-file,c", po::value<std::string>(&CFG.CFileName),
       "The JSON configuration file storing the score table.");

  po::variables_map VM;
  try {
    po::store(po::parse_command_line(argc, argv, description), VM);

    if (VM.count("help")) {
      std::cout << argv[0] << " [options]" << std::endl;
      std::cout << description << std::endl;
      exit(0);
    }

    po::notify(VM);

    if (local && global) {
      po::error E("Please select only one between local and global alignment");
      throw E;
    } else if (local) {
      CFG.Global = false;
    } else {
      CFG.Global = true;
    }
  } catch (po::error &e) {
    std::cerr << "Error: " << e.what() << "\n" << description << std::endl;
    exit(-1);
  }
}

int main(int argc, char ** argv) {
  cpts571::SequenceAlignmentDriverConfiguration CFG;

  ParseCmdOptions(argc, argv, CFG);

  cpts571::SequenceAlignmentDriver driver(CFG);

  driver.Parse();
  driver.AlignSequences();
  driver.Print();

  return 0;
}
