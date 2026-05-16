#include <iostream>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include "Blocks.h"

using namespace std;

// --- ĐỊNH NGHĨA KÍCH THƯỚC UI ---
const int W = 12;
const int H = 22;
const int OFFSET_X = 2;
const int OFFSET_Y = 1;

char board[H][W] = {};
Piece *currentPiece = nullptr;

int score = 0;
int level = 1;
int linesCleared = 0;
int speed = 1000;

enum TetrisColor {
    COLOR_BLACK = 0, COLOR_CYAN = 11, COLOR_YELLOW = 14,
    COLOR_PURPLE = 13, COLOR_GREEN = 10, COLOR_RED = 12,
    COLOR_BLUE = 9, COLOR_ORANGE = 6, COLOR_WHITE = 15, COLOR_GRAY = 8
};

//KHỞI TẠO HỆ THỐNG DOUBLE BUFFERING BẰNG WINDOWS API
//Khai báo mảng đệm (Buffer) và các hàm công cụ thay thế cho gotoxy/cout.
const int SCREEN_WIDTH = 60;
const int SCREEN_HEIGHT = 26;
CHAR_INFO screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void clearBuffer() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screenBuffer[i].Char.AsciiChar = ' ';
        screenBuffer[i].Attributes = COLOR_BLACK;
    }
}

void drawCharToBuffer(int x, int y, char c, WORD color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        screenBuffer[y * SCREEN_WIDTH + x].Char.AsciiChar = c;
        screenBuffer[y * SCREEN_WIDTH + x].Attributes = color;
    }
}

void writeStringToBuffer(int x, int y, string str, WORD color) {
    for (size_t i = 0; i < str.length(); i++) {
        drawCharToBuffer(x + i, y, str[i], color);
    }
}

void renderScreen() {
    COORD bufferSize = { (SHORT)SCREEN_WIDTH, (SHORT)SCREEN_HEIGHT };
    COORD characterPos = { 0, 0 };
    SMALL_RECT writeArea = { 0, 0, (SHORT)(SCREEN_WIDTH - 1), (SHORT)(SCREEN_HEIGHT - 1) };
    WriteConsoleOutputA(hConsole, screenBuffer, bufferSize, characterPos, &writeArea);
}


int getPieceColor(char pType) {
    switch (pType) {
        case 'I': return COLOR_CYAN;   case 'O': return COLOR_YELLOW;
        case 'T': return COLOR_PURPLE; case 'S': return COLOR_GREEN;
        case 'Z': return COLOR_RED;    case 'J': return COLOR_BLUE;
        case 'L': return COLOR_ORANGE; case '#': return COLOR_WHITE;
        default: return COLOR_WHITE;
    }
}

// Kiểm tra va chạm khi xoay (Cực kỳ quan trọng)
bool canRotate() {
    char temp[4][4];
    // Thuật toán xoay thử ma trận 4x4
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[i][j] = currentPiece->getShape(3 - j, i);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (temp[i][j] != ' ') {
                int tx = currentPiece->getX() + j;
                int ty = currentPiece->getY() + i;
                // Kiểm tra biên theo tọa độ UI mới
                if (tx < 0 || tx >= W || ty >= H || ty < 0) return false;
                if (board[ty][tx] != ' ') return false;
            }
        }
    }
    return true;
}

void setup() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(out, &cursorInfo);

    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            board[i][j] = ' ';
    score = 0; level = 1; speed = 1000; linesCleared = 0;
    srand((unsigned int)time(0));
}

// CHUYỂN GIAO DIỆN TĨNH SANG BỘ ĐỆM (BUFFER)
// Ý nghĩa: Sửa hàm drawBlock (kết hợp màu nền), vẽ khung và thông số để nạp vào Buffer
void drawBlock(int boardX, int boardY, char type) {
    int sx = OFFSET_X + boardX * 2;
    int sy = OFFSET_Y + boardY;
    if (type == ' ') {
        WORD attr = (COLOR_BLACK << 4) | COLOR_GRAY; // Nền đen, chữ xám
        drawCharToBuffer(sx, sy, '.', attr);
        drawCharToBuffer(sx + 1, sy, ' ', attr);
    } else {
        int color = getPieceColor(type);
        WORD attr = (color << 4) | COLOR_BLACK;      // Nền màu gạch, chữ đen
        drawCharToBuffer(sx, sy, ' ', attr);
        drawCharToBuffer(sx + 1, sy, ' ', attr);
    }
}

void drawOuterFrame() {
    WORD frameColor = (COLOR_BLACK << 4) | COLOR_WHITE;

    drawCharToBuffer(OFFSET_X - 1, OFFSET_Y - 1, (char)201, frameColor);
    for (int i = 0; i < W * 2; i++) drawCharToBuffer(OFFSET_X + i, OFFSET_Y - 1, (char)205, frameColor);
    drawCharToBuffer(OFFSET_X + W * 2, OFFSET_Y - 1, (char)187, frameColor);
    for (int i = 0; i < H; i++) {
        drawCharToBuffer(OFFSET_X - 1, OFFSET_Y + i, (char)186, frameColor);
        for (int j = 0; j < W; j++) {
            drawBlock(j, i, board[i][j]);
        }
        drawCharToBuffer(OFFSET_X + W * 2, OFFSET_Y + i, (char)186, frameColor);
    }
    drawCharToBuffer(OFFSET_X - 1, OFFSET_Y + H, (char)200, frameColor);
    for (int i = 0; i < W * 2; i++) drawCharToBuffer(OFFSET_X + i, OFFSET_Y + H, (char)205, frameColor);
    drawCharToBuffer(OFFSET_X + W * 2, OFFSET_Y + H, (char)188, frameColor);
}

