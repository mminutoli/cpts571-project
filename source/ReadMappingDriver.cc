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

#include <fstream>

#include "cpts571/SequenceParserDriver.h"
#include "cpts571/ReadMappingDriver.h"

namespace cpts571 {

void
ReadMappingDriver::Parse(const std::string &GF, const std::string &RF) {
  SequenceParserDriver SPD(GF);
  
  gene_ = std::move(SPD.Parse())[0];

  SequenceParserDriver SPD2(RF);

  reads_ = std::move(SPD2.Parse());
}

}
