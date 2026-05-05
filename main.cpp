#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
using namespace std;
#define H 20
#define W 15
char board[H][W] = {} ;
char blocks[][4][4] = {
        {{' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '}},
        {{' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {'I','I','I','I'},
         {' ',' ',' ',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','T',' ',' '},
         {'T','T','T',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','S','S',' '},
         {'S','S',' ',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {'Z','Z',' ',' '},
         {' ','Z','Z',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {'J',' ',' ',' '},
         {'J','J','J',' '},
         {' ',' ',' ',' '}},
        {{' ',' ',' ',' '},
         {' ',' ','L',' '},
         {'L','L','L',' '},
         {' ',' ',' ',' '}}
};

int x=4,y=0,b=1;
int score = 0;

int level = 1;       // Cấp độ hiện tại
int totalLines = 0; // Tổng số dòng đã ăn để tính level
int speed = 150;    // Tốc độ mặc định ban đầu

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void boardDelBlock(){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (blocks[b][i][j] != ' ' && y+j < H)
                board[y+i][x+j] = ' ';
}

void block2Board(){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (blocks[b][i][j] != ' ' )
                board[y+i][x+j] = blocks[b][i][j];
}

void initBoard(){
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if ((i == 0) || (i == H - 1) || (j == 0) || (j == W - 1)) board[i][j] = '#';
            else board[i][j] = ' ';
}

void draw(){
    gotoxy(0,0);
    for (int i = 0 ; i < H ; i++, cout<<endl)
        for (int j = 0 ; j < W ; j++){
        if (board[i][j] == '#') {
            setColor(8);
            if (i == 0 && j == 0) cout << "╔═";
            else if (i == 0 && j == W - 1) cout << "╗ ";
            else if (i == H - 1 && j == 0) cout << "╚═";
            else if (i == H - 1 && j == W - 1) cout << "╝ ";
            else if (i == 0 || i == H - 1) cout << "══";
            else if (j == 0 || j == W - 1) cout << "║ ";
        } else if (board[i][j] == ' ') {
            cout << "  ";
        } else {
            setColor(14);
            cout << "██";
        }
    }
    setColor(11);
    gotoxy(W * 2 + 3, 2); cout << "╔════════════╗";
    gotoxy(W * 2 + 3, 3); cout << "║   TETRIS   ║";
    gotoxy(W * 2 + 3, 4); cout << "╠════════════╣";
    gotoxy(W * 2 + 3, 5); cout << "║ SCORE: " << score << "\t ║";
    gotoxy(W * 2 + 3, 6); cout << "║ LEVEL: " << level << "\t ║";
    gotoxy(W * 2 + 3, 7); cout << "╚════════════╝";
    setColor(7);
}

bool canMove(int dx, int dy){
    for (int i = 0 ; i < 4 ; i++)
        for (int j = 0 ; j < 4 ; j++)
            if (blocks[b][i][j] != ' '){
                int tx = x + j + dx;
                int ty = y + i + dy;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1) return false;
                if ( board[ty][tx] != ' ') return false;
            }
    return true;
}
void copyBlock(char temp[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[i][j] = blocks[b][i][j];
}

void rotateTempBlock(char temp[4][4]) {
    char rotated[4][4];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            rotated[j][3 - i] = temp[i][j];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[i][j] = rotated[i][j];
}

bool canRotate(char temp[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (temp[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1 || ty < 1)
                    return false;

                if (board[ty][tx] != ' ')
                    return false;
            }
        }
    }
    return true;
}
void rotateBlock() {
    char temp[4][4];
    copyBlock(temp);
    rotateTempBlock(temp);
    if (canRotate(temp)) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                blocks[b][i][j] = temp[i][j];
    }
}

// Gộp logic đếm dòng của feature/remove-line và logic cộng điểm của main
int removeLine() {
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++) {
            if (board[i][j] == ' ') { full = false; break; }
        }

        if (full) {
            score += 10; // Cập nhật điểm
            linesCleared++; // Đếm số hàng ăn được

            for (int ii = i; ii > 1; ii--) {
                for (int j = 1; j < W - 1; j++) board[ii][j] = board[ii - 1][j];
            }
            i++;
            draw();
            Sleep(200);
        }
    }
    return linesCleared;
}
// Hàm cập nhật Cấp độ và Tốc độ
void updateLevelAndSpeed(int lines) {
    if (lines > 0) {
        totalLines += lines;
        // Tính level dựa trên tổng số dòng (ví dụ 5 dòng 1 level)
        level = (totalLines / 5) + 1;
        // Giảm thời gian trễ để tăng tốc độ rơi
        speed = 150 - (level * 10);
        // Đặt giới hạn tốc độ tối thiểu để game không quá nhanh
        if (speed < 30) speed = 30;
    }
}
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    hideCursor();
    srand(time(0));
    b = rand() % 7;
    system("cls");
    initBoard();

    while (1){
        boardDelBlock();
        if (kbhit()){
            char c = getch();
            if (c=='a' && canMove(-1,0)) x--;
            if (c=='d' && canMove(1,0) ) x++;
            if (c=='w') rotateBlock();
            if (c == 's' && canMove(0, 1)) y++;
            if (c=='q') break;
        }
        if (canMove(0,1)) y++;
        else {
            block2Board();
            // Gọi hàm xử lý tăng tốc sau khi xóa dòng
            int lines = removeLine();
            updateLevelAndSpeed(lines);

            x = 5; y = 1; b = rand() % 7;

            // Logic Game Over từ nhánh main
            if (!canMove(0, 0)) {
                gotoxy(W * 2 + 3, 8);
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
    return 0;
}
