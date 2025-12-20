#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "affichage.h"
#include "affichage_console.h" // Bibliothèque du prof

/* --- OUTILS INTERNES --- */

int tempsRestantSec(Niveau *niveau) {
    int ecoule = (int)difftime(time(NULL), niveau->startTime);
    int r = niveau->tempsTotalSec - ecoule;
    return (r < 0) ? 0 : r;
}

// Fonction pour nettoyer une zone précise (remplace ton ancien clearLineAt)
void nettoyerZone(int lig, int col, int largeur) {
    gotoxy(col, lig); // Attention : X d'abord, Y ensuite
    for (int i = 0; i < largeur; i++) printf(" ");
}

/* --- FONCTIONS PRINCIPALES --- */

void afficherCadre(void) {
    clrscr(); // Fonction du prof
    set_color(WHITE, BLACK);

    int largeurCadre = 60; 

    // --- Ligne du Haut ---
    gotoxy(0, 0);
    printf("|");
    for (int i = 0; i < largeurCadre; i++) printf("-");
    printf("|");

    // --- Bords GAUCHE et DROITE ---
    for (int i = 0; i < LIGNES; i++) {
        gotoxy(0, i + 1);              printf("|"); // Bord Gauche
        gotoxy(largeurCadre + 1, i + 1); printf("|"); // Bord Droit
    }

    // --- Ligne du Bas ---
    gotoxy(0, LIGNES + 1);
    printf("|");
    for (int i = 0; i < largeurCadre; i++) printf("-");
    printf("|");
}

void afficherGrille(Niveau *niveau, int curseurX, int curseurY) {
    // 1. LES FRUITS
    char *sym[] = {"  ", "🍉", "🫐", "🥝", "🍋", "🍑", ""}; 
    
    // 2. PARCOURS
    for (int i = 0; i < LIGNES; i++) {
        gotoxy(2, i + 1); // X=2, Y=i+1
        
        for (int j = 0; j < COLONNES; j++) {
            
            int type = niveau->grille[i][j].type;
            if (type > NB_TYPES) type = 0;

            int bg = BLACK;      // Fond par défaut

            // --- GESTION CURSEUR & SELECTION ---
            if (i == curseurY && j == curseurX) {
                bg = WHITE; 
            }
            else if (niveau->grille[i][j].estSelectionne) {
                bg = RED; 
            }

            set_color(WHITE, bg);
            printf(" %s ", sym[type]); 
        }
    }
    set_color(WHITE, BLACK);
}

void afficherHUD(Niveau *niveau) {
    int x = 65; // Ta position fixe
    int y = 2;
    int w = 40; // Largeur effacement
    int t = tempsRestantSec(niveau);

    // --- Stats ---
    nettoyerZone(y, x, w); gotoxy(x, y);
    printf("NIVEAU %d", niveau->numeroNiveau);

    nettoyerZone(y+2, x, w); gotoxy(x, y);
    printf("Vies: ");
    set_color(RED, BLACK);
    for(int i=0; i<niveau->vies; i++) printf("♥ ");
    set_color(WHITE, BLACK);

    nettoyerZone(y+4, x, w); gotoxy(x, y);
    printf("Score: %d", niveau->score);

    nettoyerZone(y+5, x, w); gotoxy(x, y);
    printf("Coups: %d", niveau->coupsRestants);

    nettoyerZone(y+7, x, w); gotoxy(x, y);
    if (t < 30) set_color(RED, BLACK);
    else set_color(WHITE, BLACK);
    
    printf("Temps: %02d:%02d", t/60, t%60);
    set_color(WHITE, BLACK);

    // --- Objectifs ---
    nettoyerZone(y+9, x, w); gotoxy(x, y+9);
    printf("OBJECTIFS :");
    
    int ligne = y + 10;
    for(int k=1; k<=NB_TYPES; k++) {
        if (niveau->contrat.quantiteCible[k] > 0) {
            int fait = niveau->contrat.quantiteActuelle[k];
            int total = niveau->contrat.quantiteCible[k];
            
            nettoyerZone(ligne, x, w); gotoxy(x, ligne);
            
            if (fait >= total) set_color(GREEN, BLACK);
            else set_color(WHITE, BLACK);
            
            printf("- Item %d : %d/%d", k, fait, total);
            ligne++;
        }
    }

    // --- Aide ---
    set_color(DARKGRAY, BLACK);
    gotoxy(2, LIGNES + 4); 
    printf("[Fleches] Bouger | [ESPACE] Selection | [S] Sauver | [X] Quitter");
    set_color(WHITE, BLACK);
}

