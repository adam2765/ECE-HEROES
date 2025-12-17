#ifndef STRUCTURES_H
#define STRUCTURES_H

// --- Constantes ---

// Dimensions (10x15 pour tester, modifiable plus tard)
#define LIGNES 10
#define COLONNES 15

// Nombre de types de bonbons (sans compter les murs)
#define NB_TYPES 5

// Taille max du nom du joueur
#define MAX_PSEUDO 50

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

// --- Structures ---

// Une case du plateau
typedef struct {
    TypeItem type;       // Le type (1, 2, MUR...)
    int estSelectionne;  // 1 si sélectionné, 0 sinon
} Case;

// Les objectifs pour gagner le niveau (Ex: détruire 10 rouges)
typedef struct {
    // Tableau : combien il faut détruire de chaque type
    // index 1 = ITEM_1, index 2 = ITEM_2, etc.
    int quantiteCible[NB_TYPES + 1]; 
    
    // Tableau : combien on en a déjà détruit
    int quantiteActuelle[NB_TYPES + 1];
} Contrat;

// La partie complète
typedef struct {
    Case grille[LIGNES][COLONNES]; // Le plateau
    
    char pseudo[MAX_PSEUDO]; // Nom du joueur (pour sauvegarder)
    int vies;                // Vies restantes
    int score;               // Score actuel
    int coupsRestants;       // Coups avant game over
    int numeroNiveau;        // Niveau 1, 2, 3...
    
    Contrat contrat;         // L'objectif à atteindre
} Niveau;

#endif