#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include "structure.h"

<<<<<<< HEAD
// --- COULEURS WINDOWS ---
// Codes simplifiés pour ne pas s'embêter avec les chiffres bizarres
=======
// --- CONSTANTES D'AFFICHAGE ---
#define NB_LIG 25
#define NB_COL 45
#define NB_ITEMS 5

// Taille max du nom du joueur
#define MAX_PSEUDO 50

// Codes Couleurs (Pour les utiliser si besoin)
>>>>>>> ac71d83ac6441efb53fc08117108fc90188edc63
#define NOIR 0
#define BLEU 1
#define VERT 2
#define CYAN 3
#define ROUGE 4
#define MAGENTA 5
#define JAUNE 6
#define BLANC 15
#define GRIS 8

<<<<<<< HEAD
// --- OUTILS TECHNIQUES (WINDOWS) ---
// Initialise la console (cache le curseur clignotant)
=======
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
>>>>>>> ac71d83ac6441efb53fc08117108fc90188edc63
void initConsole(void);

// Change la couleur du texte et du fond
void color(int t, int f);

// Déplace le curseur à la position (x, y)
void gotoligcol(int y, int x);

// Efface une ligne à partir d'une position donnée (pour nettoyer le texte)
void clearLineAt(int y, int x, int w);

// --- FONCTIONS D'AFFICHAGE DU JEU ---

// Affiche le cadre décoratif autour du jeu (à appeler une seule fois au début)
void afficherCadre(void);

<<<<<<< HEAD
// Affiche la grille de bonbons (met à jour les couleurs et sélections)
void afficherGrille(Niveau *niveau, int curseurX, int curseurY);

// Affiche les infos sur le côté (Score, Vies, Temps, Objectifs...)
void afficherHUD(Niveau *niveau);

=======
>>>>>>> ac71d83ac6441efb53fc08117108fc90188edc63
#endif