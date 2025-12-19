#include <stdio.h>
#include <stdlib.h>
#include <windows.h> 
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

    // LARGEUR ORIGINALE (Compacte)
    // 55 caractères suffisent pour la grille de fruits
    int largeur = 55; 

    // --- Ligne du Haut ---
    gotoligcol(0, 0);
    printf("+");
    for (int i = 0; i < largeur; i++) printf("-");
    printf("+");

    // --- Murs Gauche et Droite ---
    // On descend jusqu'à 25 pour encadrer le menu du bas
    for (int i = 0; i < 25; i++) {
        gotoligcol(i + 1, 0);           printf("|"); 
        gotoligcol(i + 1, largeur + 1); printf("|"); 
    }

    // --- Ligne du Bas ---
    gotoligcol(26, 0); 
    printf("+");
    for (int i = 0; i < largeur; i++) printf("-");
    printf("+");
}

void afficherGrille(Niveau *niveau, int curseurX, int curseurY) {
    // 1. LES FRUITS (Emojis)
    // Index: 0=Vide, 1=Rouge, 2=Bleu, 3=Vert, 4=Jaune, 5=Magenta
    // Note : On met des guillemets doubles " "
    char *sym[] = {"  ", "🍉", "🫐", "🥝", "🍋", "🍑", ""}; 
    
    // 2. COULEURS DE FOND UNIQUEMENT
    
    for (int i = 0; i < LIGNES; i++) {
        gotoligcol(i + 1, 2); // On se place
        
        for (int j = 0; j < COLONNES; j++) {
            
            int type = niveau->grille[i][j].type;
            if (type > NB_TYPES) type = 0;

            int bg = NOIR;      // Fond par défaut

            // --- GESTION CURSEUR & SELECTION ---
            if (i == curseurY && j == curseurX) {
                // Curseur blanc sur la case actuelle
                bg = BLANC; 
            }
            else if (niveau->grille[i][j].estSelectionne) {
                // Fond Rouge si on a cliqué (sélectionné)
                bg = ROUGE; 
            }

            // On applique la couleur (le texte reste blanc, le fond change)
            color(BLANC, bg);
            
            // AFFICHE L'EMOJI
            // %s car c'est une chaîne de caractères (UTF-8)
            // On met un espace avant/après pour aérer la grille
            printf(" %s ", sym[type]); 
        }
    }
    color(BLANC, NOIR); // On remet normal à la fin
}

// --- CORRECTION : J'AI SORTI LA FONCTION D'ICI ---
// Petite fonction locale pour effacer proprement avant d'écrire
void effacerZone(int lig, int col){
    gotoligcol(lig, col);
    printf("                               "); // 30 espaces vides
    gotoligcol(lig, col); // On revient pour écrire
}
// -------------------------------------------------

void afficherHUD(Niveau *niveau) {
    // ON COLLE LE TEXTE JUSTE A DROITE DU CADRE
    // Le cadre finit à 55, donc on écrit à 60
    int x = 60; 
    
    int y = 2; 
    int t = tempsRestantSec(niveau);

    // Fonction de nettoyage (à laisser ici ou sortir selon ton compilateur)
    // Si tu as une erreur, mets cette fonction EN DEHORS de afficherHUD
    void effacerZone(int lig, int col){
        gotoligcol(lig, col);
        printf("                        "); // Nettoyage court
        gotoligcol(lig, col);
    }
    
    // --- Stats ---
    effacerZone(y, x);
    color(JAUNE, NOIR);
    printf("NIVEAU %d", niveau->numeroNiveau);

    effacerZone(y+2, x);
    color(BLANC, NOIR);
    printf("Vies: ");
    color(ROUGE, NOIR);
    for(int i=0; i<niveau->vies; i++) printf("♥ "); 
    color(BLANC, NOIR);

    effacerZone(y+4, x);
    printf("Score: %d", niveau->score);

    effacerZone(y+5, x);
    printf("Coups: %d", niveau->coupsRestants);

    effacerZone(y+7, x);
    color((t<30)?ROUGE:BLANC, NOIR);
    printf("Temps: %02d:%02d", t/60, t%60);
    color(BLANC, NOIR);

    // --- Objectifs ---
    effacerZone(y+9, x);
    color(CYAN, NOIR);
    printf("OBJECTIFS :");
    color(BLANC, NOIR);
    
    int ligne = y + 10;
    for(int k=1; k<=NB_TYPES; k++) {
        if (niveau->contrat.quantiteCible[k] > 0) {
            int fait = niveau->contrat.quantiteActuelle[k];
            int total = niveau->contrat.quantiteCible[k];
            
            effacerZone(ligne, x);
            
            if (fait >= total) color(VERT, NOIR);
            else color(BLANC, NOIR);
            
            // CORRECTION ALIGNEMENT : on utilise %2d pour que les chiffres s'alignent
            // Exemple : " 9 / 10" au lieu de "9 / 10" qui décale tout
            printf("- Item %d : %2d / %2d", k, fait, total);
            ligne++;
        }
    }

    // --- Aide (En bas, centrée sous la grille) ---
    color(GRIS, NOIR);
    gotoligcol(24, 2); 
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