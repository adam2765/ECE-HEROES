#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "affichage.h" // Lien indispensable vers ton header

// Variable globale interne au module (static = invisible pour les autres fichiers)
static HANDLE gH;

/* ================= OUTILS TECHNIQUES ================= */

// Initialise la console (cache le curseur clignotant)
// A appeler une seule fois au tout début du main de ton collègue
void initConsole(void) {
    gH = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info = {100, FALSE}; 
    SetConsoleCursorInfo(gH, &info);
}

// Change la couleur
void color(int t, int f) {
    SetConsoleTextAttribute(gH, (WORD)(f * 16 + t));
}

// Déplace le curseur d'écriture (Evite d'utiliser system("cls"))
void gotoligcol(int y, int x) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(gH, c);
}

// Efface une ligne spécifique (Pour le HUD dynamique)
void clearLineAt(int y, int x, int w) {
    gotoligcol(y, x);
    for (int i = 0; i < w; i++) putchar(' ');
}

int tempsRestantSec(const GameState *e) {
    int ecoule = (int)difftime(time(NULL), e->startTime);
    int r = e->tempsTotalSec - ecoule;
    return (r < 0) ? 0 : r;
}

/* ================= FONCTIONS D'AFFICHAGE ================= */

// Affiche le décor fixe (Cadre). A appeler AVANT la boucle de jeu.
void afficherCadre(void) {
    system("cls"); // Seul nettoyage global autorisé au lancement
    color(BLANC, NOIR);

    // Bordure Haute
    gotoligcol(0, 1);
    printf("|");
    for (int i = 0; i < NB_COL; i++) printf("-");
    printf("|");

    // Bordures Latérales
    for (int i = 0; i < NB_LIG; i++) {
        gotoligcol(i + 1, 1);          printf("|");
        gotoligcol(i + 1, NB_COL + 2); printf("|");
    }

    // Bordure Basse
    gotoligcol(NB_LIG + 1, 1);
    printf("|");
    for (int i = 0; i < NB_COL; i++) printf("-");
    printf("|");

    color(BLANC, NOIR);
}

// Affiche le contenu du plateau. A appeler DANS la boucle de jeu.
void afficherGrille(const GameState *e) {
    // Symboles items (A, B, C...) et couleurs correspondantes
    char sym[] = {'.','A','B','C','D','E'}; 
    int col[]  = {GRIS,ROUGE,VERT,BLEU,JAUNE,MAGENTA};

    for (int i = 0; i < NB_LIG; i++) {
        gotoligcol(i + 1, 2);
        for (int j = 0; j < NB_COL; j++) {
            int v = e->grille[i][j];
            if (v < 0 || v > NB_ITEMS) v = 0;

            int bg = NOIR, fg = col[v];
            
            // Gestion visuelle du curseur (Gris = Survol, Rouge = Sélection)
            if (i == e->cursorY && j == e->cursorX) {
                bg = e->isSelected ? ROUGE : GRIS;
                fg = BLANC;
            }

            color(fg, bg);
            putchar(sym[v]);
        }
    }
    color(BLANC, NOIR);
}

// Affiche les infos latérales. A appeler DANS la boucle de jeu.
void afficherHUD(const GameState *e) {
    const int x = NB_COL + 6; // Position à droite du cadre
    const int y = 2;
    const int W = 70; // Largeur de nettoyage pour effacer les anciens textes

    int t = tempsRestantSec(e);

    // --- Infos Partie ---
    clearLineAt(y, x, W); gotoligcol(y, x);
    printf("Niveau : %d", e->niveauActuel);

    clearLineAt(y + 2, x, W); gotoligcol(y + 2, x);
    printf("Vies : ");
    color(ROUGE, NOIR);
    for (int i = 0; i < e->vies; i++) printf("<3 ");
    color(BLANC, NOIR);

    clearLineAt(y + 4, x, W); gotoligcol(y + 4, x);
    printf("Score : %d", e->score);

    clearLineAt(y + 5, x, W); gotoligcol(y + 5, x);
    printf("Coups restants : %d", e->coupsRestants);

    clearLineAt(y + 6, x, W); gotoligcol(y + 6, x);
    color(t < 30 ? ROUGE : BLANC, NOIR); // Alerte rouge si temps < 30s
    printf("Temps restant : %02d:%02d", t / 60, t % 60);
    color(BLANC, NOIR);

    // --- Contrat (Objectifs) ---
    clearLineAt(y + 8, x, W); gotoligcol(y + 8, x);
    printf("CONTRAT (Elimines / Objectif / Reste):");

    int l = y + 9;
    for (int i = 1; i <= NB_ITEMS; i++) {
        if (e->contrat[i] > 0) {
            int reste = e->contrat[i] - e->elimines[i];
            if (reste < 0) reste = 0;

            clearLineAt(l, x, W); gotoligcol(l, x);
            
            if (reste == 0) color(VERT, NOIR); // Vert = Objectif atteint
            else color(BLANC, NOIR);
            
            printf("Item %c : %d / %d  (Reste %d)", 'A'+(i-1), e->elimines[i], e->contrat[i], reste);
            l++;
        }
    }
    color(BLANC, NOIR);

    // Nettoyage esthétique en dessous
    for(int k=0; k<3; k++) clearLineAt(l+k, x, W);

    // --- Pied de page (Aide) ---
    clearLineAt(NB_LIG - 1, x, W); gotoligcol(NB_LIG - 1, x);
    color(GRIS, NOIR);
    printf("[Fleches/ZQSD] Bouger | [ESPACE] Selection | [4] Quitter");
    color(BLANC, NOIR);
}
