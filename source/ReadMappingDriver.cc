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

#include <chrono>
#include <fstream>

#include "cpts571/SequenceParserDriver.h"
#include "cpts571/ReadMappingDriver.h"

#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/istreamwrapper.h"

namespace cpts571 {

void
ReadMappingDriver::Parse(
    const std::string &GF, const std::string &RF, const std::string &CF) {
  SequenceParserDriver SPD(GF);
  
  gene_ = std::move(SPD.Parse())[0];

  SequenceParserDriver SPD2(RF);

  reads_ = std::move(SPD2.Parse());

  std::ifstream configFile(CF.c_str());

  if (!configFile.good()) return;

  rapidjson::IStreamWrapper ISW(configFile);
  rapidjson::Document document;
  document.ParseStream(ISW);

  scoreTable_.Match = document["match"].GetInt64();
  scoreTable_.Mismatch = document["mismatch"].GetInt64();
  scoreTable_.H = document["h"].GetInt64();
  scoreTable_.G = document["g"].GetInt64();
}

void ReadMappingDriver::Exec()  {
  std::string terminal("$");
  gene_.AppendChunk(terminal.begin(), terminal.end());
  std::cout << "# Gene length : " << gene_.length() << std::endl;
  std::cout << "# Read number : " << reads_.size() << std::endl;
  SuffixTree ST(gene_, config_.x);

  std::vector<ssize_t> result;
  result.reserve(reads_.size());

  double readsWithMatches = 0;
  double totNumberOfMatches = 0;
  auto beginMapReads = std::chrono::steady_clock::now();
  for (auto & r : reads_) {
    auto locations = ST.FindLoc(r);

    double bestLengthCoverage = 0;
    ssize_t bestStart = -1;
    if (locations.size() > 0) {
      ++readsWithMatches;
      totNumberOfMatches += locations.size();
    }
    for (auto l : locations) {
      ssize_t startPos = std::max<ssize_t>(0, l - r.length());
      ssize_t endPos = std::min<ssize_t>(gene_.length(), l + r.length());

      Sequence s;
      s.AppendChunk(gene_.begin() + startPos, gene_.begin() + endPos);

      auto actions = Alignment(s, r, scoreTable_, local_alignment_tag());

      double matches = 0;
      double alignLength = 0;
        
      for (auto a : actions) {
        switch (a) {
          case Action::Match:
            ++matches;
          case Action::Insertion:
          case Action::Deletion:
            ++alignLength;
            break;
          default:
            break;
        }
      }

      double percentIdentity = matches / alignLength;
      double lengthCoverage = alignLength / r.length();

      size_t delta = 0;
      if (percentIdentity >= config_.X && lengthCoverage >= config_.Y &&
          bestLengthCoverage <= lengthCoverage) {
        bestLengthCoverage = lengthCoverage;
        for (auto itr = actions.rbegin(), end = actions.rend();
             itr != end; ++itr) {
          if (*itr == Action::Match) break;
          if (*itr == Action::DC_Deletion)
            ++delta;
        }

        bestStart = startPos + delta;
      }
    }

    result.push_back(bestStart);
  }
  auto endMapReads = std::chrono::steady_clock::now();
  double MapReadsTime = std::chrono::duration_cast<
    std::chrono::duration<double> >(endMapReads - beginMapReads).count();

  std::cout << "# MapReads completed in " << MapReadsTime
            << "\n# Average number of alignment per read (w/ hits) : " << totNumberOfMatches/readsWithMatches
            << std::endl;

  
  auto beginOutput = std::chrono::steady_clock::now();

  auto itrRes = result.begin();
  for (auto itrReads = reads_.begin(), endReads = reads_.end();
       itrReads != endReads; ++itrReads, ++itrRes) {
    if (*itrRes != -1) {
      std::cout << itrReads->Name()
                << " " << *itrRes << " " << *itrRes + itrReads->length()
                << std::endl;
    } else {
      std::cout << itrReads->Name() << " No Hit Found"<< std::endl;
    }
  }

  auto endOutput = std::chrono::steady_clock::now();
  double OutputTime = std::chrono::duration_cast<
    std::chrono::duration<double> >(endOutput - beginOutput).count();

  std::cout << "# Output completed in " << OutputTime
            << std::endl;
}

}
