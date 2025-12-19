#ifndef JEU_H
#define JEU_H

#include "structure.h"

// Prépare le niveau : remplit la grille avec des items aléatoires
void initialiserNiveau(Niveau *niveau, int numNiveau);

// Echange le contenu de deux cases -> des pointeurs
void echangerCases(Case *case1, Case *case2);

// Prototypes des items
void itemBombe(Niveau *niveau, int x, int y);
void itemLigne(Niveau *niveau, int ligne);
void itemColonne(Niveau *niveau, int colonne);
void itemCouleur(Niveau *niveau, int type);

#endif