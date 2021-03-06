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
#include <chrono>
#include <limits>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <tuple>

#include "cpts571/Sequence.h"

namespace cpts571 {

class SuffixTreeNode {
 public:
  using node_ptr  = SuffixTreeNode *;
  using const_node_ptr = SuffixTreeNode const *;
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
      , startLeafIndex_(-1)
      , endLeafIndex_(-1)
      , parent_(parent)
      , suffixLink_(nullptr)
      , children_()
  {}

  size_t ID() const { return ID_;}
  void ID(size_t ID) { ID_ = ID; }

  ssize_t SuffixNumber() const { return suffixNumber_; }
  void SuffixNumber(ssize_t SFN) { suffixNumber_ = SFN; }

  ssize_t StartLeafIndex() const { return startLeafIndex_; }
  void StartLeafIndex(ssize_t SLI) { startLeafIndex_ = SLI; }

  ssize_t EndLeafIndex() const { return endLeafIndex_; }
  void EndLeafIndex(ssize_t SLI) { endLeafIndex_ = SLI; }

  size_t StringDepth() const { return stringDepth_; }
  void StringDepth(size_t D) { stringDepth_ = D; }

  parent_ptr Parent() const { return parent_; }
  void Parent(parent_ptr P) { parent_ = P; }

  suffix_link_ptr SuffixLink() const { return suffixLink_; }
  void SuffixLink(suffix_link_ptr SL) { suffixLink_ = SL; }

  typename children_map::iterator begin() { return children_.begin(); }
  typename children_map::iterator end() { return children_.end(); }

  typename children_map::reverse_iterator rbegin() { return children_.rbegin(); }
  typename children_map::reverse_iterator rend() { return children_.rend(); }

  typename children_map::mapped_type &
  operator[](const typename children_map::key_type &i) { return children_[i]; }

  typename std::string::const_iterator
  BeginIncomingArcString() const { return arcStringStart_; }
  typename std::string::const_iterator
  EndIncomingArcString() const { return arcStringEnd_; }

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

  ssize_t startLeafIndex_;
  ssize_t endLeafIndex_;

  parent_ptr parent_;
  suffix_link_ptr suffixLink_;
  children_map children_;
};

class SuffixTree {
 public:
  using node_ptr = SuffixTreeNode::node_ptr;
  using const_node_ptr = SuffixTreeNode const *;
  using sequence_iterator = Sequence::const_iterator;
  using stack_element =
      std::pair<node_ptr, typename std::map<char, node_ptr>::const_iterator>;

