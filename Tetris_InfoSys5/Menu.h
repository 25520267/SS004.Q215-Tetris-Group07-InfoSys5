#pragma once
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <fstream>
using namespace std;

inline void menuGotoXY(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

inline void menuSetColor(int foreground) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreground);
}

inline int runMenu() {
    int selectedLevel = 1;
    int highScore = 0;
    int highLevel = 1;

    // Doc diem cao nhat tu file
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore >> highLevel;
        file.close();
    }

    while (true) {
        system("cls"); 
        
        // VE DIEM CAO NHAT
        menuSetColor(15); 
        menuGotoXY(24, 5); cout << "HIGH SCORE: " << highScore;

        // 1. VE GIAO DIEN MENU VOI MAU SAC
        menuSetColor(11);
        menuGotoXY(20, 8);  cout << "=======================";
        menuSetColor(14);
        menuGotoXY(20, 9);  cout << "     TETRIS GAME       ";
        menuSetColor(11);
        menuGotoXY(20, 10); cout << "=======================";
        
        menuSetColor(10); 
        menuGotoXY(25, 13); cout << "[ 1. PLAY ]";
        
        menuSetColor(13);
        menuGotoXY(25, 15); cout << "[ 2. LEVEL: " << selectedLevel << " ]";
        
        menuSetColor(12);
        menuGotoXY(25, 17); cout << "[ 3. EXIT ]";
        
        menuSetColor(15);
        menuGotoXY(18, 21); cout << "Nhan phim 1 de vao game";
        menuGotoXY(18, 22); cout << "Nhan phim 2 de tang level";
        menuGotoXY(18, 23); cout << "Nhan phim 3 de thoat game";

        // 2. VONG LAP CHO BAN PHIM 
        bool waitingForInput = true;
        while (waitingForInput) {
            if (_kbhit()) {
                char c = _getch();
                
                if (c == '1') {
                    menuSetColor(15); 
                    return selectedLevel; 
                }
                else if (c == '2') {
                    selectedLevel++; 
                    if (selectedLevel > 9) selectedLevel = 1;
                    waitingForInput = false;
                }
                else if (c == '3') {
                    menuSetColor(15);
                    return 0; 
                }
            }
            Sleep(50);
        }
    }
}