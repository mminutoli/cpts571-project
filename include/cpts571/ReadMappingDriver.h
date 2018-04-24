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

#ifndef READ_MAPPING_DRIVER_H
#define READ_MAPPING_DRIVER_H

#include <string>
#include <vector>

#include "cpts571/Alignment.h"
#include "cpts571/Sequence.h"
#include "cpts571/SuffixTree.h"

namespace cpts571 {

struct ReadMappingDriverConfiguration {
  std::string GeneFileName;
  std::string ReadsFileName;
  std::string CFileName;
  size_t x;
  double X;
  double Y;
};

class ReadMappingDriver {
 public:
  ReadMappingDriver(const ReadMappingDriverConfiguration & C)
      : gene_()
      , reads_()
      , config_(C)
  {
    Parse(config_.GeneFileName, config_.ReadsFileName, config_.CFileName);
  }

  void Parse(const std::string &GF, const std::string &RF, const std::string &CF);

  void Exec();

 private:
  ScoreTable scoreTable_{ +1, -2, -5, -1 };
  Sequence gene_;
  std::vector<Sequence> reads_;
  ReadMappingDriverConfiguration config_;
};

}

#endif /* READ_MAPPING_DRIVER_H */
