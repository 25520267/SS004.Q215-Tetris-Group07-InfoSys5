#include <iostream>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include "Menu.h"
#include "Blocks.h"
#include "HoldPiece.h"
#include "ghost-and-harddrop.h"

using namespace std;

// --- ĐỊNH NGHĨA KÍCH THƯỚC UI ---
const int W = 12;
const int H = 22;
const int OFFSET_X = 2;
const int OFFSET_Y = 1;

char board[H][W] = {};
Piece *currentPiece = nullptr;
Piece* nextPiece = nullptr;

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

// --- Vẽ khung NEXT PIECE ở góc phải ---
void drawNextPiece() {
    if (!nextPiece) return;

    int boxX = OFFSET_X + W * 2 + 4; // Vị trí X của khung
    int boxY = OFFSET_Y + 5;          // Vị trí Y (bên dưới Stats)
    int boxW = 10;                     // Chiều rộng khung (5 ô * 2 ký tự)

    // Tiêu đề
    setColor(COLOR_CYAN, COLOR_BLACK);
    gotoxy(boxX, boxY); cout << "  NEXT";

    // Viền trên
    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(boxX, boxY + 1);
    cout << (char)218; for (int i = 0; i < boxW; i++) cout << (char)196; cout << (char)191;

    // Nội dung 4 dòng (vẽ shape của nextPiece)
    for (int i = 0; i < 4; i++) {
        gotoxy(boxX, boxY + 2 + i);
        setColor(COLOR_WHITE, COLOR_BLACK);
        cout << (char)179; // Viền trái
        for (int j = 0; j < 4; j++) {
            char c = nextPiece->getShape(i, j);
            if (c != ' ') {
                int color = getPieceColor(c);
                setColor(COLOR_BLACK, color); cout << "  ";
                setColor(COLOR_WHITE, COLOR_BLACK);
            } else {
                setColor(COLOR_BLACK, COLOR_BLACK); cout << "  ";
                setColor(COLOR_WHITE, COLOR_BLACK);
            }
        }
        // Padding thêm nếu boxW > 8
        cout << "  ";
        cout << (char)179; // Viền phải
    }

    // Viền dưới
    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(boxX, boxY + 6);
    cout << (char)192; for (int i = 0; i < boxW; i++) cout << (char)196; cout << (char)217;
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

    int choice = runMenu(); 
    if (choice == 0) {
        return 0; 
    }

    setup();
    
    // Ap dung Level va Toc do tu Menu
    level = choice;
    linesCleared = (level - 1) * 10; // Dong bo so dong da xoa voi level
    speed = max(100, 1000 - (level - 1) * 100);
    system("cls");
    drawOuterFrame();
    drawStats();
    currentPiece = getNextPieceFromBag();
    nextPiece = getNextPieceFromBag();

    int initGhostY = TetrisFeatures::getGhostY(currentPiece, board);
    TetrisFeatures::drawGhost(currentPiece, initGhostY, OFFSET_X, OFFSET_Y);
    drawCurrentPiece();

    // Khởi tạo khối đầu tiên và khối kế tiếp từ túi 7-bag
    drawNextPiece();

    clock_t start = clock();
    while (1) {
        if (_kbhit()) {
            int c = _getch();
            int oldGhostY = TetrisFeatures::getGhostY(currentPiece, board);
            TetrisFeatures::clearGhost(currentPiece, oldGhostY, OFFSET_X, OFFSET_Y);
            clearCurrentPiece();
            if (c == 224 || c == 0) { // Nếu bấm Phím Mũi Tên, hệ thống sẽ gửi 2 mã
                c = _getch(); // Đọc mã thứ 2 để biết hướng
                clearCurrentPiece(); 
                if (c == 75 && canMove(-1, 0)) currentPiece->moveLeft();  
                if (c == 77 && canMove(1, 0)) currentPiece->moveRight();  
            if (c == 72) {
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
                if (c == 80 && canMove(0, 1)) currentPiece->moveDown();   
                drawCurrentPiece(); 
            } else {
                c = tolower(c); 
                clearCurrentPiece(); 
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
                if ((c == 'c' || c == 'h')) {
                    holdCurrentPiece(currentPiece, nextPiece);
                    drawHoldPiece();
                } 
                if (c == ' ') {
                TetrisFeatures::hardDrop(currentPiece, board);
                // Trừ thẳng thời gian để vòng lặp ép khối khóa lại xuống đáy ngay lập tức
                start = clock() - speed; 
                if (c == 'q') break;
                int newGhostY = TetrisFeatures::getGhostY(currentPiece, board);
                TetrisFeatures::drawGhost(currentPiece, newGhostY, OFFSET_X, OFFSET_Y);
                drawCurrentPiece(); 
            }
        }

        if (clock() - start > speed) {
            int oldGhostYTimer = TetrisFeatures::getGhostY(currentPiece, board);
            TetrisFeatures::clearGhost(currentPiece, oldGhostYTimer, OFFSET_X, OFFSET_Y);
            clearCurrentPiece();
            if (canMove(0, 1)) {
                currentPiece->moveDown();
                int newGhostYTimer = TetrisFeatures::getGhostY(currentPiece, board);
                TetrisFeatures::drawGhost(currentPiece, newGhostYTimer, OFFSET_X, OFFSET_Y);
                drawCurrentPiece();
            } else {
                drawCurrentPiece();
                block2Board();
                checkLines();
                delete currentPiece;
                currentPiece = nextPiece;             // Khối kế tiếp trở thành khối hiện tại
                nextPiece = getNextPieceFromBag();     // Lấy khối mới từ túi 7-bag
                resetHoldPiece();
                drawNextPiece();                       // Cập nhật khung NEXT

                if (!canMove(0, 0)) {
                    clearBuffer();
                    drawOuterFrame();
                    drawCurrentPiece();
                    writeStringToBuffer(OFFSET_X + W - 3, OFFSET_Y + H / 2, " GAME OVER! ", (COLOR_BLACK << 4) | COLOR_RED);
                    renderScreen();
                    _getch(); // Đợi nhấn phím rồi mới thoát
                    
                    // --- LUU DIEM CAO NHAT (HIGH SCORE) ---
                    int currentHighScore = 0, currentHighLevel = 1;
                    ifstream inFile("highscore.txt");
                    if (inFile.is_open()) {
                        inFile >> currentHighScore >> currentHighLevel;
                        inFile.close();
                    }
                    
                    // Neu diem hien tai cao hon diem ky luc, thi luu de file
                    if (score > currentHighScore) {
                        ofstream outFile("highscore.txt");
                        if (outFile.is_open()) {
                            outFile << score << " " << level;
                            outFile.close();
                        }
                    }
                    // --------------------------------------

                    break;
                }
                int nextGhostY = TetrisFeatures::getGhostY(currentPiece, board);
                TetrisFeatures::drawGhost(currentPiece, nextGhostY, OFFSET_X, OFFSET_Y);
                drawCurrentPiece();
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
    delete nextPiece;
    currentPiece = nullptr;
    delete nextPiece;
    nextPiece = nullptr;
    return 0;
}
