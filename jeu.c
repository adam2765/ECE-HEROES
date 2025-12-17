#include <stdio.h>
#include <stdlib.h> // Pour rand() et srand()
#include <time.h>   // Pour time()
#include "jeu.h"

// Fonction pour démarrer la partie et remplir les données
void initialiserNiveau(Niveau *niveau, int numNiveau) {
    // 1. On active le hasard (sinon c'est toujours pareil)
    srand(time(NULL));

    // 2. Initialisation des variables simples
    niveau->numeroNiveau = numNiveau;
    niveau->vies = 3;            // 3 vies au départ
    niveau->score = 0;
    niveau->coupsRestants = 20;  // On a 20 coups pour réussir
    
    // On met le pseudo à vide pour l'instant (caractère de fin de chaîne)
    niveau->pseudo[0] = '\0'; 

    // 3. Initialisation du Contrat (Objectifs à 0 au début)
    // On parcourt le tableau des objectifs pour tout mettre à 0
    for (int k = 0; k <= NB_TYPES; k++) {
        niveau->contrat.quantiteCible[k] = 0;
        niveau->contrat.quantiteActuelle[k] = 0;
    }
    
    // Exemple : Pour le niveau 1, on veut tuer 10 items de type 1 (Rouge)
    // (Tu pourras changer ça plus tard pour faire des niveaux différents)
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
    // Variable temporaire pour stocker la valeur pendant l'échange
    Case temp;
    
    temp = *case1;       // On sauvegarde la case 1
    *case1 = *case2;     // La case 1 prend la valeur de la case 2
    *case2 = temp;       // La case 2 récupère la valeur sauvegardée
}