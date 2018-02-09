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


#include <algorithm>
#include <iostream>
#include <iomanip>
#include <memory>
#include <deque>
#include <tuple>

#include "cpts571/Sequence.h"

#ifndef _ALIGNMENT_H_
#define _ALIGNMENT_H_

namespace cpts571 {

//! \brief Possible actions during alignment
enum class Action {
  Match,
  Mismatch,
  Insertion,
  Deletion,
  DC_MatchMismatch, //< This is a don't care action needed for the print routine
  DC_Insertion,     //< This is a don't care action needed for the print routine
  DC_Deletion       //< This is a don't care action needed for the print routine
};


//! \brief The cell of the matrix used for the alignment procedure
struct AffineCell {
  ssize_t Match;
  ssize_t Insertion;
  ssize_t Deletion;
};

//! \brief The Matrix used for the dynamic programming table
//! \tparam Cell The type of the cell in the table.
template <typename Cell>
class Matrix {
 public:
  using CellTy = Cell;

  //! \brief Constructor
  //! \param rows Number of rows
  //! \param columns Number of columns
  Matrix(size_t rows, size_t columns)
      : rows_(rows), columns_(columns) {
    matrix_ = new CellTy *[rows_];
    if (rows_) {
      matrix_[0] = new CellTy[rows_ * columns_];
      for (size_t i = 1; i < rows_; ++i)
        matrix_[i] = matrix_[0] + i * columns_;
    }
  }

  //! \brief Destructor
  ~Matrix() {
    if (rows_) delete[] matrix_[0];
    delete [] matrix_;
  }

  //! \brief Return the number of rows of the matrix
  //! \return the number of rows of the matrix
  size_t rows() const { return rows_; }

  //! \brief Return the number of columns of the matrix
  //! \return the number of columns of the matrix
  size_t columns() const { return columns_; }

  //! \brief Get the cell (i,j) of the matrix
  CellTy & operator()(size_t row, size_t column) {
    return matrix_[row][column];
  }

  //! \brief Get the cell (i,j) of the matrix
  CellTy & operator()(size_t row, size_t column) const {
    return matrix_[row][column];
  }

 private:
  size_t rows_;
  size_t columns_;
  CellTy ** matrix_;
};

struct global_alignment_tag {};
struct local_alignment_tag {};

//! \brief The score table used by the affine score function
struct ScoreTable {
  ssize_t Match;
  ssize_t Mismatch;
  ssize_t H;
  ssize_t G;
};

//! \brief The Algorithm Trait of the Sequence alignment
template <typename T>
class AlignmentAlgorithmTrait {
 public:
  using CellTy   = ssize_t;
  using MatrixTy = Matrix<CellTy>;

  //! \brief Inizialization step
  static void InitializeMatrix(const ScoreTable & S, const MatrixTy & M);

  //! \brief The function computing the score
  static void ComputeScore(
      const ScoreTable & S,
      const MatrixTy & M, const size_t i, const size_t j,
      const char s1, const char s2);

  //! \brief retrieve the content of the final cell.
  static std::tuple<size_t, size_t, ssize_t, Action>
  GetFinalScoreAndAction(
      const MatrixTy & M, const Sequence & s1, const Sequence& s2);

  //! \brief condition to stop the traceback rutine
  static bool
  TraceBackStopCondition(const MatrixTy & M, const size_t i, const size_t j);
};


//! \brief Specialization of the trait for the local alignment
template <>
class AlignmentAlgorithmTrait<local_alignment_tag> {
 public:
  using CellTy   = AffineCell;
  using MatrixTy = Matrix<CellTy>;

