#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include "affichage.h"
#include "affichage_console.h" // la bibliothèque du cours

/* --- OUTILS INTERNES --- */

// calcule combien de temps il reste
int tempsRestantSec(Niveau *niveau) {
    // difference entre maintenant et le debut
    int ecoule = (int)difftime(time(NULL), niveau->startTime);
    int r = niveau->tempsTotalSec - ecoule;
    // si c'est negatif on renvoie 0
    return (r < 0) ? 0 : r;
}

// fonction pour effacer juste une zone de texte
void nettoyerZone(int lig, int col, int largeur) {
    gotoxy(col, lig); // on va a la position
    // on ecrit des espaces pour effacer
    for (int i = 0; i < largeur; i++) printf(" ");
}

/* --- FONCTIONS PRINCIPALES --- */

// affiche le grand cadre autour du jeu
void afficherCadre(void) {
    clrscr(); // efface l'ecran
    set_color(WHITE, BLACK); // couleur blanche

    int largeurCadre = 60; // largeur de la boite

    // --- Ligne du Haut ---
    gotoxy(0, 0);
    printf("|");
    for (int i = 0; i < largeurCadre; i++) printf("-");
    printf("|");

    // --- Bords GAUCHE et DROITE ---
    for (int i = 0; i < LIGNES; i++) {
        gotoxy(0, i + 1);                printf("|"); // mur gauche
        gotoxy(largeurCadre + 1, i + 1); printf("|"); // mur droit
    }

    // --- Ligne du Bas ---
    gotoxy(0, LIGNES + 1);
    printf("|");
    for (int i = 0; i < largeurCadre; i++) printf("-");
    printf("|");
}

// affiche la grille avec les emojis
void afficherGrille(Niveau *niveau, int curseurX, int curseurY) {
    // tableau des fruits (emojis)
    char *sym[] = {"  ", "🍉", "🫐", "🥝", "🍋", "🍑", ""}; 
    
    // on parcourt toutes les lignes
    for (int i = 0; i < LIGNES; i++) {
        gotoxy(2, i + 1); // on se place (x=2 car y'a le cadre)
        
        // on parcourt les colonnes
        for (int j = 0; j < COLONNES; j++) {
            
            int type = niveau->grille[i][j].type;
            if (type > NB_TYPES) type = 0; // securite

            int bg = BLACK; // fond noir par defaut

            // --- GESTION CURSEUR & SELECTION ---
            if (i == curseurY && j == curseurX) {
                bg = WHITE; // curseur blanc si on est dessus
            }
            else if (niveau->grille[i][j].estSelectionne) {
                bg = RED; // fond rouge si selectionné
            }

            set_color(WHITE, bg); // applique les couleurs
            printf(" %s ", sym[type]); // affiche le fruit
        }
    }
    set_color(WHITE, BLACK); // remet normal
}

