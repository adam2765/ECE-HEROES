#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <time.h> // Nécessaire pour le type time_t

// --- CONSTANTES ---
#define NB_LIG 25
#define NB_COL 45
#define NB_ITEMS 5

// Couleurs
#define NOIR 0
#define BLEU 9
#define VERT 10
#define ROUGE 12
#define MAGENTA 13
#define JAUNE 14
#define BLANC 15
#define GRIS 8

// Touches
#define KEY_UP     1001
#define KEY_DOWN   1002
#define KEY_LEFT   1003
#define KEY_RIGHT  1004

// --- STRUCTURE DU JEU ---
typedef struct {
    int grille[NB_LIG][NB_COL];
    int vies;
    int score;
    int coupsRestants;
    int niveauActuel;

    // Gestion du temps
    int tempsTotalSec;
    time_t startTime;

    // Contrat + progression
    int contrat[NB_ITEMS + 1];   // objectif à atteindre
    int elimines[NB_ITEMS + 1];  // combien éliminés

    // Curseur
    int cursorX;
    int cursorY;
    int isSelected;
} GameState;

// --- PROTOTYPES ---

// Outils Console
void initConsole(void);
void color(int t, int f);
void gotoligcol(int y, int x);
void clearLineAt(int y, int x, int w);

// Logique Temps & Input
int tempsRestantSec(const GameState *e);
int lireTouche(void);

// Fonctions d'Affichage
void afficherCadre(void);
void afficherGrille(const GameState *e);
void afficherHUD(const GameState *e);

#endif
