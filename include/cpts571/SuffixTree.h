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
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include "cpts571/Sequence.h"

namespace cpts571 {

class SuffixTreeNode {
 public:
  using node_ptr  = SuffixTreeNode *;
  using parent_ptr = SuffixTreeNode *;
  using suffix_link_ptr = SuffixTreeNode *;
  using children_map = std::map<char, node_ptr>;
  using string_const_iterator = typename std::string::const_iterator;

  SuffixTreeNode(
      size_t ID, ssize_t suffixNumber,
      string_const_iterator SStart, string_const_iterator SEnd, parent_ptr parent)
      : ID_(ID)
      , suffixNumber_(suffixNumber)
      , arcStringStart_(SStart)
      , arcStringEnd_(SEnd)
      , stringDepth_(parent != nullptr ? parent->StringDepth() + std::distance(SStart, SEnd) : 0)
      , parent_(parent)
      , suffixLink_(nullptr)
      , children_()
  {}

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

  typename std::string::const_iterator
  BeginIncomingArcString() { return arcStringStart_; }
  typename std::string::const_iterator
  EndIncomingArcString() { return arcStringEnd_; }

  std::string IncomingArcString() const { return std::string(arcStringStart_, arcStringEnd_); }

  void
  MoveStartTo(string_const_iterator S) {
    arcStringStart_ = S;
  }

  bool isLeaf() const { return children_.empty(); }

 private:
  size_t ID_;
  ssize_t suffixNumber_;

  string_const_iterator arcStringStart_;
  string_const_iterator arcStringEnd_;

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
      : sequence_(s)
      , root_(nullptr)
      , last_inserted_ (nullptr)
      , nextNodeID_(2) {

    nodePool_.push_back(SuffixTreeNode(1, -1, sequence_.begin(), sequence_.begin(), nullptr));
    root_ = &nodePool_.front();
    root_->Parent(root_);
    assert(root_ != nullptr);
    root_->SuffixLink(root_);
    buildSuffixTree(s.begin(), s.end());
  }

  class dfs_iterator {
   public:
    dfs_iterator() = default;

    dfs_iterator(node_ptr r)
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
          child_itr++;
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

  std::string BTW(Sequence &S) {
    std::string result(S.length(), '\0');

    size_t i = 0;
    for (auto v : *this) {
      if (v->isLeaf()) {
        auto leaf = v->SuffixNumber();
        result[i++] = S[ leaf > 0 ? leaf - 1 : S.length() - 1];
      }
    }

    return result;
  }

  void PostOrderPrint(std::ostream &OS) {
    PostOrderPrint(root_, OS);
  }

  void PostOrderPrint(node_ptr r, std::ostream &OS) {
    if (r->isLeaf()) {
      OS << r->ID() << std::endl;
      return;
    }

    for (auto child : *r) {
      PostOrderPrint(child.second, OS);
    }

    OS << r->ID() << std::endl;
  }

  void PrintDot(std::ostream &OS);

 private:
  node_ptr SplitNode(node_ptr r, ptrdiff_t distance);

  void PrintDot(std::ostream &OS, node_ptr r);

  void FindPath(node_ptr p, node_ptr &r, sequence_iterator itr, sequence_iterator end, size_t i);

  node_ptr NodeHops(node_ptr r, sequence_iterator itr, sequence_iterator end);

  void insertSuffix(Sequence::const_iterator B, Sequence::const_iterator E, size_t i);

  void buildSuffixTree(Sequence::const_iterator B, Sequence::const_iterator E) {
    for (auto itr = B; itr != E; ++itr) {
      insertSuffix(itr, E, std::distance(B, itr));
    }
  }

  Sequence sequence_;
  std::deque<SuffixTreeNode> nodePool_;
  node_ptr root_;
  node_ptr last_inserted_;
  size_t nextNodeID_;
};

}  // namespace cpts571

#endif  // SUFFIX_TREE_H
