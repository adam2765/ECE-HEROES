#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // Indispensable pour tes potes
#include <time.h>
#include <conio.h>
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
   PARTIE MENU : DESIGN "PRO" ECE HEROES
   ========================================================= */

int afficherMenuPrincipal(void) {
    system("cls"); // Nettoyage écran

    // Dimensions approximatives de la console (pour le centrage)
    int width = 80;  // Largeur standard
    int height = 25; // Hauteur standard
    
    // --- 1. LE CADRE "CANDY" (Double bordure colorée) ---
    // On dessine un cadre autour de l'écran avec des couleurs alternées
    for (int i = 0; i < width; i++) {
        color((i % 5) + 9, NOIR); // Couleurs vives (9-14)
        gotoligcol(0, i); printf("=");
        gotoligcol(height-1, i); printf("=");
    }
    for (int i = 0; i < height; i++) {
        color((i % 5) + 9, NOIR);
        gotoligcol(i, 0); printf("|");
        gotoligcol(i, width-1); printf("|");
    }

    // --- 2. LE TITRE "ECE" (Gros et Centré) ---
    int y = 3;
    int x_titre = (width - 44) / 2; // Calcul savant pour centrer (44 est la largeur du texte)

    color(MAGENTA, NOIR);
    gotoligcol(y++, x_titre); printf("  ______   ______   ______ ");
    gotoligcol(y++, x_titre); printf(" |  ____| /  ____| |  ____|");
    gotoligcol(y++, x_titre); printf(" | |__    | |      | |__   ");
    gotoligcol(y++, x_titre); printf(" |  __|   | |      |  __|  ");
    gotoligcol(y++, x_titre); printf(" | |____  | |____  | |____ ");
    gotoligcol(y++, x_titre); printf(" |______| \\______| |______|");

    // --- 3. LE SOUS-TITRE "HEROES" (Juste en dessous) ---
    y++; // Petit espace
    color(CYAN, NOIR); // Changement de couleur
    // Le mot HEROES est un peu plus large (environ 50 chars)
    x_titre = (width - 50) / 2; 

    gotoligcol(y++, x_titre); printf(" _    _  ______  _____   ____   ______   _____ ");
    gotoligcol(y++, x_titre); printf("| |  | ||  ____||  __ \\ / __ \\ |  ____| / ____|");
    gotoligcol(y++, x_titre); printf("| |__| || |__   | |__) || |  | || |__   | (___  ");
    gotoligcol(y++, x_titre); printf("|  __  ||  __|  |  _  / | |  | ||  __|   \\___ \\ ");
    gotoligcol(y++, x_titre); printf("| |  | || |____ | | \\ \\ | |__| || |____  ____) |");
    gotoligcol(y++, x_titre); printf("|_|  |_||______||_|  \\_\\ \\____/ |______| |_____/ ");

    // --- 4. LE MENU SELECTEUR (Style "Boîte") ---
    y += 3; // On descend
    int x_menu = (width - 30) / 2; // Centrage du bloc menu

    // Haut de la boîte
    color(GRIS, NOIR);
    gotoligcol(y++, x_menu); printf(".__________________________.");

    // Option 1
    gotoligcol(y++, x_menu); printf("|                          |");
    gotoligcol(y, x_menu);   printf("|   ");
    color(VERT, NOIR);       printf("[1] NOUVELLE PARTIE");
    color(GRIS, NOIR);       printf("    |");
    y++;

    // Séparateur
    gotoligcol(y++, x_menu); printf("|   --------------------   |");

    // Option 2
    gotoligcol(y, x_menu);   printf("|   ");
    color(JAUNE, NOIR);      printf("[2] CHARGER PARTIE");
    color(GRIS, NOIR);       printf("     |");
    y++;

    // Séparateur
    gotoligcol(y++, x_menu); printf("|   --------------------   |");

    // Option 3
    gotoligcol(y, x_menu);   printf("|   ");
    color(ROUGE, NOIR);      printf("[3] QUITTER");
    color(GRIS, NOIR);       printf("            |");
    y++;

    // Bas de la boîte
    gotoligcol(y++, x_menu); printf("|__________________________|");

    // --- 5. FOOTER / CREDITS ---
    color(BLANC, NOIR);
    gotoligcol(height - 3, (width - 25) / 2);
    printf("Projet C - ECE Paris 2025");

    // --- 6. INPUT ---
    gotoligcol(y + 1, x_menu + 5);
    color(MAGENTA, NOIR);
    printf("Votre choix > ");
    color(BLANC, NOIR);

    // Lecture sécurisée
    char choix;
    do {
        choix = _getch();
    } while (choix < '1' || choix > '3');

    return choix - '0';
}