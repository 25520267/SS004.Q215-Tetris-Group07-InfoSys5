#include <iostream>
#include <iomanip>
#include <sstream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string>
#include "Menu.h"
#include "Blocks.h"

using namespace std;

// --- ĐỊNH NGHĨA KÍCH THƯỚC UI ---
const int W = 12; 
const int H = 22; 
const int OFFSET_X = 2; 
const int OFFSET_Y = 1; 

char board[H][W] = {};
Piece *currentPiece = nullptr;
Piece *nextPiece = nullptr;

int score = 0;
int level = 1;      
int linesCleared = 0; 
int speed = 1000;    

enum TetrisColor {
    COLOR_BLACK = 0, COLOR_CYAN = 11, COLOR_YELLOW = 14, 
    COLOR_PURPLE = 13, COLOR_GREEN = 10, COLOR_RED = 12, 
    COLOR_BLUE = 9, COLOR_ORANGE = 6, COLOR_WHITE = 15, COLOR_GRAY = 8     
};

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void setColor(int foreground, int background = 0) {
    WORD wAttributes = (background << 4) | foreground;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
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

void drawBlock(int boardX, int boardY, char type) {
    gotoxy(OFFSET_X + boardX * 2, OFFSET_Y + boardY);
    if (type == ' ') {
        setColor(COLOR_GRAY, COLOR_BLACK); cout << ". "; 
    } else {
        int color = getPieceColor(type);
        setColor(COLOR_BLACK, color); cout << "  "; 
        setColor(COLOR_WHITE, COLOR_BLACK); 
    }
}

void drawOuterFrame() {
    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(OFFSET_X - 1, OFFSET_Y - 1);
    cout << (char)201; for (int i = 0; i < W * 2; i++) cout << (char)205; cout << (char)187;
    for (int i = 0; i < H; i++) {
        gotoxy(OFFSET_X - 1, OFFSET_Y + i); cout << (char)186; 
        for (int j = 0; j < W; j++) {
            if (board[i][j] == ' ') { setColor(COLOR_GRAY, COLOR_BLACK); cout << ". "; }
            else drawBlock(j, i, board[i][j]);
        }
        setColor(COLOR_WHITE, COLOR_BLACK);
        gotoxy(OFFSET_X + W * 2, OFFSET_Y + i); cout << (char)186; 
    }
    gotoxy(OFFSET_X - 1, OFFSET_Y + H);
    cout << (char)200; for (int i = 0; i < W * 2; i++) cout << (char)205; cout << (char)188; 
}

void drawStats() {
    int statsX = OFFSET_X + W * 2 + 4;
    int statsY = OFFSET_Y + 1;
    gotoxy(statsX, statsY - 1); setColor(COLOR_CYAN, COLOR_BLACK); cout << "== TETRIS INFOSYS 5 ==";
    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(statsX, statsY + 1); cout << "SCORE: " << score << "    ";
    gotoxy(statsX, statsY + 2); cout << "LEVEL: " << level << "    ";
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

void drawCurrentPiece() {
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

void clearCurrentPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getShape(i, j) != ' ') {
                int px = currentPiece->getX() + j;
                int py = currentPiece->getY() + i;
                if (py >= 0 && py < H && px >= 0 && px < W) 
                    drawBlock(px, py, ' '); 
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

void updateBoardUI() {
    for (int i = 0; i < H; i++) 
        for (int j = 0; j < W; j++) 
            drawBlock(j, i, board[i][j]);
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
        updateBoardUI();
        drawStats(); 
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

    // Khởi tạo khối đầu tiên và khối kế tiếp từ túi 7-bag
    currentPiece = getNextPieceFromBag();
    nextPiece = getNextPieceFromBag();
    drawNextPiece();

    clock_t start = clock();
    while (1) {
        if (_kbhit()) {
            int c = _getch();
            if (c == 224 || c == 0) { // Nếu bấm Phím Mũi Tên, hệ thống sẽ gửi 2 mã
                c = _getch(); // Đọc mã thứ 2 để biết hướng
                clearCurrentPiece(); 
                if (c == 75 && canMove(-1, 0)) currentPiece->moveLeft();  
                if (c == 77 && canMove(1, 0)) currentPiece->moveRight();  
                if (c == 72 && canRotate()) currentPiece->rotate();       
                if (c == 80 && canMove(0, 1)) currentPiece->moveDown();   
                drawCurrentPiece(); 
            } else {
                c = tolower(c); 
                clearCurrentPiece(); 
                if (c == 'a' && canMove(-1, 0)) currentPiece->moveLeft();
                if (c == 'd' && canMove(1, 0)) currentPiece->moveRight();
                if (c == 'w' && canRotate()) currentPiece->rotate(); 
                if (c == 's' && canMove(0, 1)) currentPiece->moveDown();
                if (c == 'q') break;
                drawCurrentPiece(); 
            }
        }

        if (clock() - start > speed) {
            clearCurrentPiece();
            if (canMove(0, 1)) {
                currentPiece->moveDown();
                drawCurrentPiece();
            } else {
                drawCurrentPiece(); 
                block2Board(); 
                checkLines(); 
                delete currentPiece;
                currentPiece = nextPiece;             // Khối kế tiếp trở thành khối hiện tại
                nextPiece = getNextPieceFromBag();     // Lấy khối mới từ túi 7-bag
                drawNextPiece();                       // Cập nhật khung NEXT

                if (!canMove(0, 0)) {
                    gotoxy(OFFSET_X + W / 2 - 5, OFFSET_Y + H / 2);
                    setColor(COLOR_RED, COLOR_BLACK);
                    cout << " GAME OVER! ";
                    
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
            }
            start = clock();
        }
    }
    delete currentPiece;
    currentPiece = nullptr;
    delete nextPiece;
    nextPiece = nullptr;
    return 0;
}