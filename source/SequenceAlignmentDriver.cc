#include <iostream>

#include "cpts571/SequenceAlignmentDriver.h"
#include "cpts571/Alignment.h"
#include "cpts571/Sequence.h"


namespace cpts571 {

void
SequenceAlignmentDriver::Parse() {
  if (!inputFile_.good()) {
    std::cerr << "Error opening the file" << std::endl;
    exit(-1);
  }

  SequenceFileScanner scanner(&inputFile_);
  SequenceFileParser parser(scanner, *(this));

  if (parser.parse() == -1)
    std::cerr << "Parsing failed" << std::endl;
}

void
SequenceAlignmentDriver::AlignSequences() {
  S_ = { 1, -2, -5, -2 };

  actions_ = Alignment(sequence1_, sequence2_, S_, global_alignment_tag());

}

static void
PrintAlignment(const Sequence & s1, const Sequence & s2,
               std::deque<Action> & actions) {
  std::cout
      << "s1 = \"" << s1.Name() << "\", length = " << s1.length() << " characters"
      << std::endl
      << "s2 = \"" << s2.Name() << "\", length = " << s2.length() << " characters"
      << "\n"
      << std::endl;

  size_t i = 0;
  size_t j = 0;
  size_t printed = 0;

  while (printed < actions.size()) {
    size_t currentLine = 0;
    std::cout << "s1 " << std::setw(10) << i + 1 << ' ';
    for (auto itr = actions.rbegin() + printed, end = actions.rend();
         itr != end && currentLine < 60; ++itr , ++currentLine) {
      switch(*itr) {
        case Action::Match:
        case Action::Mismatch:
        case Action::Deletion:
          std::cout << s1[i++];
          break;
        case Action::Insertion:
          std::cout << '-';
      }
    }
    std::cout << ' ' << std::setw(10) << i << std::endl;
    std::cout << "              ";
    currentLine = 0;
    for (auto itr = actions.rbegin() + printed, end = actions.rend();
         itr != end&& currentLine < 60; ++itr, ++currentLine) {
      switch(*itr) {
        case Action::Match:
          std::cout << '|';
          break;
        case Action::Mismatch:
        case Action::Insertion:
        case Action::Deletion:
          std::cout << ' ';
      }
    }
    std::cout << std::endl;
    std::cout << "s2 " << std::setw(10) << j + 1 << ' ';
    currentLine = 0;
    for (auto itr = actions.rbegin() + printed, end = actions.rend();
         itr != end && currentLine < 60; ++itr, ++currentLine) {
      switch(*itr) {
        case Action::Match:
        case Action::Mismatch:
        case Action::Insertion:
          std::cout << s2[j++];
          break;
        case Action::Deletion:
          std::cout << '-';
      }
    }
    std::cout << ' ' << std::setw(10) << j << '\n' << std::endl;

    printed += currentLine;
  }
}

static void
PrintScoreTable(ScoreTable & S) {
  std::cout << "Scores:\t\t"
            << "match = " << S.Match
            << ", mismatch = " << S.Mismatch
            << ", h = " << S.H
            << ", g = " << S.G << '\n'
            << std::endl;      
}

static void
PrintReport(std::deque<Action> & actions, ScoreTable & S) {
  size_t openingGaps = 0;
  size_t gaps = 0;
  size_t matches = 0;
  size_t mismatches = 0;

  bool inDeletionGap  = false;
  bool inInsertionGap = false;

  for (auto action : actions) {
    switch (action) {
      case Action::Match:
        ++matches;
        if (inInsertionGap) inInsertionGap = false;
        if (inDeletionGap) inDeletionGap = false;
        break;
      case Action::Mismatch:
        ++mismatches;
        if (inInsertionGap) inInsertionGap = false;
        if (inDeletionGap) inDeletionGap = false;
        break;
      case Action::Insertion:
        if (!inInsertionGap) {
          inInsertionGap = true;
          ++openingGaps;
        }
        ++gaps;
        if (inDeletionGap) inDeletionGap = false;
        break;
      case Action::Deletion:
        if (!inDeletionGap) {
          inDeletionGap = true;
          ++openingGaps;
        }
        ++gaps;
        if (inInsertionGap) inInsertionGap = false;
        break;
    }
  }

  ssize_t globalScore = S.Match * matches +
                        S.Mismatch * mismatches +
                        S.H * openingGaps +
                        S.G * gaps;

  std::cout
      << "\nReport:\n"
         "\nGlobal optimal score = " << globalScore
      << "\n\nNumber of: matches = " << matches
      << ", mismatches = " << mismatches
      << ", gaps = " << gaps
      << ", opening gaps = " << openingGaps
      << "\n\nIdentities = " << matches << "/" << actions.size()
      << " (" << 100 * matches / actions.size() << "%)"
      << ", Gaps = " << gaps << "/" << actions.size()
      << " (" << 100 * gaps / actions.size() << "%)"
      << std::endl;
}


void
SequenceAlignmentDriver::Print() {
  PrintScoreTable(S_);
  PrintAlignment(sequence1_, sequence2_, actions_);
  PrintReport(actions_, S_);
}

}
