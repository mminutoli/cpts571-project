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

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "cpts571/Sequence.h"

namespace cpts571 {

struct SuffixTreeNode {
  using node_ptr  = std::shared_ptr<SuffixTreeNode>;
  using parent_ptr = SuffixTreeNode *;
  using suffix_link_ptr = SuffixTreeNode *;

  SuffixTreeNode()
      : incomingArcString()
      , isLeaf(false)
      , suffixNumber(-1)
      , parent()
      , suffixLink()
      , children()
  {}

  std::string incomingArcString;
  bool isLeaf;
  ssize_t suffixNumber;

  parent_ptr parent;
  suffix_link_ptr suffixLink;
  std::map<char, node_ptr> children;
};

class SuffixTree {
 public:
  using node_ptr = SuffixTreeNode::node_ptr;
  using sequence_iterator = Sequence::const_iterator;
  using stack_element =
      std::pair<node_ptr, typename std::map<char, node_ptr>::const_iterator>;

  SuffixTree(const Sequence &s)
      : root_(new SuffixTreeNode())
      , last_inserted_ (nullptr) {
    assert(root_ != nullptr);
    root_->suffixLink = root_.get();
    buildSuffixTree(s.begin(), s.end());
  }

  class dfs_iterator {
   public:
    dfs_iterator() = default;

    dfs_iterator(node_ptr & r)
        : current_(r) {
      stack_.push_back(std::make_pair(r, r->children.begin()));
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

        while (child_itr != n->children.end()) {
          auto nextNode = child_itr->second;
          std::cout << "# " << child_itr->first << std::endl;
          stack_.push_back(std::make_pair(nextNode, nextNode->children.begin()));
          std::cout << "$ " << child_itr->first << std::endl;
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

  void FindPath(node_ptr& p, node_ptr &r, sequence_iterator itr, sequence_iterator end, size_t i);

  void insertSuffix(Sequence::const_iterator B, Sequence::const_iterator E, size_t i) {
    FindPath(root_, root_, B, E, i);
  }

  void buildSuffixTree(Sequence::const_iterator B, Sequence::const_iterator E) {
    for (auto itr = B; itr != E && std::distance(B, itr) < 11; ++itr) {
      insertSuffix(itr, E, std::distance(B, itr));
    }
  }

  node_ptr root_;
  SuffixTreeNode * last_inserted_;
};

}  // namespace cpts571

#endif  // SUFFIX_TREE_H
