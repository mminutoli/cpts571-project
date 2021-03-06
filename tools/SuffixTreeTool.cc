#include "boost/program_options.hpp"

#include "cpts571/SuffixTreeDriver.h"

void ParseCmdOptions(int argc, char **argv,
                     cpts571::SuffixTreeDriverConfiguration &CFG) {
  namespace po = boost::program_options;

  po::options_description description("Options");
  description.add_options()
      ("help,h", "Print this help message")
      ("input-file,i", po::value<std::string>(&CFG.IFileName)->required(),
       "The input file with the edge-list.")
      ("alphabet-file,a", po::value<std::string>(&CFG.AlphabetFileName),
       "The file containing the alphabet")
      ("dot", po::value<std::string>(&CFG.DotOutput),
       "The file name of the Dot output file")
      ("stats", po::value<std::string>(&CFG.StatsOutput),
       "The file name of the Stats output file")
      ("bwt", po::value<std::string>(&CFG.BWTOutput),
       "The file name of the BWT output file")
      ("post", po::value<std::string>(&CFG.PostOrderOutput),
       "The file name of the Post Order visit output file")
      ("dfs", po::value<std::string>(&CFG.DFSOutput),
       "The file name of the DFS visit output file")
      ("lcs", po::bool_switch(&CFG.PrintLCS),
       "Print the LCS in the SuffixTree");
;

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
  cpts571::SuffixTreeDriverConfiguration CFG;

  ParseCmdOptions(argc, argv, CFG);

  cpts571::SuffixTreeDriver driver(CFG);
  driver.Exec();
  return 0;
}