// affiche les infos a droite (C'EST ICI QUE J'AVAIS FAIT L'ERREUR)
void afficherHUD(Niveau *niveau) {
    int x = 65; // position a droite
    int y = 2;  // hauteur de depart
    int w = 40; // largeur pour effacer
    int t = tempsRestantSec(niveau); // recupere temps

    // --- NIVEAU ---
    nettoyerZone(y, x, w); // efface ligne
    gotoxy(x, y);          // va a la ligne
    printf("NIVEAU %d", niveau->numeroNiveau);

    // --- VIES ---
    nettoyerZone(y+2, x, w); 
    gotoxy(x, y+2); // CORRIGÉ : on descend de 2 lignes
    printf("Vies: ");
    set_color(RED, BLACK);
    for(int i=0; i<niveau->vies; i++) printf("♥ "); // affiche coeurs
    set_color(WHITE, BLACK);

    // --- SCORE ---
    nettoyerZone(y+4, x, w); 
    gotoxy(x, y+4); // CORRIGÉ : on descend encore
    printf("Score: %d", niveau->score);

    // --- COUPS ---
    nettoyerZone(y+5, x, w); 
    gotoxy(x, y+5); // CORRIGÉ
    printf("Coups: %d", niveau->coupsRestants);

    // --- TEMPS ---
    nettoyerZone(y+7, x, w); 
    gotoxy(x, y+7); // CORRIGÉ
    // met en rouge si moins de 30 secondes
    if (t < 30) set_color(RED, BLACK);
    else set_color(WHITE, BLACK);
    
    printf("Temps: %02d:%02d", t/60, t%60); // affiche minutes:secondes
    set_color(WHITE, BLACK);

    // --- OBJECTIFS ---
    nettoyerZone(y+9, x, w); 
    gotoxy(x, y+9);
    printf("OBJECTIFS :");
    
    int ligne = y + 10;
    // affiche la liste des items a detruire
    for(int k=1; k<=NB_TYPES; k++) {
        if (niveau->contrat.quantiteCible[k] > 0) {
            int fait = niveau->contrat.quantiteActuelle[k];
            int total = niveau->contrat.quantiteCible[k];
            
            nettoyerZone(ligne, x, w); 
            gotoxy(x, ligne);
            
            // vert si fini, blanc sinon
            if (fait >= total) set_color(GREEN, BLACK);
            else set_color(WHITE, BLACK);
            
            printf("- Item %d : %d/%d", k, fait, total);
            ligne++; // ligne suivante
        }
    }

    // --- AIDE (EN BAS) ---
    set_color(DARKGRAY, BLACK);
    gotoxy(2, LIGNES + 4); 
    printf("[Fleches] Bouger | [ESPACE] Selection | [S] Sauver | [X] Quitter");
    set_color(WHITE, BLACK);
}

/* =========================================================
   PARTIE MENU
   ========================================================= */

int afficherMenuPrincipal(void) {
    clrscr(); // efface ecran

    int width = 80;
    int height = 25;
    
    // --- DESSIN DU CADRE ---
    for (int i = 0; i < width; i++) {
        set_color((i % 5) + 9, BLACK); // couleurs arc en ciel
        gotoxy(i, 0); printf("=");
        gotoxy(i, height-1); printf("=");
    }
    for (int i = 0; i < height; i++) {
        set_color((i % 5) + 9, BLACK);
        gotoxy(0, i); printf("|");
        gotoxy(width-1, i); printf("|");
    }

    // --- TITRE ASCII ART ---
    int y = 3;
    int x_titre = (width - 44) / 2; // centrage

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

    // --- BOITE DU MENU ---
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

    // Ligne vide
    gotoxy(x_menu, y++); printf("|   --------------------   |");

    // Option 2
    gotoxy(x_menu, y);   printf("|   ");
    set_color(YELLOW, BLACK);      printf("[2] CHARGER PARTIE");
    set_color(DARKGRAY, BLACK);       printf("     |");
    y++;

    // Ligne vide
    gotoxy(x_menu, y++); printf("|   --------------------   |");
     
    // Option 3
    gotoxy(x_menu, y);   printf("|   ");
    set_color(RED, BLACK);      printf("[3] QUITTER");
    set_color(DARKGRAY, BLACK);       printf("            |");
    y++;

    // Ligne vide
    gotoxy(x_menu, y++); printf("|   --------------------   |");

    // Option 4
    gotoxy(x_menu, y);   printf("|   ");
    set_color(CYAN, BLACK);      printf("[4] REGLES DU JEU");
    set_color(DARKGRAY, BLACK);       printf("      |");
    y++;

    gotoxy(x_menu, y++); printf("|__________________________|");

    // --- CREDITS EN BAS ---
    set_color(WHITE, BLACK);
    gotoxy((width - 25) / 2, height - 3);
    printf("Projet C - ECE Paris 2025");

    // --- ZONE DE SAISIE ---
    gotoxy(x_menu + 5, y + 1);
    set_color(MAGENTA, BLACK);
    printf("Votre choix > ");
    set_color(WHITE, BLACK);

    char choix;
    // boucle pour forcer 1, 2, 3 ou 4
    do {
        choix = _getch();
    } while (choix < '1' || choix > '4');

    return choix - '0'; // convertit char en int
}