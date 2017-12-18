//
// Created by odownard on 02/11/17.
//

#ifndef GREATERTHANGAME_GREATERTHANGAME_H
#define GREATERTHANGAME_GREATERTHANGAME_H

#include <cstdint>

namespace GTG {
class GreaterThanPlayer {
public:
  uint16_t GetRepr() { return value; }
  void mutate();

private:
  uint16_t value;
};
class GreaterThanGame {
public:
  uint8_t PlayTwo(GreaterThanPlayer p1, GreaterThanPlayer p2);
};
} // namespace GTG

#endif // GREATERTHANGAME_GREATERTHANGAME_H
