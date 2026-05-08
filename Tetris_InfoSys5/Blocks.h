#pragma once
#include "Piece.h"

// Khối T dùng để test tạm thời
class PieceT : public Piece {
public:
    PieceT() {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                shape[i][j] = ' ';
        shape[1][1] = 'T'; shape[2][0] = 'T'; shape[2][1] = 'T'; shape[2][2] = 'T';
    }

    void rotate() override {
        // Viết tiếp hàm xoay
    }
};