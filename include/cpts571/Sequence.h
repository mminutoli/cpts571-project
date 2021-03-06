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


#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <string>
#include <ostream>

namespace cpts571 {

class Sequence {
 public:
  using iterator = std::string::iterator;
  using const_iterator = std::string::const_iterator;

  void Name(const std::string & n) { sequenceName_ = n; }
  std::string Name() const { return sequenceName_; }

  template <typename Iterator>
  void AppendChunk(const Iterator B, const Iterator E) {
    sequence_.insert(std::end(sequence_), B, E);
  }

  char operator[](size_t pos) const {
    return sequence_[pos];
  }

  size_t length() const {
    return sequence_.length();
  }

  iterator begin() { return sequence_.begin(); }
  const_iterator begin() const { return sequence_.begin(); }

  iterator end() { return sequence_.end(); }
  const_iterator end() const { return sequence_.end(); }

  friend std::ostream & operator<<(std::ostream & O, const Sequence & S) {
    O << S.Name() << ": "
      << S.sequence_;
  return O;
  }

 private:
  std::string sequenceName_;
  std::string sequence_;
};

}

#endif /* SEQUENCE_H */
