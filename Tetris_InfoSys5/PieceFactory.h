#pragma once
#include <cstdlib>
#include "Piece.h"
#include "Blocks.h"
#include "PieceO.h"
#include "PieceI.h"

// Hàm factory tạo khối ngẫu nhiên, trả về con trỏ đa hình Piece*
// Gọi srand() trước khi sử dụng hàm này
inline Piece* generateRandomPiece() {
    // Số loại khối hiện có: T, O, I
    const int TOTAL_PIECES = 3;
    int nRandom = rand() % TOTAL_PIECES;

    switch (nRandom) {
    case 0:
        return new PieceT();
    case 1:
        return new PieceO();
    case 2:
        return new PieceI();
    default:
        return new PieceT();
    }
}
