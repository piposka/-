#include <iostream>
#include <ctime>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include <vector>
using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
COORD cursor_coordinates;

void simple_drawer(short lineNumber, short columnNumber, char thisDraw);

struct sapperField {
    static const int x = 20; // количество столбцов
    static const int y = 20; // количество строк 
    char size[y][x];
    int points = 0;

    char field_random() {
        if (rand() % 40 + 1 > 3) {
            return '0';
        }
        else {
            points++;
            return '*';
        }
    }
    void new_field() {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                size[i][j] = field_random();
            }
        }
    }
    void mine_nearby(int mine_y, int mine_x) {
        for (int i = (mine_y - 1); i <= (mine_y + 1); i++) {
            for (int j = (mine_x - 1); j <= (mine_x + 1); j++) {
                if ((i >= 0) and (i < y)) {
                    if ((j >= 0) and (j < x)) {
                        if (size[i][j] != '*') {
                            int pseudo = 0;
                            pseudo = size[i][j] - 0;
                            pseudo += 1;
                            size[i][j] = pseudo;
                        }
                    }
                }
            }
        }
    }
    void mine_search() {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                if (size[i][j] == '*') {
                    mine_nearby(i, j);
                }
            }
        }
    }
    void draw_new_field() {
        for (short i = 0; i < y; i++) {
            for (short j = 0; j < x; j++) {
                simple_drawer(i, j, size[i][j]);
            }
        }
    }
    void initialization() {
        new_field();
        mine_search();
    }
};

struct playField: sapperField
{
    vector <vector <int>> cash;
    void create_field() {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                size[i][j] = '-';
            }
        }
    }
    void playField_inizialization(){
        create_field();
        draw_new_field();
    }
};

sapperField field;
playField playingField;

int person_x; // координаты объекта, который двигается, относительно пространства консоли
int person_y;

const char person = 'X';        // переменные, которые будут отрисовываться. Без их описания в переменнных можно обойтись,
const char obstruction = '*';   // но если кому-то захочется поменять, к примеру, отображение персонажа, так будет это легче сделать
const char road = '-';

bool gameOver = false;

int x_move = 1;
int y_move = 0;



void cursor_disable() {
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);
}

void element_color(char thisElement) {
    switch (thisElement) {
    case obstruction: {
        SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 4));
        break;
    }
    case person: {
        SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 10));
        break;
    }
    default: {
        SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 5));
    }
    }
}


void simple_drawer(short lineNumber, short columnNumber, char thisDraw) {
    int indentLength = 2; // отступ между объектами
    if (columnNumber != field.x) {
        columnNumber = columnNumber * (indentLength + 1); // перевод позиции в матрице в позицию в консоли (перевод необходимен из-за наличия отступов)
    }
    cursor_coordinates = { columnNumber, lineNumber };
    element_color(thisDraw);
    SetConsoleCursorPosition(hConsole, cursor_coordinates);
    cout << thisDraw;
}
void move_cursor (short lineNumber, short columnNumber) {
    int indentLength = 2; // отступ между объектами
    if (columnNumber != field.x) {
        columnNumber = columnNumber * (indentLength + 1); // перевод позиции в матрице в позицию в консоли (перевод необходимен из-за наличия отступов)
    }
    cursor_coordinates = { columnNumber, lineNumber };
    SetConsoleCursorPosition(hConsole, cursor_coordinates);
}
int random_coord(int this_coord) {
    return rand() % (this_coord - 1) + 1;
}

void person_generate() {
    person_x = random_coord(field.x);
    person_y = random_coord(field.y);
    field.size[person_y][person_x] = person;
}

char field_random() {
    if (rand() % 9 + 1 > 7) {
        return obstruction;
    }
    else {
        return road;
    }
}

void delete_person() {
    field.size[person_y][person_x] = road;
    simple_drawer(person_y, person_x, road);
}

void game_exit() {
    for (int i = 0; i < field.y; i++) {//
        for (int j = 0; j < field.x; j++) {//
            simple_drawer(i, j, ' ');
            Sleep(1);
        }
    }
}

vector<vector <int>> draw_point_square(vector<vector <int>> cashField) {
    int start_point_x = person_x - 1;
    int start_point_y = person_y - 1;
    int cash_number = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
           // Sleep(100);
            cash_number = i * 3 + j;
            if (((start_point_x + j) >= 0) and ((start_point_x + j) < playingField.x) and ((start_point_y + i) >= 0) and ((start_point_y + i) < playingField.y)) {
                char elementField = field.size[start_point_y + i][start_point_x + j];
                char elementPlayingField = playingField.size[start_point_y + i][start_point_x + j];
                if ((elementField == '0') and (elementPlayingField == road)) {
                    vector <int> elementCash(2);
                    elementCash[0] = start_point_y + i; //y
                    elementCash[1] = start_point_x + j;
                    cashField.push_back(elementCash);
                }
                playingField.size[start_point_y + i][start_point_x + j] = field.size[start_point_y + i][start_point_x + j];
                simple_drawer(start_point_y + i, start_point_x + j, playingField.size[start_point_y + i][start_point_x + j]);
            }
        }
    }
    return cashField;
}

void move_person() {

}

void draw_zero_elements() {
    int passNumber = 0;
    playField cashField;
    cashField.cash = draw_point_square(cashField.cash);
    passNumber++;
    while (cashField.cash.size() >= passNumber) {
        person_y = cashField.cash[passNumber - 1][0];
        person_x = cashField.cash[passNumber - 1][1];
        cashField.cash = draw_point_square(cashField.cash);
        passNumber++;
    }

}
void damage_check() {
    playingField.size[person_y][person_x] = field.size[person_y][person_x];
    char element = playingField.size[person_y][person_x]; // сделать с помощью указателя
    switch (element) 
    {
    case '*': {
        gameOver = true;
        game_exit();
        break;
    }
    case '0':draw_zero_elements(); break;
    default:simple_drawer(person_y, person_x, element);
    } 
}
void points_checker() {
    if ((field.size[person_y][person_x] == obstruction) and (playingField.size[person_y][person_x] == road)) {
        field.points--;
    }
}
bool is_moving() {
    int move_code = _getch();
    switch (move_code) {
    case 72:
        if (person_y != 0) {
            person_y -= 1;
        }
        break;
    case 80:
        if (person_y != (field.y - 1)) {
            person_y += 1;
        }
        break;
    case 75:
        if (person_x != 0) {
            person_x -= 1;
        }
        break;
    case 77:
        if (person_x != (field.x - 1)) {
            person_x += 1;
        }
        break;
    case 27: {
        game_exit();
        return true;
        break;
    }
    case 32: { //space
        damage_check();
        break;
    }
    case 43: { //+
        points_checker();
        playingField.size[person_y][person_x] = '+';
        simple_drawer(person_y, person_x, '+');
        break;
    }
    case 8: {
        if (playingField.size[person_y][person_x] == '+') {
            playingField.size[person_y][person_x] = '-';
            simple_drawer(person_y, person_x, playingField.size[person_y][person_x]);
        }
        break;
    }
    }
    move_cursor(person_y, person_x);
}

void how_to_play() {
    move_cursor(field.y+2, 0);
    cout << "Press + to place a check mark" << endl;
    cout << "Press SPACE to open the field" << endl;
    cout << "Version 1.0";
}

int main()
{   
    srand(time(NULL));
    field.initialization();
    playingField.playField_inizialization();
    how_to_play();
    move_cursor(0, 0);
    while (gameOver == false) {
        is_moving();
        if (field.points == 0) {
            gameOver = true;
            game_exit();
            cout << endl<< "You won!";
        }
    }
}