  static void InitializeMatrix(const ScoreTable & S, const MatrixTy & M) {
    M(0,0).Match     = 0;
    M(0,0).Insertion = std::numeric_limits<ssize_t>::min();
    M(0,0).Deletion  = std::numeric_limits<ssize_t>::min();

    for (size_t i = 1; i < M.rows(); ++i) {
      M(i, 0).Match     = std::numeric_limits<ssize_t>::min();
      M(i, 0).Deletion  = 0;
      M(i, 0).Insertion = std::numeric_limits<ssize_t>::min();
    }
    for (size_t j = 1; j < M.columns(); ++j) {
      M(0, j).Match     = std::numeric_limits<ssize_t>::min();
      M(0, j).Deletion  = std::numeric_limits<ssize_t>::min();
      M(0, j).Insertion = 0;
    }
  }

  static void ComputeScore(
      const ScoreTable & S,
      const MatrixTy & M, const size_t i, const size_t j,
      const char s1, const char s2) {
    ssize_t matchOrMismatch = s1 == s2 ? S.Match : S.Mismatch;
    M(i,j).Match =
        std::max(ssize_t(0),
                 std::max(M(i - 1, j - 1).Match,
                          std::max(M(i - 1, j - 1).Deletion,
                                   M(i - 1, j - 1).Insertion))
                 + matchOrMismatch);

    auto getNewScore = [](ssize_t current, ssize_t delta) -> ssize_t {
      if (current == std::numeric_limits<ssize_t>::min())
        return current;

      return current + delta;
    };

    M(i,j).Deletion =
        std::max(getNewScore(M(i - 1, j).Match, S.G + S.H),
                 std::max(
                     getNewScore(M(i - 1, j).Deletion, S.G),
                     getNewScore(M(i - 1, j).Insertion, S.G + S.H)));

    M(i,j).Insertion =
        std::max(getNewScore(M(i, j - 1).Match, S.G + S.H),
                 std::max(
                     getNewScore(M(i, j - 1).Deletion, S.G + S.H),
                     getNewScore(M(i, j - 1).Insertion, S.G)));
  }

  static std::tuple<size_t, size_t, ssize_t, Action>
  GetFinalScoreAndAction(
      const MatrixTy & M, const Sequence & s1, const Sequence& s2) {
    ssize_t maxScore = 0;
    size_t maxI, maxJ;
    for (size_t i = 0; i < M.rows(); ++i) {
      for (size_t j = 0; j < M.columns(); ++j) {
        if (M(i,j).Match > maxScore) {
          maxI = i;
          maxJ = j;
          maxScore = M(i,j).Match;
        }
      }
    }
    return std::make_tuple(maxI, maxJ, maxScore, Action::Match);
  }  

  static bool
  TraceBackStopCondition(const MatrixTy & M, const size_t i, const size_t j) {
    return i == 0 || j == 0 || M(i,j).Match == 0;
  }
};


//! \brief Specialization of the trait for the global alignment
template <>
class AlignmentAlgorithmTrait<global_alignment_tag> {
 public:
  using CellTy   = AffineCell;
  using MatrixTy = Matrix<CellTy>;

  static void InitializeMatrix(const ScoreTable & S, const MatrixTy & M) {
    M(0,0).Match     = 0;
    M(0,0).Insertion = std::numeric_limits<ssize_t>::min();
    M(0,0).Deletion  = std::numeric_limits<ssize_t>::min();

    for (size_t i = 1; i < M.rows(); ++i) {
      M(i, 0).Match     = std::numeric_limits<ssize_t>::min();
      M(i, 0).Deletion  = S.H + i * S.G;
      M(i, 0).Insertion = std::numeric_limits<ssize_t>::min();
    }
    for (size_t j = 1; j < M.columns(); ++j) {
      M(0, j).Match     = std::numeric_limits<ssize_t>::min();
      M(0, j).Deletion  = std::numeric_limits<ssize_t>::min();
      M(0, j).Insertion =  S.H + j * S.G;
    }
  }

