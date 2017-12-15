//
// Created by odownard on 02/11/17.
//
#include "include/GreaterThanGame.h"

void GTG::GreaterThanPlayer::mutate() {
}

uint8_t GTG::GreaterThanGame::PlayTwo(GreaterThanPlayer p1,
                                      GreaterThanPlayer p2) {
    if (p1.GetRepr() > p2.GetRepr()) {
        return 0;
    }
    return 1;
}