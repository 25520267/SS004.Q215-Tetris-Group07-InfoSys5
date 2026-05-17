#pragma once
#include "Piece.h"
#include <iostream>

// Khai báo extern để lấy trực tiếp các hàm vẽ UI từ main.cpp sang mà không bị lỗi Linker
extern void gotoxy(int x, int y);
extern void setColor(int foreground, int background);

class TetrisFeatures {
public:
    // 1. Kiểm tra va chạm ảo (Tìm xem bóng mờ có bị lọt ra ngoài hoặc đè lên khối khác không)
    static bool canMoveToY(Piece* p, int testY, char board[22][12]) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (p->getShape(i, j) != ' ') {
                    int tx = p->getX() + j;
                    int ty = testY + i;
                    // Kích thước UI chuẩn từ main: W = 12, H = 22
                    if (tx < 0 || tx >= 12 || ty >= 22 || ty < 0) return false;
                    if (board[ty][tx] != ' ') return false;
                }
            }
        }
        return true;
    }

    // 2. Quét từ vị trí hiện tại xuống dưới cùng để lấy tọa độ Y của Bóng mờ
    static int getGhostY(Piece* p, char board[22][12]) {
        int ghostY = p->getY();
        while (canMoveToY(p, ghostY + 1, board)) {
            ghostY++;
        }
        return ghostY;
    }

    // 3. Hàm vẽ Bóng mờ (Ghost Piece)
    static void drawGhost(Piece* p, int ghostY, int offsetX, int offsetY) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (p->getShape(i, j) != ' ') {
                    int px = p->getX() + j;
                    int py = ghostY + i;
                    if (py >= 0 && py < 22 && px >= 0 && px < 12) {
                        // Chỉ vẽ bóng mờ ở phía dưới khối thật để tránh đè màu
                        if (py >= p->getY() + i) { 
                            gotoxy(offsetX + px * 2, offsetY + py);
                            setColor(8, 0); // Mã 8 là COLOR_GRAY, 0 là nền đen
                            std::cout << "[]"; // Dùng dấu ngoặc vuông để tạo hình bóng mờ rỗng
                        }
                    }
                }
            }
        }
    }

    // 4. Hàm xóa Bóng mờ cũ khi khối di chuyển sang vị trí mới
    static void clearGhost(Piece* p, int ghostY, int offsetX, int offsetY) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (p->getShape(i, j) != ' ') {
                    int px = p->getX() + j;
                    int py = ghostY + i;
                    if (py >= 0 && py < 22 && px >= 0 && px < 12) {
                        gotoxy(offsetX + px * 2, offsetY + py);
                        setColor(8, 0); 
                        std::cout << ". "; // Trả lại nền lưới mặc định
                    }
                }
            }
        }
    }

    // 5. Hàm Hard Drop (Lệnh ném khối xuống thẳng đáy)
    static void hardDrop(Piece* p, char board[22][12]) {
        std::cout << '\a'; // Phát tiếng kêu bíp của hệ thống mà không cần dùng Sleep
        int bottomY = getGhostY(p, board);
        // Do x, y ở class Piece để mức protected, ta dùng hàm moveDown() để đẩy y xuống đáy
        while (p->getY() < bottomY) {
            p->moveDown();
        }
    }
};