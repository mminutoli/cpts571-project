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
    SuffixTree::node_ptr p, SuffixTree::node_ptr &r,
    SuffixTree::sequence_iterator itr, SuffixTree::sequence_iterator end,
    size_t i) {
  if (r != root_) {
    if (r == nullptr) {
      r = std::move(
          std::shared_ptr<SuffixTreeNode>(
              new SuffixTreeNode(nextNodeID_++, i, std::string(itr, end), p)));
      if (last_inserted_ != nullptr)
        last_inserted_->SuffixLink(r);
      last_inserted_ = r;
      return;
    }

    auto sItr = r->BeginIncomingArcString(), sEnd = r->EndIncomingArcString();
    for (;sItr != sEnd && itr != end && *sItr == *itr; ++sItr, ++itr);

    if (sItr == sEnd && itr == end) {
      r->SuffixNumber(i);
    } else if (sItr == sEnd) {
      FindPath(r, r->operator[](*itr), itr, end, i);
    } else if (itr == end) {
      char startChar = *r->BeginIncomingArcString();
      char splitChar = *sItr;

      std::shared_ptr<SuffixTreeNode> splitNode(
          new SuffixTreeNode(nextNodeID_++, i, std::string(r->BeginIncomingArcString(), sItr),
                             r->Parent()));

      last_inserted_->SuffixLink(splitNode);
      last_inserted_ = splitNode;

      r->Parent(splitNode);
      r->Erase(r->BeginIncomingArcString(), sItr);

      splitNode->operator[](splitChar) = r;
      splitNode->Parent()->operator[](startChar) = splitNode;

      return;
    } else {
      assert(*itr != *sItr);
      std::cout << "# *sItr = " << *sItr << std::endl;
      std::cout << "# *itr = " << *itr << std::endl;

      // I need to break the edge
      char startChar = *r->BeginIncomingArcString();
      char splitChar = *sItr;

      std::shared_ptr<SuffixTreeNode> splitNode(
          new SuffixTreeNode(nextNodeID_++, -1, std::string(r->BeginIncomingArcString(), sItr), r->Parent()));

      r->Parent(splitNode);
      std::cout << "# *sItr = " << *sItr << std::endl;
      r->Erase(r->BeginIncomingArcString(), sItr);

      std::cout << "# *sItr = " << *sItr << std::endl;
      splitNode->operator[](splitChar) = r;
      splitNode->Parent()->operator[](startChar) = splitNode;

      std::shared_ptr<SuffixTreeNode> theRest(
          new SuffixTreeNode(nextNodeID_++, i, std::string(itr, end), splitNode));

      last_inserted_->SuffixLink(theRest);
      last_inserted_ = theRest;

      std::cout << "# *itr = " << *itr << std::endl;
      splitNode->operator[](*itr) = theRest;
      return;
    }
  } else {
    FindPath(r, r->operator[](*itr), itr, end, i);
  }
}

void SuffixTree::PrintDot(std::ostream & OS, SuffixTree::node_ptr &r) {
  assert(r != nullptr);

  for (auto v : *r)
    PrintDot(OS, v.second);

  if (r->Parent() != nullptr) {
    OS << "\""<< r->Parent()->ID() << "\" -> \"" << r->ID() << "\""
       << "[ label=\"" << r->IncomingArcString() << "\" ];"
       << std::endl;
  }
  if (r->isLeaf()) {
    OS << "\"" << r->ID() << "\" [ shape=rectangle, label=\""
       << "ID : " << r->ID() << "\n"
       << "Depth : " << r->StringDepth() << "\n"
       << "SuffixID : " << r->SuffixNumber()
       << "\" ];"
       << std::endl;
  } else {
    OS << "\"" << r->ID()
       << "\" [ label=\"ID : " << r->ID() << "\n"
       << "Depth : " << r->StringDepth()
       << "\" ];"
       << std::endl;
  }

  if (r->SuffixLink() != nullptr)
    OS << "\""<< r->ID() << "\" -> \"" << r->SuffixLink()->ID() << "\""
              << "[ color=green ]" << std::endl;
}

SuffixTree::node_ptr &
SuffixTree::NodeHops(SuffixTree::sequence_iterator &itr, SuffixTree::sequence_iterator end) {
  if (last_inserted_ == nullptr) return root_;

  auto U = last_inserted_->Parent();

  size_t bethaLenght = 0;
  size_t alphaLenght = U->StringDepth();

  node_ptr & V = root_;
  if (U->SuffixLink() != nullptr && U != root_) {
    // Case IA
    itr += U->StringDepth();
    // Retrieve the smart pointer to V
    V = U->SuffixLink()->Parent();
  } else if (U->SuffixLink() != nullptr && U == root_) {
    // Case IB
    V = root_;
  } else if (U->SuffixLink() == nullptr && U != root_) {
    // Case IIA
    V = root_;
  } else {
    // Case IIB
    V = root_;
  }

  return V;
}

} // namespace cpts571
