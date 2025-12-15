#include <stdio.h>
#include <stdlib.h> // Pour rand() et srand()
#include <time.h>   // Pour time()
#include "jeu.h"

void initialiserNiveau(Niveau *niveau, int numNiveau) {
    // 1. On active le générateur de nombres aléatoires
    // (Sinon ce sera toujours la même grille à chaque lancement !)
    srand(time(NULL));

    // 2. On remplit les infos de base
    niveau->numeroNiveau = numNiveau;
    niveau->vies = 3;            // Donné par le sujet (page 4)
    niveau->score = 0;
    niveau->coupsRestants = 20;  // Exemple, à adapter selon difficulté
    
    // 3. On remplit la grille case par case
    for(int i = 0; i < LIGNES; i++) {
        for(int j = 0; j < COLONNES; j++) {
            
            // Tirage au sort entre 1 et NB_TYPES (donc entre 1 et 5)
            int typeAleatoire = (rand() % NB_TYPES) + 1;
            
            niveau->grille[i][j].type = typeAleatoire;
            niveau->grille[i][j].estSelectionne = 0; // Pas sélectionné au début
        }
    }
    
    // NOTE : Plus tard, il faudra ajouter ici une boucle pour vérifier 
    // qu'il n'y a pas déjà 3 items alignés (interdit au début selon le sujet).
    // Mais pour l'instant, on veut juste voir si ça s'affiche !
}