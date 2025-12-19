#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <time.h> // Nécessaire pour time_t

// --- CONSTANTES D'AFFICHAGE ---
#define NB_LIG 25
#define NB_COL 45
#define NB_ITEMS 5

// Codes Couleurs (Pour que tes collègues puissent aussi les utiliser si besoin)
#define NOIR 0
#define BLEU 9
#define VERT 10
#define ROUGE 12
#define MAGENTA 13
#define JAUNE 14
#define BLANC 15
#define GRIS 8

// --- STRUCTURE PARTAGÉE ---
// NOTE POUR L'EQUIPE : Cette structure définit les données que l'affichage doit lire.
// Si une structure commune existe déjà (ex: jeu.h), il faudra déplacer ceci dedans.
typedef struct {
    int grille[NB_LIG][NB_COL]; // Matrice du plateau
    int vies;
    int score;
    int coupsRestants;
    int niveauActuel;

    // Gestion du temps
    int tempsTotalSec;
    time_t startTime;

    // Contrat (Objectif)
    int contrat[NB_ITEMS + 1];   // Objectif à atteindre par type d'item
    int elimines[NB_ITEMS + 1];  // Combien ont été éliminés

    // Curseur & Interaction
    int cursorX;
    int cursorY;
    int isSelected; // 1 si le joueur a activé la sélection, 0 sinon
} GameState;

// --- PROTOTYPES (TES FONCTIONS) ---

// Outils techniques (Initialisation console)
void initConsole(void);
void color(int t, int f);
void gotoligcol(int y, int x);
void clearLineAt(int y, int x, int w);

// Calcul du temps (Utile pour l'affichage)
int tempsRestantSec(const GameState *e);

// Fonctions principales d'affichage à appeler dans le main
void afficherCadre(void);
void afficherGrille(const GameState *e);
void afficherHUD(const GameState *e);

#endif
