#ifndef JEU_H
#define JEU_H

#include "structure.h"

// Prépare le niveau : remplit la grille avec des items aléatoires
void initialiserNiveau(Niveau *niveau, int numNiveau);

// Echange le contenu de deux cases -> des pointeurs
void echangerCases(Case *case1, Case *case2);

#endif