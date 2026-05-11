#pragma once
#include "Piece.h"

// Khối I (Dài) - Chỉ có 2 trạng thái xoay: ngang và dọc
class PieceI : public Piece {
private:
    bool bIsVertical; // Trạng thái xoay: true = dọc, false = ngang

public:
    PieceI() : bIsVertical(false) {
        // Khởi tạo ma trận shape rỗng
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        // Trạng thái mặc định: ngang (hàng thứ 1)
        shape[1][0] = 'I';
        shape[1][1] = 'I';
        shape[1][2] = 'I';
        shape[1][3] = 'I';
    }

    // Lật trạng thái giữa ngang và dọc (chỉ 2 trạng thái, tối ưu hơn xoay ma trận)
    void rotate() override {
        // Xóa trạng thái cũ
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        if (bIsVertical) {
            // Chuyển từ dọc sang ngang
            shape[1][0] = 'I';
            shape[1][1] = 'I';
            shape[1][2] = 'I';
            shape[1][3] = 'I';
        }
        else {
            // Chuyển từ ngang sang dọc
            shape[0][1] = 'I';
            shape[1][1] = 'I';
            shape[2][1] = 'I';
            shape[3][1] = 'I';
        }

        // Đảo trạng thái
        bIsVertical = !bIsVertical;
    }
};