void drawStats() {
    int statsX = OFFSET_X + W * 2 + 4;
    int statsY = OFFSET_Y + 1;
    writeStringToBuffer(statsX, statsY - 1, "== TETRIS INFOSYS 5 ==", (COLOR_BLACK << 4) | COLOR_CYAN);
    writeStringToBuffer(statsX, statsY + 1, "SCORE: " + to_string(score), (COLOR_BLACK << 4) | COLOR_WHITE);
    writeStringToBuffer(statsX, statsY + 2, "LEVEL: " + to_string(level), (COLOR_BLACK << 4) | COLOR_WHITE);
}

// CHUYỂN KHỐI GẠCH RƠI SANG BUFFER VÀ XÓA HÀM DƯ THỪA
// Ý nghĩa: Cập nhật hàm drawCurrentPiece. Xóa bỏ các hàm clearCurrentPiece() và updateBoardUI() cũ.
void drawCurrentPiece() {
    if (!currentPiece) return;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getShape(i, j) != ' ') {
                int px = currentPiece->getX() + j;
                int py = currentPiece->getY() + i;
                if (py >= 0 && py < H && px >= 0 && px < W)
                    drawBlock(px, py, currentPiece->getShape(i,j));
            }
        }
    }
}

bool canMove(int dx, int dy) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getShape(i, j) != ' ') {
                int tx = currentPiece->getX() + j + dx;
                int ty = currentPiece->getY() + i + dy;
                if (tx < 0 || tx >= W || ty >= H || ty < 0) return false;
                if (board[ty][tx] != ' ') return false;
            }
        }
    }
    return true;
}

void block2Board() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getShape(i, j) != ' ') {
                if (currentPiece->getY() + i >= 0)
                    board[currentPiece->getY() + i][currentPiece->getX() + j] = currentPiece->getShape(i,j);
            }
        }
    }
}



void checkLines() {
    int combo = 0;
    for (int i = H - 1; i >= 0; i--) {
        bool full = true;
        for (int j = 0; j < W; j++) {
            if (board[i][j] == ' ') { full = false; break; }
        }
        if (full) {
            combo++;
            for (int k = i; k > 0; k--)
                for (int j = 0; j < W; j++)
                    board[k][j] = board[k - 1][j];
            for (int j = 0; j < W; j++) board[0][j] = ' ';
            i++;
        }
    }
    if (combo > 0) {
        score += combo * 100 * level;
        linesCleared += combo;
        level = (linesCleared / 10) + 1;
        speed = max(100, 1000 - (level - 1) * 100);
    }
}

int main() {
    system("mode con cols=60 lines=26");
    SetConsoleTitleA("Tetris InfoSys5 - Reworked UI");
    setup();
    currentPiece = createRandomPiece();

    clock_t start = clock();
    while (1) {
        if (_kbhit()) {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0)) currentPiece->moveLeft();
            if (c == 'd' && canMove(1, 0)) currentPiece->moveRight();
            if (c == 'w') {
                if (canRotate()) currentPiece->rotate();
                else {
                    currentPiece->moveRight();
                    if (canRotate()) currentPiece->rotate();
                    else {
                        currentPiece->moveRight();
                        if (canRotate()) currentPiece->rotate();
                        else {
                            currentPiece->moveLeft(); currentPiece->moveLeft(); currentPiece->moveLeft();
                            if (canRotate()) currentPiece->rotate();
                            else {
                                currentPiece->moveLeft();
                                if (canRotate()) currentPiece->rotate();
                                else {
                                    currentPiece->moveRight(); currentPiece->moveRight();
                                }
                            }
                        }
                    }
                }
            }
            if (c == 's' && canMove(0, 1)) currentPiece->moveDown();
            if (c == 'q') break;
        }

        if (clock() - start > speed) {
            if (canMove(0, 1)) {
                currentPiece->moveDown();
                drawCurrentPiece();
            } else {
                drawCurrentPiece();
                block2Board();
                checkLines();
                delete currentPiece;
                currentPiece = createRandomPiece();

                if (!canMove(0, 0)) {
                    // Xử lý đồ họa kết thúc Game
                    clearBuffer();
                    drawOuterFrame();
                    drawCurrentPiece();
                    writeStringToBuffer(OFFSET_X + W - 3, OFFSET_Y + H / 2, " GAME OVER! ", (COLOR_BLACK << 4) | COLOR_RED);
                    renderScreen();
                    _getch(); // Đợi nhấn phím rồi mới thoát
                    break;
                }
            }
            start = clock();
        }
        // CHỈ GỌI VẼ BẰNG NHỮNG HÀM CỦA BUFFER
        clearBuffer();
        drawOuterFrame();
        drawStats();
        drawCurrentPiece();
        renderScreen(); // Tung toàn bộ bản nháp ra màn hình trong 1 nhịp
        Sleep(10); // Cho CPU nghỉ
    }
    delete currentPiece;
    currentPiece = nullptr;
    return 0;
}
