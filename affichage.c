#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // Indispensable pour tes potes
#include <time.h>
#include "affichage.h"

static HANDLE gH; // Handle de la console

/* --- OUTILS TECHNIQUES --- */

void initConsole(void) {
    gH = GetStdHandle(STD_OUTPUT_HANDLE);
    // Cache le curseur clignotant moche
    CONSOLE_CURSOR_INFO info = {100, FALSE}; 
    SetConsoleCursorInfo(gH, &info);
}

void color(int t, int f) {
    SetConsoleTextAttribute(gH, (WORD)(f * 16 + t));
}

void gotoligcol(int y, int x) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(gH, c);
}

void clearLineAt(int y, int x, int w) {
    gotoligcol(y, x);
    for (int i = 0; i < w; i++) putchar(' ');
}

int tempsRestantSec(Niveau *niveau) {
    int ecoule = (int)difftime(time(NULL), niveau->startTime);
    int r = niveau->tempsTotalSec - ecoule;
    return (r < 0) ? 0 : r;
}

/* --- FONCTIONS PRINCIPALES --- */

void afficherCadre(void) {
    system("cls");
    color(BLANC, NOIR);

    // Haut
    gotoligcol(0, 1);
    printf("|");
    for (int i = 0; i < COLONNES; i++) printf("---");
    printf("|");

    // Côtés
    for (int i = 0; i < LIGNES; i++) {
        gotoligcol(i + 1, 1);              printf("|");
        gotoligcol(i + 1, (COLONNES*3)+2); printf("|");
    }

    // Bas
    gotoligcol(LIGNES + 1, 1);
    printf("|");
    for (int i = 0; i < COLONNES; i++) printf("---");
    printf("|");
}

void afficherGrille(Niveau *niveau, int curseurX, int curseurY) {
    char sym[] = {'.','O','@','#','&','%'}; // Symboles
    int col[]  = {GRIS,ROUGE,BLEU,VERT,JAUNE,MAGENTA}; // Couleurs

    for (int i = 0; i < LIGNES; i++) {
        gotoligcol(i + 1, 2);
        for (int j = 0; j < COLONNES; j++) {
            
            // On récupère le type dans TA structure Case
            int type = niveau->grille[i][j].type;
            if (type > NB_TYPES) type = 0;

            int fg = col[type];
            int bg = NOIR;

            // Gestion Curseur & Sélection
            if (i == curseurY && j == curseurX) {
                // Si sélectionné : ROUGE, sinon GRIS
                bg = (niveau->grille[i][j].estSelectionne) ? ROUGE : GRIS;
                fg = BLANC;
            }
            else if (niveau->grille[i][j].estSelectionne) {
                // Sélectionné mais on est parti ailleurs
                bg = BLANC;
                fg = NOIR;
            }

            color(fg, bg);
            printf(" %c ", sym[type]);
        }
    }
    color(BLANC, NOIR);
}

void afficherHUD(Niveau *niveau) {
    int x = (COLONNES * 3) + 6; // Position à droite
    int y = 2;
    int w = 40; // Largeur effacement
    int t = tempsRestantSec(niveau);

    // --- Stats ---
    clearLineAt(y, x, w); gotoligcol(y, x);
    printf("NIVEAU %d", niveau->numeroNiveau);

    clearLineAt(y+2, x, w); gotoligcol(y+2, x);
    printf("Vies: ");
    color(ROUGE, NOIR);
    for(int i=0; i<niveau->vies; i++) printf("<3 ");
    color(BLANC, NOIR);

    clearLineAt(y+4, x, w); gotoligcol(y+4, x);
    printf("Score: %d", niveau->score);

    clearLineAt(y+5, x, w); gotoligcol(y+5, x);
    printf("Coups: %d", niveau->coupsRestants);

    clearLineAt(y+7, x, w); gotoligcol(y+7, x);
    color((t<30)?ROUGE:BLANC, NOIR);
    printf("Temps: %02d:%02d", t/60, t%60);
    color(BLANC, NOIR);

    // --- Objectifs ---
    clearLineAt(y+9, x, w); gotoligcol(y+9, x);
    printf("OBJECTIFS :");
    
    int ligne = y + 10;
    for(int k=1; k<=NB_TYPES; k++) {
        if (niveau->contrat.quantiteCible[k] > 0) {
            int fait = niveau->contrat.quantiteActuelle[k];
            int total = niveau->contrat.quantiteCible[k];
            
            clearLineAt(ligne, x, w); gotoligcol(ligne, x);
            
            if (fait >= total) color(VERT, NOIR);
            else color(BLANC, NOIR);
            
            printf("- Item %d : %d / %d", k, fait, total);
            ligne++;
        }
    }

    // --- Aide ---
    color(GRIS, NOIR);
    gotoligcol(LIGNES+2, 2);
    printf("[Fleches] Bouger | [ESPACE] Selection | [X] Quitter");
    color(BLANC, NOIR);
}