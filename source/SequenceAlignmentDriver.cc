//===------------------------------------------------------------*- C++ -*-===//
//
// Copyright 2017 Marco Minutoli <mminutoli@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <iostream>
#include <fstream>

#include "cpts571/SequenceAlignmentDriver.h"
#include "cpts571/SequenceParserDriver.h"
#include "cpts571/Alignment.h"
#include "cpts571/Sequence.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/istreamwrapper.h"

namespace cpts571 {

void
SequenceAlignmentDriver::Parse(const std::string &IF, const std::string &CF) {
  SequenceParserDriver SPD(IF);

  sequences_ = std::move(SPD.Parse());

  std::ifstream configFile(CF.c_str());

  if (!configFile.good()) return;

  rapidjson::IStreamWrapper ISW(configFile);
  rapidjson::Document document;
  document.ParseStream(ISW);

  S_.Match = document["match"].GetInt64();
  S_.Mismatch = document["mismatch"].GetInt64();
  S_.H = document["h"].GetInt64();
  S_.G = document["g"].GetInt64();
}

void
SequenceAlignmentDriver::AlignSequences() {
  assert(sequences_.size() == 2);

  if (isGlobal_) {
    actions_ = Alignment(sequences_[0], sequences_[1], S_, global_alignment_tag());
  } else {
    actions_ = Alignment(sequences_[0], sequences_[1], S_, local_alignment_tag());
  }
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
        case Action::DC_MatchMismatch:
        case Action::Match:
        case Action::Mismatch:
        case Action::DC_Deletion:
        case Action::Deletion:
          std::cout << s1[i++];
          break;
        case Action::DC_Insertion:
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
        case Action::DC_MatchMismatch:
        case Action::DC_Insertion:
        case Action::DC_Deletion:
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
        case Action::DC_MatchMismatch:
        case Action::Insertion:
        case Action::DC_Insertion:
          std::cout << s2[j++];
          break;
        case Action::Deletion:
        case Action::DC_Deletion:
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
      default:
        break;
    }
  }

  ssize_t globalScore = S.Match * matches +
                        S.Mismatch * mismatches +
                        S.H * openingGaps +
                        S.G * gaps;

  size_t lengthOfAlignment = matches + mismatches + gaps;
  std::cout
      << "\nReport:\n"
         "\nGlobal optimal score = " << globalScore
      << "\n\nNumber of: matches = " << matches
      << ", mismatches = " << mismatches
      << ", gaps = " << gaps
      << ", opening gaps = " << openingGaps
      << "\n\nIdentities = " << matches << "/" << lengthOfAlignment
      << " (" << 100 * matches / lengthOfAlignment << "%)"
      << ", Gaps = " << gaps << "/" << lengthOfAlignment
      << " (" << 100 * gaps / lengthOfAlignment << "%)"
      << std::endl;
}


void
SequenceAlignmentDriver::Print() {
  assert(sequences_.size() == 2);

  PrintScoreTable(S_);
  PrintAlignment(sequences_[0], sequences_[1], actions_);
  PrintReport(actions_, S_);
}

}