/* =========================================================
   PARTIE MENU
   ========================================================= */

int afficherMenuPrincipal(void) {
    clrscr(); 

    int width = 80;
    int height = 25;
    
    // --- CADRE ---
    for (int i = 0; i < width; i++) {
        set_color((i % 5) + 9, BLACK);
        gotoxy(i, 0); printf("=");
        gotoxy(i, height-1); printf("=");
    }
    for (int i = 0; i < height; i++) {
        set_color((i % 5) + 9, BLACK);
        gotoxy(0, i); printf("|");
        gotoxy(width-1, i); printf("|");
    }

    // --- TITRE ---
    int y = 3;
    int x_titre = (width - 44) / 2; 

    set_color(MAGENTA, BLACK);
    gotoxy(x_titre, y++); printf("  ______   ______   ______ ");
    gotoxy(x_titre, y++); printf(" |  ____| /  ____| |  ____|");
    gotoxy(x_titre, y++); printf(" | |__    | |      | |__   ");
    gotoxy(x_titre, y++); printf(" |  __|   | |      |  __|  ");
    gotoxy(x_titre, y++); printf(" | |____  | |____  | |____ ");
    gotoxy(x_titre, y++); printf(" |______| \\______| |______|");

    y++; 
    set_color(CYAN, BLACK); 
    x_titre = (width - 50) / 2; 

    gotoxy(x_titre, y++); printf(" _    _  ______  _____   ____   ______   _____ ");
    gotoxy(x_titre, y++); printf("| |  | ||  ____||  __ \\ / __ \\ |  ____| / ____|");
    gotoxy(x_titre, y++); printf("| |__| || |__   | |__) || |  | || |__   | (___  ");
    gotoxy(x_titre, y++); printf("|  __  ||  __|  |  _  / | |  | ||  __|   \\___ \\ ");
    gotoxy(x_titre, y++); printf("| |  | || |____ | | \\ \\ | |__| || |____  ____) |");
    gotoxy(x_titre, y++); printf("|_|  |_||______||_|  \\_\\ \\____/ |______| |_____/ ");

    // --- MENU ---
    y += 3; 
    int x_menu = (width - 30) / 2; 

    set_color(DARKGRAY, BLACK);
    gotoxy(x_menu, y++); printf(".__________________________.");

    // Option 1
    gotoxy(x_menu, y++); printf("|                          |");
    gotoxy(x_menu, y);   printf("|   ");
    set_color(GREEN, BLACK);       printf("[1] NOUVELLE PARTIE");
    set_color(DARKGRAY, BLACK);       printf("    |");
    y++;

    // Séparateur
    gotoxy(x_menu, y++); printf("|   --------------------   |");

    // Option 2
    gotoxy(x_menu, y);   printf("|   ");
    set_color(YELLOW, BLACK);      printf("[2] CHARGER PARTIE");
    set_color(DARKGRAY, BLACK);       printf("     |");
    y++;

    // Séparateur
    gotoxy(x_menu, y++); printf("|   --------------------   |");
     
    // Option 3
    gotoxy(x_menu, y);   printf("|   ");
    set_color(RED, BLACK);      printf("[3] QUITTER");
    set_color(DARKGRAY, BLACK);       printf("            |");
    y++;

    // Séparateur
    gotoxy(x_menu, y++); printf("|   --------------------   |");

    // Option 4
    gotoxy(x_menu, y);   printf("|   ");
    set_color(CYAN, BLACK);      printf("[4] REGLES DU JEU");
    set_color(DARKGRAY, BLACK);       printf("      |");
    y++;

    gotoxy(x_menu, y++); printf("|__________________________|");

    // --- FOOTER ---
    set_color(WHITE, BLACK);
    gotoxy((width - 25) / 2, height - 3);
    printf("Projet C - ECE Paris 2025");

    // --- INPUT ---
    gotoxy(x_menu + 5, y + 1);
    set_color(MAGENTA, BLACK);
    printf("Votre choix > ");
    set_color(WHITE, BLACK);

    char choix;
    do {
        choix = _getch();
    } while (choix < '1' || choix > '4');

    return choix - '0';
}