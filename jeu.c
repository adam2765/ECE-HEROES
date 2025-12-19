#include <stdio.h>
#include <stdlib.h> // Pour rand() et srand()
#include <time.h>   // Pour time()
#include "jeu.h"

// Fonction pour démarrer la partie et remplir les données
void initialiserNiveau(GameState *niveau, int numNiveau) {
    // 1. On active le hasard (sinon c'est toujours pareil)
    srand(time(NULL));

    // 2. Initialisation des variables simples
    niveau->niveauActuel = numNiveau;
    niveau->vies = 3;            // 3 vies au départ
    niveau->score = 0;
    niveau->coupsRestants = 20;  // On a 20 coups pour réussir
    niveau->tempsTotalSec = 120; // 2 minutes
    niveau->startTime = time(NULL);
    niveau->cursorX = 0;
    niveau->cursorY = 0;
    niveau->isSelected = 0;

    // 3. Initialisation du Contrat (Objectifs à 0 au début)
    // On parcourt le tableau des objectifs pour tout mettre à 0
    for (int k = 0; k <= NB_ITEMS; k++) {
        niveau->contrat[k] = 0;
        niveau->elimines[k] = 0;
    }
    
    // Exemple : Pour le niveau 1, on veut tuer 10 items de type 1 (Rouge)
    // (Tu pourras changer ça plus tard pour faire des niveaux différents)
    if (numNiveau == 1) {
        niveau->contrat[1] = 10; 
    }

    // 4. Remplissage de la grille avec des items aléatoires
    for(int i = 0; i < NB_LIG; i++) {
        for(int j = 0; j < NB_COL; j++) {
            
            // On tire un nombre entre 1 et 5
            int typeAleatoire = (rand() % NB_ITEMS) + 1;
            
            niveau->grille[i][j] = typeAleatoire;
        }
    }
}

// Fonction pour échanger deux cases (Chapitre 11 : Pointeurs)
void echangerCases(int *case1, int *case2) {
    // Variable temporaire pour stocker la valeur pendant l'échange
    int temp;
    
    temp = *case1;       // On sauvegarde la case 1
    *case1 = *case2;     // La case 1 prend la valeur de la case 2
    *case2 = temp;       // La case 2 récupère la valeur sauvegardée
}

// ============== ITEMS ==============

// ITEM 1 : Bombe - détruit zone 3x3
void itemBombe(GameState *niveau, int x, int y) {
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i >= 0 && i < NB_LIG && j >= 0 && j < NB_COL) {
                niveau->grille[i][j] = 0;
            }
        }
    }
}

// ITEM 2 : Supprimer une ligne entière
void itemLigne(GameState *niveau, int ligne) {
    for (int j = 0; j < NB_COL; j++) {
        niveau->grille[ligne][j] = 0;
    }
}

// ITEM 3 : Supprimer une colonne entière
void itemColonne(GameState *niveau, int colonne) {
    for (int i = 0; i < NB_LIG; i++) {
        niveau->grille[i][colonne] = 0;
    }
}

// ITEM 4 : Supprimer toutes les cases d'un type/couleur
void itemCouleur(GameState *niveau, int type) {
    for (int i = 0; i < NB_LIG; i++) {
        for (int j = 0; j < NB_COL; j++) {
            if (niveau->grille[i][j] == type) {
                niveau->grille[i][j] = 0;
            }
        }
    }
}