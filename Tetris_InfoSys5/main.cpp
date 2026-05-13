#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include "Piece.h"
#include "Blocks.h"
using namespace std;

#define H 20
#define W 15
char board[H][W] = {};
Piece *currentPiece = nullptr;

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

void hideCursor()
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void block2Board()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece->getShape(i, j) != ' ')
                board[currentPiece->getY() + i][currentPiece->getX() + j] = currentPiece->getShape(i, j);
}

void boardDelBlock()
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece->getShape(i, j) != ' ' && currentPiece->getY() + j < H)
                board[currentPiece->getY() + i][currentPiece->getX() + j] = ' ';
}

void initBoard()
{
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if ((i == 0) || (i == H - 1) || (j == 0) || (j == W - 1))
                board[i][j] = '#';
            else
                board[i][j] = ' ';
}

//  Mở rộng khung và hiển thị Level trong hàm draw
void draw()
{
    gotoxy(0, 0);
    for (int i = 0; i < H; i++, cout << endl)
        for (int j = 0; j < W; j++)
        {
            if (board[i][j] == '#')
            {
                setColor(8);
                if (i == 0 && j == 0)
                    cout << "╔═";
                else if (i == 0 && j == W - 1)
                    cout << "╗ ";
                else if (i == H - 1 && j == 0)
                    cout << "╚═";
                else if (i == H - 1 && j == W - 1)
                    cout << "╝ ";
                else if (i == 0 || i == H - 1)
                    cout << "══";
                else if (j == 0 || j == W - 1)
                    cout << "║ ";
            }
            else if (board[i][j] == ' ')
            {
                cout << "  ";
            }
            else
            {
                setColor(14);
                cout << "██";
            }
        }
    setColor(11);
    gotoxy(W * 2 + 3, 2);
    cout << "╔════════════╗";
    gotoxy(W * 2 + 3, 3);
    cout << "║   TETRIS   ║";
    gotoxy(W * 2 + 3, 4);
    cout << "╠════════════╣";
    gotoxy(W * 2 + 3, 5);
    cout << "║ SCORE: " << score << "\t ║";
    // Hiển thị Level ra màn hình
    gotoxy(W * 2 + 3, 6);
    cout << "║ LEVEL: " << level << "\t ║";
    gotoxy(W * 2 + 3, 7);
    cout << "╚════════════╝";
    setColor(7);
}

bool canMove(int dx, int dy)
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece->getShape(i, j) != ' ')
            {
                int tx = currentPiece->getX() + j + dx;
                int ty = currentPiece->getY() + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1)
                    return false;
                if (board[ty][tx] != ' ')
                    return false;
            }
    return true;
}

// Kiểm tra xem khối hiện tại có thể xoay mà không va chạm không
bool canRotate()
{
    // Bước 1: Transpose (hoán vị)
    char temp[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[i][j] = currentPiece->getShape(j, i);

    // Bước 2: Reverse từng hàng -> ra hình xoay thử
    char rotated[4][4];
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            rotated[i][j] = temp[i][3 - j];

    // Bước 3: Kiểm tra va chạm với tường và board
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (rotated[i][j] != ' ')
            {
                int tx = currentPiece->getX() + j;
                int ty = currentPiece->getY() + i;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1)
                    return false;
                if (board[ty][tx] != ' ')
                    return false;
            }
    return true;
}

int removeLine()
{
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--)
    {
        bool full = true;
        for (int j = 1; j < W - 1; j++)
        {
            if (board[i][j] == ' ')
            {
                full = false;
                break;
            }
        }
        if (full)
        {
            score += 10;
            linesCleared++;
            for (int ii = i; ii > 1; ii--)
            {
                for (int j = 1; j < W - 1; j++)
                    board[ii][j] = board[ii - 1][j];
            }
            i++;
            draw();
            Sleep(200);
        }
    }
    return linesCleared;
}
// Sinh ngẫu nhiên một trong 7 loại khối gạch
Piece* spawnRandom()
{
    int r = rand() % 7;
    switch (r)
    {
        case 0: return new PieceT();
        case 1: return new PieceL();
        case 2: return new PieceJ();
        case 3: return new PieceS();
        case 4: return new PieceZ();
        case 5: return new PieceI();
        case 6: return new PieceO();
    }
    return new PieceT(); // Mặc định
}

// Tách hàm cập nhật Cấp độ và Tốc độ
void updateLevelAndSpeed(int lines)
{
    if (lines > 0)
    {
        totalLines += lines;
        // Tính level dựa trên tổng số dòng (ví dụ 5 dòng 1 level)
        level = (totalLines / 5) + 1;
        // Giảm thời gian trễ để tăng tốc độ rơi
        speed = 150 - (level * 10);
        // Đặt giới hạn tốc độ tối thiểu để game không quá nhanh
        if (speed < 30)
            speed = 30;
    }
}



int main()
{
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    srand((unsigned int)time(0));
    system("cls");
    initBoard();

    currentPiece = spawnRandom();

    while (1)
    {
        boardDelBlock();
        if (_kbhit())
        {
            char c = _getch();
            if (c == 'a' && canMove(-1, 0))
                currentPiece->moveLeft();
            if (c == 'd' && canMove(1, 0))
                currentPiece->moveRight();
            // Chặn xoay xuyên tường: chỉ xoay khi canRotate() trả true
            if (c == 'w' && canRotate())
                currentPiece->rotate(); 
            if (c == 's' && canMove(0, 1))
                currentPiece->moveDown();
            if (c == 'q')
                break;
        }
        if (canMove(0, 1))
            currentPiece->moveDown();
        else
        {
            block2Board();
            // Hứng kết quả số hàng ăn được (Lưu ý: dùng biến lines để không đè biến score toàn cục)
            // Gọi hàm xử lý tăng tốc sau khi xóa dòng
            int lines = removeLine();
            updateLevelAndSpeed(lines);

            
            // Giải phóng bộ nhớ khối cũ trước khi tạo khối mới
            delete currentPiece;
            currentPiece = spawnRandom();

            // Logic Game Over từ nhánh main
            // Chỉnh tọa độ thông báo Game Over cho khớp UI mới
            if (!canMove(0, 0))
            {
                gotoxy(W * 2 + 3, 9);
                setColor(12);
                cout << " GAME OVER! ";
                break;
            }
        }
        block2Board();
        draw();
        // Sử dụng biến speed thay cho giá trị cố định
        Sleep(speed);
    }
    // Giải phóng bộ nhớ khi kết thúc game (chống memory leak)
    delete currentPiece;
    currentPiece = nullptr;
    return 0;
}
