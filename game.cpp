#include <iostream>
#include <math.h>
#include <windows.h>

using namespace std;

bool keys[256];
#define keyPressed(key) keys[key]
#define updateKey(key) keys[key] = ((GetAsyncKeyState(key) & 0x8000) != 0)

// Configuraciones
#define SIZEX 40
#define SIZEY 20
#define STARTX (SIZEX / 2)
#define STARTY (SIZEY / 2) 
#define PLAYER_CHAR 'O'
#define STARTDIR 0
// 0 -> arriba 
// 1 -> abajo
// 2 -> izquierda
// 3 -> derecha
#define START_MS 100
#define FOOD_CHAR '$'

// Variables globales
string a[SIZEY];
int posx;
int posy;
int8_t goingTo;
int curMS;
int points;
int foodx, foody;

// Funcion para imprimir el mapa
inline void print() {
    system("cls");
    for (int i = 0; i < SIZEY; i++) {
        cout << a[i];
        cout << endl;
    }
    cout << "\n\nPuntos: " << points;
}

void generateFood();

void updateScore(int &x, int &y) {
    if (a[y][x] == FOOD_CHAR) {
        points++;
        generateFood();
        if (curMS > 10) curMS -= 10;
        else curMS = 1;
    }
}

// Mover en el eje x
inline void movex(int& pos, int val) {
    a[posy][pos] = ' ';
    pos += val;
    updateScore(pos, posy);
    a[posy][pos] = PLAYER_CHAR;
    print();
}

// Mover en el eje y
inline void movey(int& pos, int val) {
    a[pos][posx] = ' ';
    pos += val;
    updateScore(posx, pos);
    a[pos][posx] = PLAYER_CHAR;
    print();
}

inline void setMap() {
    // Preparacion del mapa
    for (int i = 1; i < SIZEY - 1; i++) {
        a[i].resize(SIZEX, ' ');
    }

    // Barra inferior y superior
    a[0].resize(SIZEX, '*');
    a[SIZEY - 1].resize(SIZEX, '*');

    // Barras laterales
    for (int i = 0; i < SIZEY; i++) {
        a[i][0] = '*';
        a[i][SIZEX - 1] = '*';
    }
}

bool rand_seeded = false;
int hrandomInRange(int min, int max) {
    if (!rand_seeded) {
        srand((unsigned int)GetTickCount());
        rand_seeded = true;
    }
    return (rand() % (max - min + 1)) + min;
}

void generateFood() {
    foodx = hrandomInRange(1, SIZEX - 2);
    foody = hrandomInRange(1, SIZEY - 2);
    a[foody][foodx] = FOOD_CHAR;

    // Previene que la comida desaparezca :O
    if ((foodx == posx) && (foody == posy)) generateFood();
}

inline bool playState() {
    // Cambiar el movimiento
    if      (keyPressed(VK_UP))     goingTo = 0;
    else if (keyPressed(VK_DOWN))   goingTo = 1;
    else if (keyPressed(VK_LEFT))   goingTo = 2;
    else if (keyPressed(VK_RIGHT))  goingTo = 3;

    // Movimiento y chequeo
    switch (goingTo) {
    case 0:
        if (posy < 2) return false;
        movey(posy, -1);
        break;
    case 1:
        if (posy + 3 > SIZEY) return false;
        movey(posy, 1);
        break;
    case 2:
        if (posx < 2) return false;
        movex(posx, -1);
        break;
    case 3:
        if (posx + 3 > SIZEX) return false;
        movex(posx, 1);
        break;
    }
    return true;
}

int main() {
    // Crear el mapa
    setMap();

    // Estados
    bool playing = true;
    bool ended = false;

    // Configuraciones iniciales
    posx = STARTX;
    posy = STARTY;
    goingTo = STARTDIR;
    a[posy][posx] = PLAYER_CHAR;
    curMS = START_MS;
    points = 0;

    // Genera de lo que no hay en Venezuela
    generateFood();

    // ks es un arreglo que contiene las teclas a ser registradas
    int ks[]{ VK_ESCAPE, VK_SPACE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT };

    // Juego
    while (!keyPressed(VK_ESCAPE)) {
        // Actualizar el registro de las teclas
        for (int k : ks) {
            updateKey(k);
        }

        // Game Loop
        if (playing) {
            if (!playState()) ended = true;
            else Sleep(curMS);
        }

        // Game Over
        if (ended && playing) {
            system("cls");
            cout << "GAME OVER\n\n";
            cout << "Puntuacion: " << points << "\n\n";
            cout << "Pulsa espacio para jugar de nuevo\n";
            cout << "Pulsa escape para salir";
            playing = false;
        }
        else if (ended && !playing) {
            // Reiniciar
            if (keyPressed(VK_SPACE)) {
                ended = false;
                playing = true;
                goingTo = STARTDIR;
                curMS = START_MS;
                movex(posx, STARTX - posx);
                movey(posy, STARTY - posy);
                a[foody][foodx] = ' ';
                generateFood();
                points = 0;
            }
        }
    }
}