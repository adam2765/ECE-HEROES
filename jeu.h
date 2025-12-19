#ifndef JEU_H
#define JEU_H

#include "affichage.h"

// Prépare le niveau : remplit la grille avec des items aléatoires
void initialiserNiveau(GameState *niveau, int numNiveau);

// Echange le contenu de deux cases -> des pointeurs
void echangerCases(int *case1, int *case2);

// Prototypes des items
void itemBombe(GameState *niveau, int x, int y);
void itemLigne(GameState *niveau, int ligne);
void itemColonne(GameState *niveau, int colonne);
void itemCouleur(GameState *niveau, int type);

#endif