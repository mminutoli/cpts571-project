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

enum class Action {
  Match,
  Mismatch,
  Insertion,
  Deletion
};

struct AffineCell {
  ssize_t Match;
  ssize_t Insertion;
  ssize_t Deletion;
};

template <typename Cell>
class Matrix {
 public:
  using CellTy = Cell;

  Matrix(size_t rows, size_t columns)
      : rows_(rows), columns_(columns) {
    matrix_ = new CellTy *[rows_];
    if (rows_) {
      matrix_[0] = new CellTy[rows_ * columns_];
      for (size_t i = 1; i < rows_; ++i)
        matrix_[i] = matrix_[0] + i * columns_;
    }
  }

  ~Matrix() {
    if (rows_) delete[] matrix_[0];
    delete [] matrix_;
  }

  size_t rows() const { return rows_; }
  size_t columns() const { return columns_; }

  CellTy & operator()(size_t row, size_t column) {
    return matrix_[row][column];
  }

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

struct ScoreTable {
  ssize_t Match;
  ssize_t Mismatch;
  ssize_t H;
  ssize_t G;
};

template <typename T>
class AlignmentAlgorithmTrait {
 public:
  using CellTy   = ssize_t;
  using MatrixTy = Matrix<CellTy>;

  static void InitializeMatrix(const ScoreTable & S, const MatrixTy & M);

  static void ComputeScore(
      const ScoreTable & S,
      const MatrixTy & M, const size_t i, const size_t j,
      const char s1, const char s2);

  static std::tuple<size_t, size_t, ssize_t, Action>
  GetFinalScoreAndAction(
      const MatrixTy & M, const Sequence & s1, const Sequence& s2);

  static bool
  TraceBackStopCondition(const MatrixTy & M, const size_t i, const size_t j);
};


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
std::deque<Action>
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
    }

    actionStack.push_back(action);
  }

  return actionStack;
}

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

  size_t i, j;
  ssize_t score;
  Action action;
  std::tie(i, j, score, action) =
      AlgorithmTrait::GetFinalScoreAndAction(M, s1, s2);

  std::deque<Action> actionStack =
      TraceBackActions<algorithm_tag>(M, i, j, action, score, S, s1, s2);

  return actionStack;
}

}

#endif  // _ALIGNMENT_H_
