#pragma once
#include <iostream>
#include "Blocks.h"

inline Piece* holdPiece = nullptr;
inline bool canHold = true;

inline void holdCurrentPiece(Piece*& currentPiece, Piece*& nextPiece) {
    if (!canHold || currentPiece == nullptr) return;

    if (holdPiece == nullptr) {
        holdPiece = currentPiece;
        currentPiece = nextPiece;
        nextPiece = getNextPieceFromBag();
    }
    else {
        Piece* temp = currentPiece;
        currentPiece = holdPiece;
        holdPiece = temp;
    }

    currentPiece->setPosition(5, 1);
    canHold = false;
}

inline void resetHoldPiece() {
    canHold = true;
}

inline void drawHoldPiece() {
    std::cout << "\nHOLD:\n";

    if (holdPiece == nullptr) {
        std::cout << "[EMPTY]\n";
        return;
    }

    for (int i = 0; i < 4; i++) {
        std::cout << " ";
        for (int j = 0; j < 4; j++) {
            char c = holdPiece->getShape(i, j);
            std::cout << (c == ' ' ? '.' : c);
        }
        std::cout << std::endl;
    }
}