  static void ComputeScore(
      const ScoreTable & S, const MatrixTy & M, const size_t i, const size_t j,
      const char s1, const char s2) {
    ssize_t matchOrMismatch = s1 == s2 ? S.Match : S.Mismatch;
    M(i,j).Match =
        std::max(M(i - 1, j - 1).Match,
                 std::max(M(i - 1, j - 1).Deletion, M(i - 1, j - 1).Insertion))
        + matchOrMismatch;

    auto getNewScore = [](ssize_t current, ssize_t delta) -> ssize_t {
      if (current == std::numeric_limits<ssize_t>::min())
        return current;

      return current + delta;
    };

    M(i,j).Deletion =
        std::max(getNewScore(M(i - 1, j).Match, S.G + S.H),
                 std::max(
                     getNewScore(M(i - 1, j).Deletion, S.G),
                     getNewScore(M(i - 1, j).Insertion, S.G + S.H)));

    M(i,j).Insertion =
        std::max(getNewScore(M(i, j - 1).Match, S.G + S.H),
                 std::max(
                     getNewScore(M(i, j - 1).Deletion, S.G + S.H),
                     getNewScore(M(i, j - 1).Insertion, S.G)));
  }

  static std::tuple<size_t, size_t, ssize_t, Action>
  GetFinalScoreAndAction(
      const MatrixTy & M, const Sequence & s1, const Sequence& s2) {
    auto cell = M(M.rows() - 1, M.columns() - 1);
    Action action =
        s1[s1.length() - 1] == s2[s2.length() - 1]
        ? Action::Match : Action::Mismatch;

    ssize_t score = cell.Match;

    if (score < cell.Deletion) {
      score = cell.Deletion;
      action = Action::Deletion;
    }

    if (score < cell.Insertion) {
      score = cell.Insertion;
      action = Action::Insertion;
    }

    return std::make_tuple(M.rows() - 1, M.columns() - 1, score, action);
  }

