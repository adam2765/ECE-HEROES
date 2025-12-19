#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include "structure.h"

// --- COULEURS WINDOWS ---
// Codes simplifiés pour ne pas s'embêter avec les chiffres bizarres
#define NOIR 0
#define BLEU 1
#define VERT 2
#define CYAN 3
#define ROUGE 4
#define MAGENTA 5
#define JAUNE 6
#define BLANC 15
#define GRIS 8

// --- OUTILS TECHNIQUES (WINDOWS) ---

// Initialise la console (cache le curseur clignotant)
void initConsole(void);

// Change la couleur du texte (t) et du fond (f)
void color(int t, int f);

// Déplace le curseur à la position (x, y)
void gotoligcol(int y, int x);

// Efface une ligne à partir d'une position donnée (pour nettoyer le texte)
void clearLineAt(int y, int x, int w);

// --- FONCTIONS D'AFFICHAGE DU JEU ---

// Affiche le cadre décoratif autour du jeu (à appeler une seule fois au début)
void afficherCadre(void);

// Affiche la grille de bonbons (met à jour les couleurs et sélections)
void afficherGrille(Niveau *niveau, int curseurX, int curseurY);

// Affiche les infos sur le côté (Score, Vies, Temps, Objectifs...)
void afficherHUD(Niveau *niveau);

// --- MENU PRINCIPAL ---

// Affiche le menu stylé et renvoie le choix du joueur (1, 2 ou 3)
int afficherMenuPrincipal(void);

#endif