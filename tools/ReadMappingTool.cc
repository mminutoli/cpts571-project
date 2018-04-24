#include <iostream>

#include "boost/program_options.hpp"

#include "cpts571/ReadMappingDriver.h"

void check_range(double v) {
  if (v <= 0.0 || v > 1.0)
    throw boost::program_options::error("The value has to be in the interval ]0;1]");
}

void ParseCmdOptions(int argc, char **argv,
                     cpts571::ReadMappingDriverConfiguration &CFG) {
  namespace po = boost::program_options;

  po::options_description description("Options");
  description.add_options()
      ("help,h", "Print this help message")
      ("gene-file,g", po::value<std::string>(&CFG.GeneFileName)->required(),
       "The input file with the gene.")
      ("reads-file,r", po::value<std::string>(&CFG.ReadsFileName)->required(),
       "The file containing the reads")
      (",x", po::value<size_t>(&CFG.x)->default_value(25),
       "The x parameter")
      (",X", po::value<double>(&CFG.X)->default_value(0.9)->notifier(&check_range),
       "The X parameter ]0;1]")
      (",Y", po::value<double>(&CFG.Y)->default_value(0.8)->notifier(&check_range),
       "The Y parameter ]0;1]")
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
  } catch (po::error &e) {
    std::cerr << "Error: " << e.what() << "\n" << description << std::endl;
    exit(-1);
  }
}


int main(int argc, char **argv) {
  cpts571::ReadMappingDriverConfiguration CFG;

  ParseCmdOptions(argc, argv, CFG);

  cpts571::ReadMappingDriver driver(CFG);
  auto execBegin = std::chrono::steady_clock::now();
  driver.Exec();
  auto execEnd = std::chrono::steady_clock::now();
  double ExecTime = std::chrono::duration_cast<
    std::chrono::duration<double> >(execEnd - execBegin).count();

  std::cout << "# Total Execution Time : " << ExecTime
            << std::endl;
  return 0;
}
