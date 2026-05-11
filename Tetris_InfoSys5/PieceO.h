#pragma once
#include "Piece.h"

// Khối O (Vuông 2x2) - Không cần xoay vì đối xứng hoàn toàn
class PieceO : public Piece {
public:
    PieceO() {
        // Khởi tạo ma trận shape rỗng
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        // Đặt hình vuông 2x2 ở vị trí trung tâm
        shape[1][1] = 'O';
        shape[1][2] = 'O';
        shape[2][1] = 'O';
        shape[2][2] = 'O';
    }

    // Khối O đối xứng hoàn toàn => không cần xoay, tiết kiệm tài nguyên
    void rotate() override {
        // Không thực hiện gì
    }
};
