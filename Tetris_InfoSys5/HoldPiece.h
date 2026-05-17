#pragma once
#include "Blocks.h"

inline Piece* holdPiece = nullptr;
inline bool canHold = true;

inline void holdCurrentPiece(Piece*& currentPiece, Piece*& nextPiece) {
    if (!canHold || currentPiece == nullptr) return;

    if (holdPiece == nullptr) {
        holdPiece = currentPiece;
        currentPiece = nextPiece;
        nextPiece = createRandomPiece();
    }
    else {
        Piece* temp = currentPiece;
        currentPiece = holdPiece;
        holdPiece = temp;
    }

    canHold = false;
}

inline void resetHoldPiece() {
    canHold = true;
}

inline void drawHoldPiece() {
    // Tạm để trống nếu chưa muốn vẽ UI Hold
}