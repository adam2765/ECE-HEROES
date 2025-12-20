#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include "structure.h"

// Note : Les couleurs sont maintenant gérées par affichage_console.h

// --- FONCTIONS D'AFFICHAGE DU JEU ---

// Affiche le cadre décoratif autour du jeu
void afficherCadre(void);

// Affiche la grille de bonbons
void afficherGrille(Niveau *niveau, int curseurX, int curseurY);

// Affiche les infos sur le côté
void afficherHUD(Niveau *niveau);

// --- MENU PRINCIPAL ---
int afficherMenuPrincipal(void);

#endif