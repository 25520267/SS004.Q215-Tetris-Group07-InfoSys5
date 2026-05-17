#pragma once

class Piece {
protected:
    int x, y;
    char shape[4][4];

public:
    Piece() {
        x = 5; 
        y = 1;
    }
    virtual ~Piece() {}

    virtual void rotate() = 0; 

    int getX() { return x; }
    int getY() { return y; }
    char getShape(int i, int j) { return shape[i][j]; }
    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }
    void moveLeft() { x--; }
    void moveRight() { x++; }
    void moveDown() { y++; }
};