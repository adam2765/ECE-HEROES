#include <stdio.h>
#include <stdlib.h> // Pour rand() et srand()
#include <time.h>   // Pour time()
#include "jeu.h"

// Fonction pour démarrer la partie et remplir les données
void initialiserNiveau(Niveau *niveau, int numNiveau) {
    // 1. On active le hasard
    srand(time(NULL));

    // 2. Initialisation des variables simples
    niveau->numeroNiveau = numNiveau;
    niveau->vies = 3;            
    niveau->score = 0;
    niveau->coupsRestants = 20; 
    
    // --- IMPORTANT POUR L'AFFICHAGE ---
    niveau->tempsTotalSec = 120;     // 2 minutes
    niveau->startTime = time(NULL);  // Top départ
    // ----------------------------------

    // On met le pseudo à vide
    niveau->pseudo[0] = '\0'; 

    // 3. Initialisation du Contrat (Objectifs à 0 au début)
    for (int k = 0; k <= NB_TYPES; k++) {
        niveau->contrat.quantiteCible[k] = 0;
        niveau->contrat.quantiteActuelle[k] = 0;
    }
    
    // Exemple : Pour le niveau 1, on veut tuer 10 items de type 1
    if (numNiveau == 1) {
        niveau->contrat.quantiteCible[ITEM_1] = 10; 
    }

    // 4. Remplissage de la grille avec des items aléatoires
    for(int i = 0; i < LIGNES; i++) {
        for(int j = 0; j < COLONNES; j++) {
            
            // On tire un nombre entre 1 et 5
            int typeAleatoire = (rand() % NB_TYPES) + 1;
            
            niveau->grille[i][j].type = typeAleatoire;
            niveau->grille[i][j].estSelectionne = 0; // Pas sélectionné
        }
    }
}

// Fonction pour échanger deux cases (Chapitre 11 : Pointeurs)
void echangerCases(Case *case1, Case *case2) {
    Case temp;
    temp = *case1;       // Sauvegarde
    *case1 = *case2;     // Echange
    *case2 = temp;       // Restauration
}

// ============== ITEMS SPÉCIAUX (Travail de ton collègue adapté) ==============

// ITEM 1 : Bombe - détruit zone 3x3 autour de (x,y)
void itemBombe(Niveau *niveau, int x, int y) {
    // On parcourt les cases autour (i, j)
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            // On vérifie qu'on ne sort pas du tableau (Sécurité)
            if (i >= 0 && i < LIGNES && j >= 0 && j < COLONNES) {
                // ADAPTATION : On change le .type, pas juste l'int
                niveau->grille[i][j].type = VIDE; // 0 = VIDE
                
                // Petit bonus : ça augmente le score !
                niveau->score += 10; 
            }
        }
    }
}

// ITEM 2 : Supprimer une ligne entière
void itemLigne(Niveau *niveau, int ligne) {
    // Sécurité
    if (ligne < 0 || ligne >= LIGNES) return;

    for (int j = 0; j < COLONNES; j++) {
        niveau->grille[ligne][j].type = VIDE;
        niveau->score += 10;
    }
}

// ITEM 3 : Supprimer une colonne entière
void itemColonne(Niveau *niveau, int colonne) {
    // Sécurité
    if (colonne < 0 || colonne >= COLONNES) return;

    for (int i = 0; i < LIGNES; i++) {
        niveau->grille[i][colonne].type = VIDE;
        niveau->score += 10;
    }
}

// ITEM 4 : Supprimer toutes les cases d'un type/couleur
void itemCouleur(Niveau *niveau, int type) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            // On compare avec .type
            if (niveau->grille[i][j].type == type) {
                niveau->grille[i][j].type = VIDE;
                niveau->score += 10;
            }
        }
    }
}