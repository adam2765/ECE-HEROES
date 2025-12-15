#ifndef STRUCTURES_H
#define STRUCTURES_H

// --- 1. CONSTANTES (Faciles à changer ici) ---

// Taille de la grille. Le prof a dit par mail qu'on pouvait changer.
// 25x45 c'est trop grand, on met 10x15 pour tester tranquillement.
#define LIGNES 10
#define COLONNES 15

// Nombre de types de bonbons de base (A, B, C, D, E)
#define NB_TYPES 5

// Longueur max du pseudo du joueur (pour la sauvegarde)
#define MAX_PSEUDO 50

// --- 2. ENUMERATIONS (Les types d'objets) ---

typedef enum {
    VIDE = 0,      // Case vide (quand ça explose)
    
    // Les items de base (Bonbons, Légumes...)
    ITEM_1,
    ITEM_2,
    ITEM_3,
    ITEM_4,
    ITEM_5,

    // Extensions obligatoires (Page 9 du sujet)
    MUR,           // Bloque la gravité
    BONUS,         // Item spécial positif
    MALUS          // Item spécial négatif (virus...)
} TypeItem;

// --- 3. STRUCTURES (Les briques de ton jeu) ---

// Une case unique de la grille
typedef struct {
    TypeItem type;       // C'est quoi ? (Bonbon rouge, mur, vide...)
    int estSelectionne;  // 1 si le joueur a cliqué dessus (surbrillance), 0 sinon
} Case;

// Pour gérer le "Contrat" (Objectifs à atteindre pour gagner)
// Exemple : "Détruire 20 fraises" -> typeCible = FRAISE, nbA_Detruire = 20
typedef struct {
    int quantiteCible[NB_TYPES + 1]; // Combien il faut détruire de chaque type (index 1 à 5)
    int quantiteActuelle[NB_TYPES + 1]; // Combien on en a déjà détruit
} Contrat;

// La structure principale qui contient TOUTE ta partie
typedef struct {
    // Le plateau de jeu
    Case grille[LIGNES][COLONNES];

    // Infos du joueur
    char pseudo[MAX_PSEUDO]; // Pour charger/sauvegarder
    int vies;                // Si 0, game over
    int score;               // Score actuel
    
    // Contraintes du niveau
    int numeroNiveau;        // Niveau 1, 2, 3...
    int coupsRestants;       // Si 0, on perd une vie
    int tempsRestant;        // En secondes (optionnel au début, mais demandé dans le sujet)
    
    // L'objectif pour gagner ce niveau
    Contrat contrat;

} Niveau;

#endif