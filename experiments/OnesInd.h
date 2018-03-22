//
// Created by Oliver Downard on 14/02/2018.
//

#pragma once

template <size_t size, size_t Dimensions>
class OnesIndRep : public IndividualRep {
public:
  OnesIndRep() : bits{} {
    for (auto &a : bits) {
      a.resize(size);
    }
  }

  std::string name() override { return "Ones"; }

  size_t getNumOnes(size_t dim) const {
    uint cnt = 0;
    for (const auto b : bits[dim]) {
      if (b) {
        cnt += 1;
      }
    }
    return cnt;
  }

  OnesIndRep(std::array<std::vector<bool>, Dimensions> a) : bits{a} {
    for (size_t i = 0; i < Dimensions; ++i) {
      assert(a[i].size() == size);
    }
  }

  OnesIndRep(OnesIndRep &other) = default;

  constexpr size_t getSize() const { return size; }

  bool flipBit(size_t i) {
    bits[i] = ~bits[i];
    return bits[i];
  }

  std::unique_ptr<IndividualRep> copy() override {
    return std::make_unique<OnesIndRep>(bits);
  }

  std::string toString() const override {
    std::stringstream ss;
    for (auto dit = std::cbegin(bits); dit != std::cend(bits); ++dit) {
      if (dit != std::cbegin(bits)) {
        ss << " ";
      }
      for (const auto &bit : *dit) {
        ss << bit;
      }
    }
    return ss.str();
  }

  /**
   * [0, 1, 2, 3, 4]
   *
   * Slice(1, 4) returns [1, 2, 3]
   *
   * @param front The start of the slice, inclusive
   * @param back One past the end of the slice
   * @return
   */
  std::vector<bool> getSlice(size_t dim, size_t front, size_t back) {
    assert(back <= bits[dim].size());
    assert(front <= back);
    std::vector<bool> ret;
    ret.reserve(back - front);
    for (size_t i = front; i < back; ++i) {
      ret.emplace_back(bits[dim][i]);
    }
    return ret;
  }

private:
  std::array<std::vector<bool>, Dimensions> bits;
};
