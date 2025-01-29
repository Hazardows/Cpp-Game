#include <iostream>
#include <math.h>
#include <windows.h>

using namespace std;

bool keys[256];
#define keyPressed(key) keys[key]
#define updateKey(key) keys[key] = ((GetAsyncKeyState(key) & 0x8000) != 0)

// Configuraciones
#define SIZEX 20
#define SIZEY 20
#define STARTX (SIZEX / 2)
#define STARTY (SIZEY / 2) 
#define PLAYER_STRING "NIGGA"
#define PS_SIZE 5
#define STARTDIR 0
// 0 -> arriba 
// 1 -> abajo
// 2 -> izquierda
// 3 -> derecha
#define START_MS 100
#define FOOD_CHAR '$'

// Variables globales
string a[SIZEY];
int posx[PS_SIZE];
int posy[PS_SIZE];
int8_t goingTo;
int curMS;
int points;
int foodx, foody;
// Estados
bool playing;
bool ended;
bool moving;

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

// Mover el jugador
inline void move(int& posx, int &posy, int valx, int valy) {
    a[posy][posx] = ' ';
    posx += valx * 2;
    posy += valy;
    if (!ended) updateScore(posx, posy);
    a[posy][posx] = PLAYER_STRING[0];
    print();
}

inline void setMap() {
    // Preparacion del mapa
    for (int i = 1; i < SIZEY - 1; i++) {
        a[i].resize(SIZEX * 2, ' ');
    }

    // Barra inferior y superior
    a[0].resize((SIZEX * 2) - 1, '*');
    a[SIZEY - 1].resize((SIZEX * 2) - 1, '*');

    for (int i = 1; i < SIZEX * 2; i += 2) {
        a[0][i] = ' ';
        a[SIZEY - 1][i] = ' ';
    }

    // Barras laterales
    for (int i = 0; i < SIZEY; i++) {
        a[i][0] = '*';
        a[i][(SIZEX * 2) - 2] = '*';
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
    foodx = hrandomInRange(1, (SIZEX * 2) - 3);
    foody = hrandomInRange(1, SIZEY - 2);
    // Previene que la comida desaparezca :O
    if (((foodx == posx[0]) && (foody == posy[0])) || (foodx % 2 != 0)) generateFood();
    else a[foody][foodx] = FOOD_CHAR;
}

inline bool playState() {
    // Cambiar el movimiento

    // TODO: Fix this
    /*if (keyPressed(VK_SPACE)) {
        moving = !moving;
    }*/
    if (keyPressed(VK_UP) || keyPressed(0x57)) {
        if (!moving) moving = true;
        goingTo = 0;
    }
    else if (keyPressed(VK_DOWN) || keyPressed(0x53)) {
        if (!moving) moving = true;
        goingTo = 1;
    }
    else if (keyPressed(VK_LEFT) || keyPressed(0x41)) {
        if (!moving) moving = true;
        goingTo = 2;
    }
    else if (keyPressed(VK_RIGHT) || keyPressed(0x44)) {
        if (!moving) moving = true;
        goingTo = 3;
    }    
    // Movimiento y chequeo
    if (moving) {
        switch (goingTo) {
        case 0:
            if (posy[0] < 2) return false;
            move(posx[0], posy[0], 0, -1);
            break;
        case 1:
            if ((posy[0] + 3) > SIZEY) return false;
            move(posx[0], posy[0], 0, 1);
            break;
        case 2:
            if (posx[0] < 3) return false;
            move(posx[0], posy[0], -1, 0);
            break;
        case 3:
            if ((posx[0] + 5) > (SIZEX * 2)) return false;
            move(posx[0], posy[0], 1, 0);
            break;
        }
    }
    return true;
}

int main() {
    // Crear el mapa
    setMap();

    // Configuraciones iniciales
    posx[0] = STARTX * 2;
    posy[0] = STARTY;
    goingTo = STARTDIR;
    a[posy[0]][posx[0]] = PLAYER_STRING[0];
    curMS = START_MS;
    points = 0;
    playing = true;
    ended = false;
    moving = true;

    // Genera de lo que no hay en Venezuela
    generateFood();
    print();

    // ks es un arreglo que contiene las teclas a ser registradas
    int ks[]{ VK_ESCAPE, VK_SPACE, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 0x57, 0x53, 0x44, 0x41 };

    // Juego
    while (!keyPressed(VK_ESCAPE)) {
        // Actualizar el registro de las teclas
        for (int k : ks) updateKey(k);

        // Game Loop
        if (playing) {
            if (!playState()) ended = true;
            else {
                /*if (!(goingTo == 0 || goingTo == 1)) Sleep(curMS);
                else Sleep((curMS * 1.25));*/
                Sleep(curMS);
            }
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
                move(posx[0], posy[0], (((STARTX * 2) - posx[0]) / 2), STARTY - posy[0]);
                a[foody][foodx] = ' ';
                generateFood();
                points = 0;
                moving = true;
            }
        }
    }
}