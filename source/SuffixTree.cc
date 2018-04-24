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
#include <cassert>
#include <tuple>

#include "cpts571/SuffixTree.h"

namespace cpts571 {

void
SuffixTree::insertSuffix(Sequence::const_iterator B, Sequence::const_iterator E, size_t i) {
  node_ptr U = last_inserted_ ? last_inserted_->Parent() : root_;
  if (U->SuffixLink() != nullptr) {
    // Case I
    if (U != root_) {
      // Case IA
      node_ptr V = U->SuffixLink();
      size_t alphaLenght = V->StringDepth();
      B += alphaLenght;
      FindPath(V, (*V)[*B], B, E, i);
    } else {
      // Case IB
      FindPath(root_, root_, B, E, i);
    }
  } else {
    // Case II
    node_ptr UPrime = U->Parent();
    if (UPrime != root_) {
      // Case IIA
      assert(UPrime->SuffixLink() != nullptr);
      node_ptr VPrime = UPrime->SuffixLink();
      size_t alphaPrimeLenght = VPrime->StringDepth();
      B += alphaPrimeLenght;
      size_t bethaLenght = U->StringDepth() - alphaPrimeLenght - 1;
      node_ptr V = NodeHops(VPrime, B, B + bethaLenght);
      assert(V != nullptr);
      U->SuffixLink(V);
      B += bethaLenght;
      FindPath(V, V->operator[](*B), B, E, i);
    } else {
      // Case IIB
      size_t bethaPrimeLenght = U->StringDepth() - 1;
      node_ptr V = NodeHops(root_, B, B + bethaPrimeLenght);
      assert(V != nullptr);
      U->SuffixLink(V);
      B += bethaPrimeLenght;
      FindPath(V, V->operator[](*B), B, E, i);
    }
  }
}

std::vector<size_t>
SuffixTree::FindLoc(const Sequence & read) const {
  auto itr = read.begin();
  auto end = read.end();

  node_ptr node = root_;
  node_ptr deepestNode = root_;

  assert(itr != end);

  while (itr != end) {
    std::tie(node, itr) = FindLoc(node, itr, end);

    if (node->StringDepth() >= x_ &&
        deepestNode->StringDepth() < node->StringDepth()) {
      deepestNode = node;
    }
    node = node->SuffixLink();
  }

  return std::vector<size_t>(
      &A_[deepestNode->StartLeafIndex()], &A_[deepestNode->EndLeafIndex()]);
}

std::tuple<SuffixTree::node_ptr, Sequence::const_iterator>
SuffixTree::FindLoc(
    SuffixTree::node_ptr r,
    Sequence::const_iterator itr, Sequence::const_iterator end) const {

  auto sItr = r->BeginIncomingArcString(), sEnd = r->EndIncomingArcString();
  for (;sItr != sEnd && itr != end && *sItr == *itr; ++sItr, ++itr);

  if (sItr == sEnd) {
    if (itr != end) {
      auto nextRoot = r->operator[](*itr);
      if (nextRoot != nullptr)
        return FindLoc(nextRoot, itr, end);
    }
    return std::make_tuple(r, itr);
  }

  itr -= std::distance(r->BeginIncomingArcString(), sItr);
  return std::make_tuple(r->Parent(), itr);
}

void
SuffixTree::FindPath(
    SuffixTree::node_ptr p, SuffixTree::node_ptr &r,
    SuffixTree::sequence_iterator itr, SuffixTree::sequence_iterator end,
    size_t i) {
  if (r == nullptr) {
    assert(itr != end);
    // Adding a leaf
    leavesPool_.push_back(SuffixTreeNode(nextNodeID_++, i, itr, end, p));
    r = &leavesPool_.back();

    last_inserted_ = r;
    return;
  }

  auto sItr = r->BeginIncomingArcString(), sEnd = r->EndIncomingArcString();
  for (;sItr != sEnd && itr != end && *sItr == *itr; ++sItr, ++itr);

  if (sItr == sEnd) {
    FindPath(r, r->operator[](*itr), itr, end, i);
  } else {
    assert(*itr != *sItr);
    assert(std::distance(r->BeginIncomingArcString(), sItr) > 0);

    SuffixTreeNode * splitNode = SplitNode(r, std::distance(r->BeginIncomingArcString(), sItr));

    // Add the remaninig part
    leavesPool_.push_back(
        SuffixTreeNode(nextNodeID_++, i, itr, end, r));
    SuffixTreeNode * theRest = &leavesPool_.back();

    last_inserted_ = theRest;
    (*splitNode)[*itr] = theRest;
  }
}

void SuffixTree::PrintDot(std::ostream & OS) {
  OS << "digraph G {" << std::endl;
  for (auto r : *this) {
    if (r->Parent() != nullptr) {
      OS << "\""<< r->Parent()->ID() << "\" -> \"" << r->ID() << "\""
         << "[ label=\"" << r->IncomingArcString() << "\" ];"
         << std::endl;
    }
    if (r->isLeaf()) {
      OS << "\"" << r->ID() << "\" [ shape=rectangle, label=\""
         << "ID : " << r->ID() << "\n"
         << "Depth : " << r->StringDepth() << "\n"
         << "SuffixID : " << r->SuffixNumber() << "\n"
         << "StartLeafIndex : " << r->StartLeafIndex() << "\n"
         << "EndLeafIndex : " << r->EndLeafIndex()
         << "\" ];"
         << std::endl;
    } else {
      OS << "\"" << r->ID()
         << "\" [ label=\"ID : " << r->ID() << "\n"
         << "Depth : " << r->StringDepth() << "\n"
         << "StartLeafIndex : " << r->StartLeafIndex() << "\n"
         << "EndLeafIndex : " << r->EndLeafIndex()
         << "\" ];"
         << std::endl;
    }

    if (r->SuffixLink() != nullptr)
      OS << "\""<< r->ID() << "\" -> \"" << r->SuffixLink()->ID() << "\""
         << "[ color=green ]" << std::endl;
  }
  OS << "}" << std::endl;
}

SuffixTree::node_ptr
SuffixTree::SplitNode(node_ptr r, ptrdiff_t distance) {
  // Create the new node.
  SuffixTreeNode * oldParentOfR = r->Parent();
  {
    auto B  = r->BeginIncomingArcString();
    auto SP = r->BeginIncomingArcString() + distance;

    // Split nodes are always internal
    internalNodesPool_.push_back(
        SuffixTreeNode(
            nextNodeID_++, r->StringDepth() - std::distance(SP, r->EndIncomingArcString()),
            B, SP, oldParentOfR));
    // Update r string.
    r->MoveStartTo(SP);
  }

  // We are splitting edges preserving leaves.
  SuffixTreeNode * splitNode = &internalNodesPool_.back();

  // Connect r -> splitNode
  r->Parent(splitNode);
  (*splitNode)[*r->BeginIncomingArcString()] = r;

  // Connect splitNode to the parent.
  (*oldParentOfR)[*splitNode->BeginIncomingArcString()] = splitNode;

  return splitNode;
}

SuffixTree::node_ptr
SuffixTree::NodeHops(SuffixTree::node_ptr r, SuffixTree::sequence_iterator itr, SuffixTree::sequence_iterator end) {
  assert(std::distance(itr, end) >= 0);

  if (itr == end) return r;

  node_ptr next = r->operator[](*itr);
  assert(next != nullptr);
  size_t jumpLenght = next->StringDepth() - r->StringDepth();

  if (itr + jumpLenght > end) {
    return SplitNode(next, std::distance(itr, end));
  }

  return NodeHops(next, itr + jumpLenght, end);
}

} // namespace cpts571