  SuffixTree(const Sequence &s, size_t x = 0)
      : sequence_(s)
      , root_(nullptr)
      , last_inserted_ (nullptr)
      , nextNodeID_(2)
      , A_(s.length(), -1)
      , x_(x) {
    // Root is going to be an internal node
    internalNodesPool_.push_back(SuffixTreeNode(1, -1, sequence_.begin(), sequence_.begin(), nullptr));
    root_ = &internalNodesPool_.front();
    root_->Parent(root_);
    assert(root_ != nullptr);
    root_->SuffixLink(root_);
    
    auto beginBuild = std::chrono::steady_clock::now();
    buildSuffixTree(s.begin(), s.end());
    auto endBuild = std::chrono::steady_clock::now();

    double buildTime = std::chrono::duration_cast<
      std::chrono::duration<double> >(endBuild - beginBuild).count();
    std::cout << "# SuffixTree created in " << buildTime
              << std::endl;

    auto beginPrepare = std::chrono::steady_clock::now();
    prepareSuffixTree();
    auto endPrepare = std::chrono::steady_clock::now();

    double prepareTime = std::chrono::duration_cast<
      std::chrono::duration<double> >(endPrepare - beginPrepare).count();
    std::cout << "# SuffixTree prepared in " << prepareTime << std::endl;
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

  typename std::deque<SuffixTreeNode>::iterator
  begin_leaves() { return leavesPool_.begin(); }

  typename std::deque<SuffixTreeNode>::iterator
  end_leaves() { return leavesPool_.end(); }

  typename std::deque<SuffixTreeNode>::const_iterator
  begin_leaves() const { return leavesPool_.begin(); }

  typename std::deque<SuffixTreeNode>::const_iterator
  end_leaves() const { return leavesPool_.end(); }

  std::string BTW(Sequence &S) {
    std::string result(S.length(), '\0');

    size_t i = 0;

    // here order matters we can't Iterate over the leavesPool
    for (auto v : *this) {
      if (v->isLeaf()) {
        auto leaf = v->SuffixNumber();
        result[i++] = S[ leaf > 0 ? leaf - 1 : S.length() - 1];
      }
    }

    return result;
  }

  std::tuple<size_t, size_t, std::string> LCS() const {
    // For all possible pairs of suffixes
    auto max = internalNodesPool_.begin();
    for (auto itr = max + 1, end = internalNodesPool_.end(); itr != end; ++itr) {
      if (itr->StringDepth() > max->StringDepth())
        max = itr;
    } 

    // Climb up.
    size_t startP = max->SuffixNumber();
    size_t endP = startP + max->StringDepth();
    std::string s(sequence_.begin() + startP, sequence_.begin() + endP);
    return std::make_tuple(startP, endP, s);
  }

  const_node_ptr
  LCA(const_node_ptr S1, const_node_ptr S2, size_t bound = 0) const {
    // We reached the top of the tree
    if(S1->StringDepth() == 0 || S2->StringDepth() == 0) return root_;

    // S1 is the LCA
    if(S1 == S2->Parent()) return S1;

    // S2 is the LCA
    if(S2 == S1->Parent()) return S2;

    // We know that there is a better solution.  Stop and report nullptr as don't care.
    if (S1->StringDepth() < bound || S2->StringDepth() < bound) return nullptr;

    // Go Up from the deepest side
    if(S1->StringDepth() > S2->StringDepth())
      return LCA(S1->Parent(), S2, bound);
    else
      return LCA(S1, S2->Parent(), bound);
  }

  void DFSPrint(std::ostream &OS) {
    for (auto node : *this) {
      OS << node->ID() << '\n';
    }
    OS << std::endl;
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

  void PrintStats(std::ostream &OS) {
    size_t numberOfNodes = internalNodesPool_.size() + leavesPool_.size();
    size_t numberOfLeaves = leavesPool_.size();
    size_t totDepth = 0;
    size_t maxDepth = 0;

    for (auto & n : internalNodesPool_) {
      totDepth += n.StringDepth();
      maxDepth = std::max(maxDepth, n.StringDepth());
    }

    for (auto & n : leavesPool_) totDepth += n.StringDepth();

    size_t internalNodes = internalNodesPool_.size();
    OS << "# Total number of nodes : " << numberOfNodes << "\n"
       << "# Number of leaves : " << numberOfLeaves << "\n"
       << "# Number of internal nodes : " << internalNodes << "\n"
       << "# Max Depth : " << maxDepth << "\n"
       << "# Average Depth : " << double(totDepth) / internalNodes << "\n"
       << "# Esistimate of the SuffixTree (Bytes) : "
       << sizeof(SuffixTreeNode) * numberOfNodes +
        std::distance(sequence_.begin(), sequence_.end()) * sizeof(std::string::value_type) +
        sizeof(SuffixTreeNode::children_map::value_type) * internalNodes
       << std::endl;
  }

  std::vector<size_t> FindLoc(const Sequence & read) const;

 private:
  std::tuple<node_ptr, Sequence::const_iterator>
  FindLoc(node_ptr r, Sequence::const_iterator i, Sequence::const_iterator E) const;
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

  void prepareSuffixTree() {
    size_t nextIndex = 0;
    prepareSuffixTree(root_, nextIndex);
  }

  void prepareSuffixTree(node_ptr v, size_t & nextIndex) {
    if (v == nullptr) return;

    if (v->isLeaf()) {
      A_[nextIndex] = v->SuffixNumber();
      if (v->StringDepth() >= x_) {
        v->StartLeafIndex(nextIndex);
        v->EndLeafIndex(nextIndex);
      }
      ++nextIndex;
      return;
    }

    for (auto child : *v) {
      prepareSuffixTree(child.second, nextIndex);
    }

    if (v->StringDepth() >= x_) {
      v->StartLeafIndex(v->begin()->second->StartLeafIndex());
      v->EndLeafIndex(v->rbegin()->second->EndLeafIndex());
    }
  }

  Sequence sequence_;
  std::deque<SuffixTreeNode> internalNodesPool_;
  std::deque<SuffixTreeNode> leavesPool_;
  std::vector<ssize_t> A_;
  size_t x_;

  node_ptr root_;
  node_ptr last_inserted_;
  size_t nextNodeID_;
};

}  // namespace cpts571

#endif  // SUFFIX_TREE_H