  static bool
  TraceBackStopCondition(const MatrixTy &, const size_t i, const size_t j) {
    return (i == 0 && j == 0);
  }
};

template <typename algorithm_tag>
std::tuple<std::deque<Action>, size_t, size_t>
TraceBackActions(
    const typename AlignmentAlgorithmTrait<algorithm_tag>::MatrixTy & M,
    size_t i, size_t j, Action action, ssize_t score,
    const ScoreTable & S, const Sequence & s1, const Sequence & s2) {
  using AlgorithmTrait = AlignmentAlgorithmTrait<algorithm_tag>;

  std::deque<Action> actionStack;
  actionStack.push_back(action);

  switch (action) {
    case Action::Match:
    case Action::Mismatch:
      --i; --j;
      break;
    case Action::Insertion:
      --j;
      break;
    case Action::Deletion:
      --i;
      break;
    default:
      break;
  }

  while (!AlgorithmTrait::TraceBackStopCondition(M, i, j)) {
    switch(action) {
      case Action::Match:
        if (M(i,j).Match + S.Match == score) {
          action = s1[i - 1] == s2[j - 1] ? Action::Match : Action::Mismatch;
          score  = M(i,j).Match;
          --i; --j;
        } else if (M(i,j).Insertion + S.Match == score) {
          action = Action::Insertion;
          score  = M(i,j).Insertion;
          --j;
        } else if (M(i,j).Deletion + S.Match == score) {
          action = Action::Deletion;
          score  = M(i,j).Deletion;
          --i;
        } else {
          exit(-1);
        }
        break;
      case Action::Mismatch:
        if (M(i,j).Match + S.Mismatch == score) {
          action = s1[i - 1] == s2[j - 1] ? Action::Match : Action::Mismatch;
          score  = M(i,j).Match;
          --i; --j;
        } else if (M(i,j).Insertion + S.Mismatch == score) {
          action = Action::Insertion;
          score  = M(i,j).Insertion;
          --j;
        } else if (M(i,j).Deletion + S.Mismatch == score) {
          action = Action::Deletion;
          score  = M(i,j).Deletion;
          --i;
        } else {
          exit(-2);
        }
        break;
      case Action::Insertion:
        if (M(i,j).Match + S.H + S.G == score) {
          action = s1[i - 1] == s2[j - 1] ? Action::Match : Action::Mismatch;
          score  = M(i,j).Match;
          --i; --j;
        } else if (M(i,j).Insertion + S.G == score) {
          action = Action::Insertion;
          score  = M(i,j).Insertion;
          --j;
        } else if (M(i,j).Deletion + S.H + S.G == score) {
          action = Action::Deletion;
          score  = M(i,j).Deletion;
          --i;
        } else {
          exit(-3);
        }
        break;
      case Action::Deletion:
        if (M(i,j).Match + S.H + S.G == score) {
          action = s1[i - 1] == s2[j - 1] ? Action::Match : Action::Mismatch;
          score  = M(i,j).Match;
          --i; --j;
        } else if (M(i,j).Insertion + S.H + S.G == score) {
          action = Action::Insertion;
          score  = M(i,j).Insertion;
          --j;
        } else if (M(i,j).Deletion + S.G == score) {
          action = Action::Deletion;
          score  = M(i,j).Deletion;
          --i;
        } else {
          exit(-4);
        }
        break;
      default:
        break;
    }

    actionStack.push_back(action);
  }

  return std::make_tuple(actionStack, i, j);
}

//! \brief The alignment procedure.
//!
//! \param s1 The first sequence
//! \param s2 The second sequence
//! \param S  The score table
//! \returns The list of action needed to alingn s1 to s2
template <typename algorithm_tag>
std::deque<Action>
Alignment(const Sequence &s1, const Sequence &s2,
          ScoreTable &S, const algorithm_tag &) {
  // Initialize the matrix
  using AlgorithmTrait = AlignmentAlgorithmTrait<algorithm_tag>;
  using Matrix = typename AlgorithmTrait::MatrixTy;

  Matrix M(s1.length() + 1, s2.length() + 1);
  AlgorithmTrait::InitializeMatrix(S, M);

  // Fill in the matrix top to bottom, left to right
  for (size_t i = 1; i < M.rows(); ++i) {
    for (size_t j = 1; j < M.columns(); ++j) {
      AlgorithmTrait::ComputeScore(S, M, i, j, s1[i-1], s2[j-1]);
    }
  }

  size_t startI, startJ, endI, endJ;
  ssize_t score;
  Action action;
  std::tie(startI, startJ, score, action) =
      AlgorithmTrait::GetFinalScoreAndAction(M, s1, s2);

  std::deque<Action> actionStack;

  std::tie(actionStack, endI, endJ) =
      TraceBackActions<algorithm_tag>(
          M, startI, startJ, action, score, S, s1, s2);

  if (startI != M.rows() - 1 || startJ != M.columns() - 1) {
    size_t diagonal = std::min(s1.length() - startI, s2.length() - startJ);
    actionStack.insert(
        std::begin(actionStack),
        diagonal,
        Action::DC_MatchMismatch);
    if (s1.length() - startI > s2.length() - startJ) {
      actionStack.insert(
          std::begin(actionStack),
          (s1.length() - startI) - (s2.length() - startJ), Action::DC_Deletion);
    } else if (s1.length() < s2.length()) {
      actionStack.insert(
          std::begin(actionStack),
          (s2.length() - startJ) - (s1.length() - startI), Action::DC_Insertion);
    }
  }

  if (endI != 0 || endJ != 0) {
    actionStack.insert(
        std::end(actionStack),
        std::min(endI, endJ), Action::DC_MatchMismatch);
    if (endI > endJ) {
      actionStack.insert(
          std::end(actionStack),
          endI - endJ, Action::DC_Deletion);
    } else if (endJ > endI) {
      actionStack.insert(
          std::end(actionStack),
          endJ - endI, Action::DC_Insertion);
    }
  }

  return actionStack;
}

}

#endif  // _ALIGNMENT_H_
