#pragma once
#include "Piece.h"
#include <cstdlib>
#include <algorithm>

inline void rotateMatrix(char shape[4][4]) {
    char temp[4][4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp[j][3 - i] = shape[i][j];
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            shape[i][j] = temp[i][j];
        }
    }
}

// Khối T
class PieceT : public Piece {
public:
    PieceT() {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                shape[i][j] = ' ';
            }
        }

        shape[1][1] = 'T';
        shape[2][0] = 'T';
        shape[2][1] = 'T';
        shape[2][2] = 'T';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};
// Khối L
class PieceL : public Piece {
public:
    PieceL() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[0][1] = 'L';
        shape[1][1] = 'L';
        shape[2][1] = 'L';
        shape[2][2] = 'L';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};

// Khối J
class PieceJ : public Piece {
public:
    PieceJ() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[0][1] = 'J';
        shape[1][1] = 'J';
        shape[2][1] = 'J';
        shape[2][0] = 'J';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};

// Khối I
class PieceI : public Piece {
public:
    PieceI() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[1][0] = 'I';
        shape[1][1] = 'I';
        shape[1][2] = 'I';
        shape[1][3] = 'I';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};

// Khối O
class PieceO : public Piece {
public:
    PieceO() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[1][1] = 'O';
        shape[1][2] = 'O';
        shape[2][1] = 'O';
        shape[2][2] = 'O';
    }

    void rotate() override {
        // Khối O xoay không đổi
    }
};

// Khối S
class PieceS : public Piece {
public:
    PieceS() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[1][1] = 'S';
        shape[1][2] = 'S';
        shape[2][0] = 'S';
        shape[2][1] = 'S';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};

// Khối Z
class PieceZ : public Piece {
public:
    PieceZ() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';

        shape[1][0] = 'Z';
        shape[1][1] = 'Z';
        shape[2][1] = 'Z';
        shape[2][2] = 'Z';
    }

    void rotate() override {
        rotateMatrix(shape);
    }
};
// --- Hệ thống Túi 7 khối (7-bag Randomizer) ---
// Xáo trộn 7 loại khối, nhả ra từng cái một.
// Khi hết túi thì xáo trộn lại, tránh ra cùng 1 loại liên tiếp nhiều lần.

inline Piece* createPieceByType(int type) {
    switch (type) {
    case 0: return new PieceT();
    case 1: return new PieceL();
    case 2: return new PieceJ();
    case 3: return new PieceI();
    case 4: return new PieceO();
    case 5: return new PieceS();
    case 6: return new PieceZ();
    default: return new PieceT();
    }
}

inline Piece* getNextPieceFromBag() {
    static int bag[7] = { 0, 1, 2, 3, 4, 5, 6 };
    static int bagIndex = 7; // Bắt đầu = 7 để xáo trộn ngay lần đầu

    if (bagIndex >= 7) {
        // Xáo trộn túi bằng Fisher-Yates (std::random_shuffle)
        std::random_shuffle(bag, bag + 7);
        bagIndex = 0;
    }

    return createPieceByType(bag[bagIndex++]);
}