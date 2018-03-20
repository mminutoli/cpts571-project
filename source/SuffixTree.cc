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

#include <iostream>
#include <ostream>

#include "cpts571/SuffixTree.h"

namespace cpts571 {

void
SuffixTree::FindPath(
    SuffixTree::node_ptr& p, SuffixTree::node_ptr &r,
    SuffixTree::sequence_iterator itr, SuffixTree::sequence_iterator end,
    size_t i) {
  if (r != root_) {
    if (r == nullptr) {
      r = std::move(std::shared_ptr<SuffixTreeNode>(new SuffixTreeNode()));
      r->incomingArcString.insert(r->incomingArcString.begin(), itr, end);
      r->parent = p.get();
      r->isLeaf = true;
      r->suffixNumber = i;
      r->suffixLink = nullptr;
      if (last_inserted_ != nullptr)
        last_inserted_->suffixLink = r.get();
      last_inserted_ = r.get();
      return;
    }

    auto sItr = r->incomingArcString.begin(), sEnd = r->incomingArcString.end();
    for (;sItr != sEnd && itr != end && *sItr == *itr; ++sItr, ++itr);

    if (sItr == sEnd && itr == end) {
      r->isLeaf = true;
      r->suffixNumber = i;
    } else if (sItr == sEnd) {
      FindPath(r, r->children[*itr], itr, end, i);
    } else if (itr == end) {
      char startChar = r->incomingArcString[0];
      char splitChar = *sItr;

      std::shared_ptr<SuffixTreeNode> splitNode(new SuffixTreeNode());
      splitNode->incomingArcString.insert(
          splitNode->incomingArcString.begin(), r->incomingArcString.begin(), sItr);
      splitNode->parent = r->parent;
      splitNode->isLeaf = true;
      splitNode->suffixNumber = i;

      last_inserted_->suffixLink = splitNode.get();
      last_inserted_ = splitNode.get();

      r->parent = splitNode.get();
      r->incomingArcString.erase(r->incomingArcString.begin(), sItr);

      splitNode->children[splitChar] = r;
      splitNode->parent->children[startChar] = splitNode;

      return;
    } else {
      assert(*itr != *sItr);
      std::cout << "# *sItr = " << *sItr << std::endl;
      std::cout << "# *itr = " << *itr << std::endl;

      // I need to break the edge
      char startChar = r->incomingArcString[0];
      char splitChar = *sItr;

      std::shared_ptr<SuffixTreeNode> splitNode(new SuffixTreeNode());
      splitNode->incomingArcString.insert(
          splitNode->incomingArcString.begin(), r->incomingArcString.begin(), sItr);
      splitNode->parent = r->parent;
      splitNode->isLeaf = false;

      r->parent = splitNode.get();
      std::cout << "# *sItr = " << *sItr << std::endl;
      r->incomingArcString.erase(r->incomingArcString.begin(), sItr);

      std::cout << "# *sItr = " << *sItr << std::endl;
      splitNode->children[splitChar] = r;
      splitNode->parent->children[startChar] = splitNode;

      std::shared_ptr<SuffixTreeNode> theRest(new SuffixTreeNode());
      theRest->incomingArcString.insert(theRest->incomingArcString.begin(), itr, end);
      theRest->parent = splitNode.get();
      theRest->isLeaf = true;
      theRest->suffixNumber = i;

      last_inserted_->suffixLink = theRest.get();
      last_inserted_ = theRest.get();

      std::cout << "# *itr = " << *itr << std::endl;
      splitNode->children[*itr] = theRest;
      return;
    }
  } else {
    FindPath(r, r->children[*itr], itr, end, i);
  }
}

void SuffixTree::PrintDot(std::ostream & OS, SuffixTree::node_ptr &r) {
  assert(r != nullptr);
  for (auto v : r->children)
    PrintDot(OS, v.second);

  if (r->parent != nullptr) {
    OS << "\""<< r->parent << "\" -> \"" << r.get() << "\""
              << "[ label=\"" << r->incomingArcString << "\" ];"
              << std::endl;
  }
  if (r->isLeaf) {
    OS << "\"" << r << "\" [ shape=rectangle, label=\""
              << r->suffixNumber << " " << r <<  "\" ];"
              << std::endl;
  } else {
    OS << "\"" << r << "\" [ label=\"" << r << "\" ];"
              << std::endl;
  }

  if (r->suffixLink != nullptr)
    OS << "\""<< r.get() << "\" -> \"" << r->suffixLink << "\""
              << "[ color=green ]" << std::endl;
}

} // namespace cpts571
