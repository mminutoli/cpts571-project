#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <string>
#include <ostream>

namespace cpts571 {

class Sequence {
 public:
  using iterator = std::string::iterator;
  using const_iterator = std::string::const_iterator;

  void Name(const std::string & n) { sequenceName_ = n; }
  std::string Name() const { return sequenceName_; }

  template <typename Iterator>
  void AppendChunk(const Iterator B, const Iterator E) {
    sequence_.insert(std::end(sequence_), B, E);
  }

  iterator begin() { return sequence_.begin(); }
  const_iterator begin() const { return sequence_.begin(); }

  iterator end() { return sequence_.end(); }
  const_iterator end() const { return sequence_.end(); }

  friend std::ostream & operator<<(std::ostream & O, const Sequence & S) {
    O << S.Name() << ": "
      << S.sequence_;
  return O;
  }

 private:
  std::string sequenceName_;
  std::string sequence_;
};

}

#endif /* SEQUENCE_H */
