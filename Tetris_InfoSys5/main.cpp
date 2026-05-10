#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "Piece.h"
#include "Blocks.h"
using namespace std;


char board[H][W] = {};
Piece *currentPiece = nullptr;

// --- ĐỊNH NGHĨA KÍCH THƯỚC UI ---
const int W = 12; 
const int H = 22; 
const int OFFSET_X = 2; 
const int OFFSET_Y = 1; 

enum TetrisColor {
    COLOR_BLACK = 0, COLOR_CYAN = 11, COLOR_YELLOW = 14, 
    COLOR_PURPLE = 13, COLOR_GREEN = 10, COLOR_RED = 12, 
    COLOR_BLUE = 9, COLOR_ORANGE = 6, COLOR_WHITE = 15, COLOR_GRAY = 8     
};

int linesCleared = 0; // Thêm biến đếm số dòng đã xóa

// Cập nhật hàm setColor có nền
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
    currentPiece = new PieceT();
}

// Khởi tạo các biến global cho Level, Speed và tổng số hàng
int score = 0;
int level = 1;      // Cấp độ hiện tại
int totalLines = 0; // Tổng số dòng đã ăn để tính level
int speed = 150;    // Tốc độ mặc định ban đầu

// Ép kiểu SHORT cho COORD để tránh cảnh báo compiler
void gotoxy(int x, int y)
{
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}


void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
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
    gotoxy(statsX, statsY + 1); cout << (char)218 << "-------------"; cout << (char)191; 
    gotoxy(statsX, statsY + 2); cout << (char)179 << " SCORE       "; cout << (char)179; 
    gotoxy(statsX, statsY + 3); cout << (char)195 << "-------------"; cout << (char)180; 
    
    gotoxy(statsX + 2, statsY + 4); setColor(COLOR_YELLOW, COLOR_BLACK);
    string s = to_string(score);
    for(int i=0; i < 10 - s.length(); i++) cout << " "; cout << s;

    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(statsX, statsY + 5); cout << (char)192 << "-------------"; cout << (char)217; 
    gotoxy(statsX, statsY + 7); cout << (char)218 << "-------------"; cout << (char)191;
    gotoxy(statsX, statsY + 8); cout << (char)179 << " LEVEL       "; cout << (char)179;
    gotoxy(statsX, statsY + 9); setColor(COLOR_GREEN, COLOR_BLACK); printf("      %02d     ", level);
    setColor(COLOR_WHITE, COLOR_BLACK);
    gotoxy(statsX, statsY + 10); cout << (char)192 << "-------------"; cout << (char)217;

    setColor(COLOR_GRAY, COLOR_BLACK);
    gotoxy(statsX, statsY + 13); cout << "CONTROL:";
    gotoxy(statsX + 2, statsY + 14); cout << "A / D : Move";
    gotoxy(statsX + 2, statsY + 15); cout << "W     : Rotate";
    gotoxy(statsX + 2, statsY + 16); cout << "S     : Soft Drop";
    gotoxy(statsX + 2, statsY + 17); cout << "Q     : Quit";
    setColor(COLOR_WHITE, COLOR_BLACK); 
}

void drawCurrentPiece() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getShape(i, j) != ' ') {
                int px = currentPiece->getX() + j;
                int py = currentPiece->getY() + i;
                if (py >= 0 && py < H && px >= 0 && px < W) 
                    drawBlock(px, py, '#'); // Sẽ đổi thành ký tự khác khi có nhiều khối
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
                // Cập nhật biên cho khớp với UI 12x22
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
                    board[currentPiece->getY() + i][currentPiece->getX() + j] = '#'; 
            }
        }
    }
}

void updateBoardUI() {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            drawBlock(j, i, board[i][j]);
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
            for(int blink=0; blink<2; blink++) {
                gotoxy(OFFSET_X, OFFSET_Y + i); setColor(COLOR_BLACK, COLOR_WHITE);
                for(int j=0; j<W*2; j++) cout << " "; Sleep(50);
                gotoxy(OFFSET_X, OFFSET_Y + i); setColor(COLOR_BLACK, COLOR_BLACK);
                for(int j=0; j<W*2; j++) cout << " "; Sleep(50);
            }
            setColor(COLOR_WHITE, COLOR_BLACK);

            for (int k = i; k > 0; k--) 
                for (int j = 0; j < W; j++) 
                    board[k][j] = board[k - 1][j];
                    
            for (int j = 0; j < W; j++) board[0][j] = ' ';
            i++; 
            updateBoardUI(); 
        }
    }
    
    if (combo == 1) score += 100 * level;
    else if (combo == 2) score += 300 * level;
    else if (combo == 3) score += 500 * level;
    else if (combo == 4) score += 800 * level;

    if (combo > 0) {
        linesCleared += combo;
        level = (linesCleared / 10) + 1;
        speed = max(100, 1000 - (level - 1) * 100);
        drawStats(); 
    }
}

int main() {
    system("mode con cols=60 lines=26");
    SetConsoleTitleA("Tetris InfoSys5 - Reworked UI");

    setup();
    system("cls");
    drawOuterFrame();
    drawStats();

    clock_t start = clock();
    while (1) {
        if (_kbhit()) {
            char c = _getch();
            clearCurrentPiece(); 
            if (c == 'a' && canMove(-1, 0)) currentPiece->moveLeft();
            if (c == 'd' && canMove(1, 0)) currentPiece->moveRight();
            if (c == 'w') currentPiece->rotate(); 
            if (c == 's' && canMove(0, 1)) currentPiece->moveDown();
            if (c == 'q') break;
            drawCurrentPiece(); 
        }

        if (clock() - start > speed) {
            clearCurrentPiece();
            if (canMove(0, 1)) {
                currentPiece->moveDown();
                drawCurrentPiece();
            }
            else {
                drawCurrentPiece(); 
                block2Board();
                checkLines();
                delete currentPiece;
                currentPiece = new PieceT(); 

                if (!canMove(0, 0)) {
                    for(int i=0; i<3; i++) {
                        system("color 08"); Sleep(200);
                        system("color 00"); Sleep(200);
                    }
                    
                    int goX = OFFSET_X + W / 2 + 1;
                    int goY = OFFSET_Y + H / 2 - 1;
                    
                    setColor(COLOR_RED, COLOR_BLACK);
                    gotoxy(goX - 2, goY - 1); cout << (char)201 << "═════════════" << (char)187;
                    gotoxy(goX - 2, goY);     cout << (char)186 << "  GAME OVER! " << (char)186;
                    gotoxy(goX - 2, goY + 1); cout << (char)200 << "═════════════" << (char)188;
                    
                    setColor(COLOR_WHITE, COLOR_BLACK);
                    gotoxy(0, OFFSET_Y + H + 2);
                    system("pause");
                    break;
                }
            }
            start = clock();
        }
    }
    return 0;
}