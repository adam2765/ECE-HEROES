#ifndef AFFICHAGE_CONSOLE_H_INCLUDED
#define AFFICHAGE_CONSOLE_H_INCLUDED
#include <windows.h>
#include <conio.h>
// 16 couleurs utilisables en mode console de base
typedef enum{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
} COLORS;
//effacer la console
void clrscr ();
//déplacer le curseur à la position horizontale x, verticale y
void gotoxy(int x, int y);
//effacer la suite de la ligne
void delete_line();
//récupérer la position horizontale du curseur
int wherex ();
//récupérer la position verticale du curseur
int wherey ();
//changer la couleur du texte
void text_color(int color);
//changer la couleur de fond
void bg_color(int color);
//changer la couleur du texte et du fond
void set_color(int colorT,int colorBg);
//cacher le curseur
void hide_cursor();
//montrer le curseur
void show_cursor();
/*savoir si une touche a été pressée
retour : 1 si une touche a été pressée (caractère disponible en lecture avec getch), 0 sinon*/
int kbhit();
/*Lire un caractère du clavier immédiatement, sans avoir besoin d'appuyer sur Entrée.
s'utilise généralement avec kbhit :
si une touche a été pressée, on lit le caractère correspondant
Retour :  entier ayant le même code binaire que le caractère lu (code ASCII du caractère)
*/
int getch();
#endif // AFFICHAGE_CONSOLE_H_INCLUDED
