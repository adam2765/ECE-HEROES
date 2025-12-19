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

/* =========================================================
   PARTIE MENU : LE VISUEL STYLÉ
   ========================================================= */

int afficherMenuPrincipal(void) {
    system("cls"); // On nettoie l'écran pour afficher le menu proprement
    
    // Coordonnées pour centrer un peu le menu
    int x_menu = 30; 
    int y_menu = 10;

    // --- 1. LE TITRE (ASCII ART) ---
    // Affiche "CANDY" en gros avec des couleurs alternées
    color(MAGENTA, NOIR);
    printf("\n\n");
    printf("        ______   _____  ______  _    _  ______  _____ \n");
    printf("       |  ____| / ____||  ____|| |  | ||  ____||  __ \\\n");
    color(CYAN, NOIR); // Petit effet dégradé
    printf("       | |__   | |     | |__   | |__| || |__   | |__) |\n");
    printf("       |  __|  | |     |  __|  |  __  ||  __|  |  _  / \n");
    color(MAGENTA, NOIR);
    printf("       | |____ | |____ | |____ | |  | || |____ | | \\ \\ \n");
    printf("       |______| \\_____||______||_|  |_||______||_|  \\_\\\n");
    printf("\n");
    
    // --- 2. LE CADRE DES OPTIONS ---
    color(BLANC, NOIR);
    
    // Haut du cadre
    gotoligcol(y_menu - 2, x_menu); printf("========================================");
    gotoligcol(y_menu - 1, x_menu); printf("|                                      |");
    
    // Option 1 : JOUER (Vert)
    gotoligcol(y_menu, x_menu);     printf("|      ");
    color(VERT, NOIR);              printf("[1] NOUVELLE PARTIE");
    color(BLANC, NOIR);             printf("             |");

    gotoligcol(y_menu + 1, x_menu); printf("|                                      |");

    // Option 2 : CHARGER (Jaune)
    gotoligcol(y_menu + 2, x_menu); printf("|      ");
    color(JAUNE, NOIR);             printf("[2] CHARGER PARTIE");
    color(BLANC, NOIR);             printf("              |");

    gotoligcol(y_menu + 3, x_menu); printf("|                                      |");

    // Option 3 : QUITTER (Rouge)
    gotoligcol(y_menu + 4, x_menu); printf("|      ");
    color(ROUGE, NOIR);             printf("[3] QUITTER");
    color(BLANC, NOIR);             printf("                     |");

    // Bas du cadre
    gotoligcol(y_menu + 5, x_menu); printf("|                                      |");
    gotoligcol(y_menu + 6, x_menu); printf("========================================");

    // --- 3. PETITE INFO EN BAS ---
    color(GRIS, NOIR);
    gotoligcol(y_menu + 8, x_menu + 5); 
    printf("Projet C - ECE Paris 2025");
    
    // --- 4. GESTION DU CHOIX ---
    gotoligcol(y_menu + 10, x_menu + 5);
    color(BLANC, NOIR);
    printf("Votre choix > ");
    
    // Lecture sécurisée : on attend tant que l'utilisateur ne tape pas 1, 2 ou 3
    char choix;
    do {
        choix = _getch(); // _getch() lit sans attendre "Entrée"
    } while (choix < '1' || choix > '3');
    
    // On renvoie un entier (1, 2 ou 3) pour que le main.c sache quoi faire
    return choix - '0'; 
}