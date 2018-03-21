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
  if (i == 4) {
    std::cout << root_ << std::endl;
  }
  if (r != root_) {
    if (r == nullptr) {
      nodePool_.push_back(SuffixTreeNode(nextNodeID_++, i, std::string(itr, end), p));
      r = &nodePool_.back();

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

      nodePool_.push_back(
          SuffixTreeNode(
              nextNodeID_++, i, std::string(r->BeginIncomingArcString(), sItr), p));
      SuffixTreeNode * splitNode = &nodePool_.back();

      last_inserted_ = splitNode;

      r->Parent(splitNode);
      r->Erase(r->BeginIncomingArcString(), sItr);

      splitNode->operator[](splitChar) = r;
      p->operator[](startChar) = splitNode;

      return;
    } else {
      assert(*itr != *sItr);

      // I need to break the edge
      char startChar = *r->BeginIncomingArcString();
      char splitChar = *sItr;

      nodePool_.push_back(
          SuffixTreeNode(
              nextNodeID_++, -1, std::string(r->BeginIncomingArcString(), sItr), p));
      SuffixTreeNode * splitNode = &nodePool_.back();

      r->Parent(splitNode);
      r->Erase(r->BeginIncomingArcString(), sItr);

      splitNode->operator[](splitChar) = r;
      p->operator[](startChar) = splitNode;

      nodePool_.push_back(
          SuffixTreeNode(nextNodeID_++, i, std::string(itr, end), splitNode));
      SuffixTreeNode * theRest = &nodePool_.back();

      last_inserted_ = theRest;

      splitNode->operator[](*itr) = theRest;
      return;
    }
  } else {
    FindPath(r, (*r)[*itr], itr, end, i);
  }
}

void SuffixTree::PrintDot(std::ostream & OS, SuffixTree::node_ptr r) {
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

SuffixTree::node_ptr
SuffixTree::NodeHops(SuffixTree::sequence_iterator &itr, SuffixTree::sequence_iterator end) {
  return root_;
  if (last_inserted_ == nullptr) return root_;

  SuffixTreeNode * U = last_inserted_->Parent();

  ssize_t bethaLenght = 0;
  ssize_t alphaLenght = U->StringDepth();

  node_ptr V = root_;
  if (U->SuffixLink() != nullptr && U != root_) {
    // Case IA
    std::cout << "Case IA" << std::endl;
    itr += U->StringDepth();
    // Retrieve the smart pointer to V
    V = U->SuffixLink()->Parent();
  } else if (U->SuffixLink() != nullptr && U == root_) {
    // Case IB
    std::cout << "Case IB" << std::endl;
    V = root_;
  } else if (U->SuffixLink() == nullptr) {
    auto UPrime = U->Parent();
    alphaLenght = UPrime->StringDepth() - 1;
    bethaLenght = U->StringDepth() - UPrime->StringDepth();

    std::cout << "$$$$ U ID " << U->ID() << std::endl;
    std::cout << "$$$$ UPrime ID " << UPrime->ID() << std::endl;
    std::cout << "$$$$ alpha = " << alphaLenght << std::endl;
    std::cout << "$$$$ betha = " << bethaLenght << std::endl;
    for (; UPrime->SuffixLink() == nullptr; UPrime = UPrime->Parent()) {
      bethaLenght += UPrime->StringDepth();
      alphaLenght -= bethaLenght;
      std::cout << "$$$$ UPrime ID " << UPrime->ID() << std::endl;
      std::cout << "$$$$ alpha = " << alphaLenght << std::endl;
      std::cout << "$$$$ betha = " << bethaLenght << std::endl;
    }

    std::cout << "$$$$ alpha = " << alphaLenght << std::endl;
    std::cout << "$$$$ betha = " << bethaLenght << std::endl;

    auto VPrime = UPrime->SuffixLink();
    std::cout << "$$$$ UPrime ID " << UPrime->ID() << std::endl;
    std::cout << "$$$$ VPrime ID " << VPrime->ID() << std::endl;

    if (UPrime != root_) {
      // Case IIA
      std::cout << "Case IIA" << std::endl;
      itr += alphaLenght;

      VPrime = VPrime->operator[](*itr);
      std::cout << "$$$$ VPrime ID " << VPrime->ID() << std::endl;
      std::cout << "$$$$ VPrime String " << VPrime->IncomingArcString() << std::endl;
      std::cout << "$$$$ " << *itr << " " << VPrime->IncomingArcString()[0] << std::endl;
      assert(*itr == *VPrime->BeginIncomingArcString());
      
      std::cout << "$$$$ VPrime->IncomingArcString().size() " << VPrime->IncomingArcString().size() << std::endl;
      std::cout << "$$$$ std::distance(itr, end) " << std::distance(itr, end) << std::endl;
      while (bethaLenght > 0 && bethaLenght > VPrime->IncomingArcString().size() &&
             std::distance(itr, end) > VPrime->IncomingArcString().size()) {
        std::cout << "$$$$ VPrime->IncomingArcString().size() " << VPrime->IncomingArcString().size() << std::endl;
        std::cout << "$$$$ std::distance(itr, end) " << std::distance(itr, end) << std::endl;
        std::cout << "$$$$ " << *itr << " " << VPrime->IncomingArcString()[0] << std::endl;

        std::cout << "$$$$ HERE " << *itr;
        itr += VPrime->IncomingArcString().size();
        std::cout << " -> " << *itr << std::endl;
        bethaLenght -= VPrime->IncomingArcString().size();
        VPrime = VPrime->operator[](*itr);
        assert(VPrime != nullptr);
        std::cout << "$$$$ betha = " << bethaLenght << std::endl;
      }

      V = VPrime;
      U->SuffixLink(V);
    } else {
      // Case IIB : U' is the root
      bethaLenght -= 1;

      std::cout << "Case IIB : betha = " << bethaLenght << std::endl;

      auto VPrime = UPrime->SuffixLink();
      
      while (bethaLenght > 0 && bethaLenght > VPrime->IncomingArcString().size() &&
             std::distance(itr, end) > VPrime->IncomingArcString().size()) {
        std::cout << "$$$$ HERE " << *itr;
        itr += VPrime->IncomingArcString().size();
        std::cout << " -> " << *itr << std::endl;
        bethaLenght -= VPrime->IncomingArcString().size();
        VPrime = VPrime->operator[](*itr);
      }

      if (bethaLenght > 0 && bethaLenght < VPrime->IncomingArcString().size()) {
        auto p = VPrime; 
        VPrime = VPrime->operator[](*itr);

        itr += bethaLenght;

        // I need to break the edge
        char startChar = *VPrime->BeginIncomingArcString();
        char splitChar = *(VPrime->BeginIncomingArcString() + bethaLenght);

        nodePool_.push_back(
            SuffixTreeNode(
                nextNodeID_++, -1, std::string(VPrime->BeginIncomingArcString(), VPrime->BeginIncomingArcString() + bethaLenght), VPrime->Parent()));
        SuffixTreeNode * splitNode = &nodePool_.back();

        VPrime->Parent(splitNode);
        VPrime->Erase(VPrime->BeginIncomingArcString(), VPrime->BeginIncomingArcString() + bethaLenght);

        splitNode->operator[](splitChar) = VPrime;
        p->operator[](startChar) = splitNode;

        VPrime = splitNode;
      }

      std::cout << "Case IIB : betha = " << bethaLenght << std::endl;
      V = VPrime;
      U->SuffixLink(V);
    }
  } else {
    assert(false);
  }

  std::cout << "Will start at node " << V->ID() << std::endl;
  return V;
}

} // namespace cpts571
