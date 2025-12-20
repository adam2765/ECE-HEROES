#ifndef JEU_H
#define JEU_H

#include "structure.h"

// --- FONCTIONS DE BASE ---

// Prépare le niveau : remplit la grille, lance le chrono, met le score à 0
void initialiserNiveau(Niveau *niveau, int numNiveau);

// Echange le contenu de deux cases (utilise des pointeurs de Case)
void echangerCases(Case *case1, Case *case2);

// --- EFFETS SPÉCIAUX (Figures du CDC) ---

// Détruit une zone de 3x3 autour de la case (x,y) [EXTENSION niveau 2+]
void effetBombe(Niveau *niveau, int x, int y);

// Détruit toute la ligne indiquée (Croix de 9)
void effetLigne(Niveau *niveau, int ligne);

// Détruit toute la colonne indiquée (Croix de 9)
void effetColonne(Niveau *niveau, int colonne);

// Détruit toutes les cases d'une certaine couleur/type (6 alignés)
void effetCouleur(Niveau *niveau, int type);

// Détruit un carré 4x4 (Carré 4x4)
void effetCarre4x4(Niveau *niveau, int x, int y);

// --- LOGIQUE MATCH-3 ---

// Détecte les alignements (3 ou +), supprime les fruits et met à jour le score/contrat
// Retourne 1 si des fruits ont été détruits, 0 sinon
int detecterEtSupprimerAlignements(Niveau *niveau);

// Fait tomber les fruits du haut vers le bas (Gravité) et remplit les trous
void faireTomberEtRemplir(Niveau *niveau);

// Fonction Mère : Enchaine détection et gravité tant qu'il y a des combos
void gererCombos(Niveau *niveau);

// --- VERIFICATION VICTOIRE ---

// Vérifie si tous les objectifs du contrat sont atteints
// Retourne 1 si victoire, 0 sinon
int verifierContrat(Niveau *niveau);


#endif
