#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <time.h> // Nécessaire pour time_t

// --- CONSTANTES D'AFFICHAGE ---
#define NB_LIG 25
#define NB_COL 45
#define NB_ITEMS 5

// Taille max du nom du joueur
#define MAX_PSEUDO 50

// Codes Couleurs (Pour les utiliser si besoin)
#define NOIR 0
#define BLEU 9
#define VERT 10
#define ROUGE 12
#define MAGENTA 13
#define JAUNE 14
#define BLANC 15
#define GRIS 8

// --- Types d'objets ---
typedef enum {
    VIDE = 0,      // Case vide
    ITEM_1,        // Les symboles (1 à 5)
    ITEM_2,
    ITEM_3,
    ITEM_4,
    ITEM_5,
    MUR,           // Extension : Mur
    BONUS,         // Extension : Bonus
    MALUS          // Extension : Malus
} TypeItem;

// --- STRUCTURE PARTAGÉE ---
// NOTE POUR L'EQUIPE : Cette structure définit les données que l'affichage doit lire.
typedef struct {
    int grille[NB_LIG][NB_COL]; // Matrice du plateau
    
    int vies;                // Vies restantes
    int score;               // Score actuel
    int coupsRestants;       // Coups avant game over
    int niveauActuel;        // Niveau 1, 2, 3...

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

    // Joueur (pour sauvegarder)
    char pseudo[MAX_PSEUDO]; // Nom du joueur
} GameState;

// --- PROTOTYPES (LES FONCTIONS) ---

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