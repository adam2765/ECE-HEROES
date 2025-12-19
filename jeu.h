#ifndef JEU_H
#define JEU_H

#include "structure.h"

// --- FONCTIONS DE BASE ---

// Prépare le niveau : remplit la grille, lance le chrono, met le score à 0
void initialiserNiveau(Niveau *niveau, int numNiveau);

// Echange le contenu de deux cases (utilise des pointeurs de Case)
void echangerCases(Case *case1, Case *case2);

// --- FONCTIONS BONUS (ITEMS DE TON COLLEGUE) ---
// Adaptées pour utiliser la structure 'Niveau'

// Détruit une zone de 3x3 autour de la case (x,y)
void itemBombe(Niveau *niveau, int x, int y);

// Détruit toute la ligne indiquée
void itemLigne(Niveau *niveau, int ligne);

// Détruit toute la colonne indiquée
void itemColonne(Niveau *niveau, int colonne);

// Détruit toutes les cases d'une certaine couleur/type sur tout le plateau
void itemCouleur(Niveau *niveau, int type);

// --- LOGIQUE MATCH-3 ---

// 1. Détecte les alignements (3 ou +), supprime les bonbons et met à jour le score/contrat
// Retourne 1 si des bonbons ont été détruits, 0 sinon
int detecterEtSupprimerAlignements(Niveau *niveau);

// 2. Fait tomber les bonbons du haut vers le bas (Gravité) et remplit les trous
void faireTomberEtRemplir(Niveau *niveau);

// 3. Fonction Mère : Enchaine détection et gravité tant qu'il y a des combos
void gererCombos(Niveau *niveau);

#endif