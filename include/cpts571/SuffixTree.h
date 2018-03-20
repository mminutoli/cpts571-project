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


#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "cpts571/Sequence.h"

namespace cpts571 {

class SuffixTreeNode {
 public:
  using node_ptr  = std::shared_ptr<SuffixTreeNode>;
  using parent_ptr = std::shared_ptr<SuffixTreeNode>;
  using suffix_link_ptr = std::shared_ptr<SuffixTreeNode>;
  using children_map = std::map<char, node_ptr>;

  SuffixTreeNode(
      size_t ID, ssize_t suffixNumber, std::string incomingArcString, parent_ptr parent)
      : ID_(ID)
      , suffixNumber_(suffixNumber)
      , incomingArcString_(incomingArcString)
      , stringDepth_(parent != nullptr ? parent->StringDepth() + incomingArcString.size() : 0)
      , parent_(parent)
      , suffixLink_(nullptr)
  {}

  explicit SuffixTreeNode(size_t ID)
      : SuffixTreeNode(ID, -1, "", nullptr) {}

  size_t ID() const { return ID_;}
  void ID(size_t ID) { ID_ = ID; }

  ssize_t SuffixNumber() const { return suffixNumber_; }
  void SuffixNumber(ssize_t SFN) { suffixNumber_ = SFN; }

  size_t StringDepth() const { return stringDepth_; }
  void StringDepth(size_t D) { stringDepth_ = D; }

  parent_ptr Parent() const { return parent_; }
  void Parent(parent_ptr P) { parent_ = P; }

  suffix_link_ptr SuffixLink() const { return suffixLink_; }
  void SuffixLink(suffix_link_ptr SL) { suffixLink_ = SL; }

  typename children_map::iterator begin() { return children_.begin(); }
  typename children_map::iterator end() { return children_.end(); }

  typename children_map::mapped_type &
  operator[](const typename children_map::key_type &i) { return children_[i]; }

  typename std::string::iterator
  BeginIncomingArcString() { return incomingArcString_.begin(); }
  typename std::string::iterator
  EndIncomingArcString() { return incomingArcString_.end(); }

  std::string IncomingArcString() const { return incomingArcString_; }

  void
  Erase(typename std::string::iterator S, typename std::string::iterator E) {
    incomingArcString_.erase(S, E);
  }

  bool isLeaf() const { return children_.empty(); }

 private:
  size_t ID_;
  ssize_t suffixNumber_;

  std::string incomingArcString_;
  size_t stringDepth_;

  parent_ptr parent_;
  suffix_link_ptr suffixLink_;
  children_map children_;
};

class SuffixTree {
 public:
  using node_ptr = SuffixTreeNode::node_ptr;
  using sequence_iterator = Sequence::const_iterator;
  using stack_element =
      std::pair<node_ptr, typename std::map<char, node_ptr>::const_iterator>;

  SuffixTree(const Sequence &s)
      : root_(new SuffixTreeNode(0))
      , last_inserted_ (nullptr)
      , nextNodeID_(1) {
    assert(root_ != nullptr);
    root_->SuffixLink(root_);
    buildSuffixTree(s.begin(), s.end());
  }

  class dfs_iterator {
   public:
    dfs_iterator() = default;

    dfs_iterator(node_ptr & r)
        : current_(r) {
      stack_.push_back(std::make_pair(r, r->begin()));
    }

    dfs_iterator & operator++() {
      toNextNode();
      return *this;
    }

    dfs_iterator operator++(int) {
      dfs_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const dfs_iterator &x) {
      return stack_ == x.stack_;
    }

    bool operator!=(const dfs_iterator &x) {
      return !(*this == x);
    }

    const node_ptr & operator*() const { return stack_.back().first; }
    
   private:
    void toNextNode() {
      do {
        node_ptr n = stack_.back().first;
        auto & child_itr = stack_.back().second;

        while (child_itr != n->end()) {
          auto nextNode = child_itr->second;
          stack_.push_back(std::make_pair(nextNode, nextNode->begin()));
          return;
        }

        stack_.pop_back();
      } while (!stack_.empty());
    }

    std::vector<stack_element> stack_;
    node_ptr current_;
  };

  dfs_iterator begin() { return dfs_iterator(root_); }
  dfs_iterator end() { return dfs_iterator(); }

  void PrintDot(std::ostream &OS) {
    OS << "digraph G {" << std::endl;
    PrintDot(OS, root_);
    OS << "}" << std::endl;
  }

 private:

  void PrintDot(std::ostream &OS, node_ptr &r);

  void FindPath(node_ptr p, node_ptr &r, sequence_iterator itr, sequence_iterator end, size_t i);

  node_ptr & NodeHops(sequence_iterator &itr, sequence_iterator end);

  void insertSuffix(Sequence::const_iterator B, Sequence::const_iterator E, size_t i) {
    auto startingPoint = NodeHops(B, E);
    if (startingPoint->Parent() == root_)
      FindPath(root_, root_, B, E, i);
    else
      FindPath(startingPoint->Parent(), startingPoint, B, E, i);
  }

  void buildSuffixTree(Sequence::const_iterator B, Sequence::const_iterator E) {
    for (auto itr = B; itr != E - 1; ++itr) {
      insertSuffix(itr, E, std::distance(B, itr));
    }
  }

  node_ptr root_;
  node_ptr last_inserted_;
  size_t nextNodeID_;
};

}  // namespace cpts571

#endif  // SUFFIX_TREE_H
