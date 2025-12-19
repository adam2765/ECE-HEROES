#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <time.h> // Nécessaire pour gérer le temps (demandé par l'affichage)

// --- CONSTANTES ---
#define LIGNES 10       // Hauteur de la grille
#define COLONNES 15     // Largeur de la grille
#define NB_TYPES 5      // Nombre de couleurs de bonbons différents
#define MAX_PSEUDO 50   // Taille max du nom du joueur

// --- TYPES D'OBJETS ---
typedef enum {
    VIDE = 0,      // Case vide (quand un bonbon explose)
    ITEM_1,        // Bonbon Rouge
    ITEM_2,        // Bonbon Bleu
    ITEM_3,        // Bonbon Vert
    ITEM_4,        // Bonbon Jaune
    ITEM_5,        // Bonbon Magenta
    MUR,           // Obstacle (Extension)
    BONUS,         // Bonus spécial (Extension)
    MALUS          // Piège (Extension)
} TypeItem;

// --- STRUCTURES ---

// 1. Une case de la grille
typedef struct {
    TypeItem type;       // Ce qu'il y a dedans (1, 2, MUR...)
    int estSelectionne;  // 1 si le joueur a cliqué dessus (Espace), 0 sinon
} Case;

// 2. Le contrat (Les objectifs pour gagner le niveau)
typedef struct {
    // Combien de bonbons de chaque type il faut détruire
    // (Tableau de taille NB_TYPES + 1 pour utiliser les indices 1 à 5)
    int quantiteCible[NB_TYPES + 1]; 
    
    // Combien on en a déjà détruit
    int quantiteActuelle[NB_TYPES + 1]; 
} Contrat;

// 3. Le Niveau complet (Regroupe tout)
typedef struct {
    Case grille[LIGNES][COLONNES]; // Le plateau de jeu
    
    char pseudo[MAX_PSEUDO]; // Nom du joueur (pour la sauvegarde)
    int vies;                // Vies restantes
    int score;               // Score du joueur
    int coupsRestants;       // Nombre de coups avant Game Over
    int numeroNiveau;        // Niveau actuel (1, 2, 3...)
    
    // --- Gestion du Temps (Pour l'affichage HUD) ---
    int tempsTotalSec;       // Combien de temps on a au total (ex: 120s)
    time_t startTime;        // À quel moment le niveau a commencé
    
    Contrat contrat;         // Les objectifs à remplir
} Niveau;

#endif