#include <stdio.h>
#include <stdlib.h> // Pour rand() et srand()
#include <time.h>   // Pour time()
#include "jeu.h"

// Fonction pour démarrer la partie et remplir les données
void initialiserNiveau(Niveau *niveau, int numNiveau) {
    // 1. On active le hasard
    srand(time(NULL));

    // 2. Initialisation des variables simples
    niveau->numeroNiveau = numNiveau;
    niveau->vies = 3;            
    niveau->score = 0;
    niveau->coupsRestants = 20; 
    
    // --- IMPORTANT POUR L'AFFICHAGE ---
    niveau->tempsTotalSec = 120;     // 2 minutes
    niveau->startTime = time(NULL);  // Top départ
    // ----------------------------------

    // On met le pseudo à vide
    niveau->pseudo[0] = '\0'; 

    // 3. Initialisation du Contrat (Objectifs à 0 au début)
    for (int k = 0; k <= NB_TYPES; k++) {
        niveau->contrat.quantiteCible[k] = 0;
        niveau->contrat.quantiteActuelle[k] = 0;
    }
    
    // Contrats différents selon le niveau (difficulté croissante)
    if (numNiveau == 1) {
        niveau->contrat.quantiteCible[ITEM_1] = 10;
        niveau->coupsRestants = 30;
        niveau->tempsTotalSec = 120; // 2 min
    }
    else if (numNiveau == 2) {
        niveau->contrat.quantiteCible[ITEM_1] = 15;
        niveau->contrat.quantiteCible[ITEM_2] = 15;
        niveau->coupsRestants = 25;
        niveau->tempsTotalSec = 90; // 1 min 30
    }
    else if (numNiveau == 3) {
        niveau->contrat.quantiteCible[ITEM_1] = 20;
        niveau->contrat.quantiteCible[ITEM_2] = 20;
        niveau->contrat.quantiteCible[ITEM_3] = 10;
        niveau->coupsRestants = 20;
        niveau->tempsTotalSec = 60; // 1 min
    }

    // 4. Remplissage de la grille avec des items aléatoires
    for(int i = 0; i < LIGNES; i++) {
        for(int j = 0; j < COLONNES; j++) {
            
            // On tire un nombre entre 1 et 5
            int typeAleatoire = (rand() % NB_TYPES) + 1;
            
            niveau->grille[i][j].type = typeAleatoire;
            niveau->grille[i][j].estSelectionne = 0; // Pas sélectionné
        }
    }
}

// Fonction pour échanger deux cases (Chapitre 11 : Pointeurs)
void echangerCases(Case *case1, Case *case2) {
    Case temp;
    temp = *case1;       // Sauvegarde
    *case1 = *case2;     // Echange
    *case2 = temp;       // Restauration
}

// ============== EFFETS SPÉCIAUX (Figures du CDC) ==============

// EFFET BOMBE : Détruit zone 3x3 autour de (x,y) [EXTENSION niveau 2+]
void effetBombe(Niveau *niveau, int x, int y) {
    // On parcourt les cases autour (i, j)
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            // On vérifie qu'on ne sort pas du tableau (Sécurité)
            if (i >= 0 && i < LIGNES && j >= 0 && j < COLONNES) {
                niveau->grille[i][j].type = VIDE;
                niveau->score += 10; 
            }
        }
    }
}

// EFFET LIGNE : Supprimer une ligne entière (Croix de 9 - CDC page 5)
void effetLigne(Niveau *niveau, int ligne) {
    // Sécurité
    if (ligne < 0 || ligne >= LIGNES) return;

    for (int j = 0; j < COLONNES; j++) {
        if (niveau->grille[ligne][j].type != VIDE) {
            niveau->grille[ligne][j].type = VIDE;
            niveau->score += 10;
        }
    }
}

// EFFET COLONNE : Supprimer une colonne entière (Croix de 9 - CDC page 5)
void effetColonne(Niveau *niveau, int colonne) {
    // Sécurité
    if (colonne < 0 || colonne >= COLONNES) return;

    for (int i = 0; i < LIGNES; i++) {
        if (niveau->grille[i][colonne].type != VIDE) {
            niveau->grille[i][colonne].type = VIDE;
            niveau->score += 10;
        }
    }
}

// EFFET COULEUR : Supprimer toutes les cases d'un type (6 alignés - CDC page 5)
void effetCouleur(Niveau *niveau, int type) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            // On compare avec .type
            if (niveau->grille[i][j].type == type) {
                niveau->grille[i][j].type = VIDE;
                niveau->score += 10;
            }
        }
    }
}

// EFFET CARRE : Supprimer un carré 4x4 (Carré 4x4 - CDC page 5)
void effetCarre4x4(Niveau *niveau, int x, int y) {
    for (int i = x; i < x + 4 && i < LIGNES; i++) {
        for (int j = y; j < y + 4 && j < COLONNES; j++) {
            if (niveau->grille[i][j].type != VIDE) {
                niveau->grille[i][j].type = VIDE;
                niveau->score += 10;
            }
        }
    }
}

/* =========================================================
   MOTEUR MATCH-3 (DESTRUCTION & GRAVITÉ)
   ========================================================= */

// Fonction interne pour supprimer une case proprement
void supprimerCase(Niveau *niveau, int i, int j) {
    int type = niveau->grille[i][j].type;
    
    // Si c'est déjà vide, on ne fait rien
    if (type == VIDE) return;

    // 1. On met à jour le contrat (Objectifs)
    if (type <= NB_TYPES) { // Si c'est un fruit normal
        niveau->contrat.quantiteActuelle[type]++;
    }

    // 2. On augmente le score
    niveau->score += 10;

    // 3. On vide la case
    niveau->grille[i][j].type = VIDE;
    niveau->grille[i][j].estSelectionne = 0;
}

// Détecte une croix de 9 items identiques (CDC page 5)
// Une croix = 5 horizontal + 5 vertical avec centre commun
int detecterCroix(Niveau *niveau) {
    int modif = 0;
    
    // Une croix a besoin d'au moins 2 cases de chaque côté du centre
    for (int i = 2; i < LIGNES - 2; i++) {
        for (int j = 2; j < COLONNES - 2; j++) {
            int t = niveau->grille[i][j].type;
            if (t == VIDE) continue;
            
            // Vérifie si on a une croix complète
            int haut = (niveau->grille[i-1][j].type == t && niveau->grille[i-2][j].type == t);
            int bas = (niveau->grille[i+1][j].type == t && niveau->grille[i+2][j].type == t);
            int gauche = (niveau->grille[i][j-1].type == t && niveau->grille[i][j-2].type == t);
            int droite = (niveau->grille[i][j+1].type == t && niveau->grille[i][j+2].type == t);
            
            // Croix complète = haut + bas + gauche + droite
            if (haut && bas && gauche && droite) {
                // Supprime toute la ligne et la colonne
                effetLigne(niveau, i);
                effetColonne(niveau, j);
                modif = 1;
            }
        }
    }
    return modif;
}

// Détecte un carré 4x4 d'items identiques (CDC page 5)
int detecterCarre4x4(Niveau *niveau) {
    int modif = 0;
    
    for (int i = 0; i <= LIGNES - 4; i++) {
        for (int j = 0; j <= COLONNES - 4; j++) {
            int t = niveau->grille[i][j].type;
            if (t == VIDE) continue;
            
            // Vérifie si toutes les 16 cases du carré sont identiques
            int estCarre = 1;
            for (int di = 0; di < 4 && estCarre; di++) {
                for (int dj = 0; dj < 4 && estCarre; dj++) {
                    if (niveau->grille[i + di][j + dj].type != t) {
                        estCarre = 0;
                    }
                }
            }
            
            if (estCarre) {
                effetCarre4x4(niveau, i, j);
                modif = 1;
            }
        }
    }
    return modif;
}

int detecterEtSupprimerAlignements(Niveau *niveau) {
    int modif = 0;
    // On utilise une matrice temporaire pour marquer ce qu'il faut détruire
    // (Sinon, si on détruit tout de suite, on casse les croisements en T ou L)
    int aDetruire[LIGNES][COLONNES] = {0}; 

    // --- 0. Détection figures spéciales AVANT les alignements simples ---
    
    // Carré 4x4 (CDC page 5)
    if (detecterCarre4x4(niveau)) modif = 1;
    
    // Croix de 9 (CDC page 5)
    if (detecterCroix(niveau)) modif = 1;

    // --- 1. Vérification Horizontale ---
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES - 2; j++) { // -2 car on regarde j, j+1, j+2
            int t = niveau->grille[i][j].type;
            if (t == VIDE) continue;
            
            // On compte combien de cases identiques à la suite
            int longueur = 1;
            while (j + longueur < COLONNES && niveau->grille[i][j + longueur].type == t) {
                longueur++;
            }
            
            // Si 3 ou plus → on marque à détruire
            if (longueur >= 3) {
                for (int k = 0; k < longueur; k++) {
                    aDetruire[i][j + k] = 1;
                }
                
                // 6 alignés → supprime toute la couleur (CDC page 5)
                if (longueur >= 6) {
                    effetCouleur(niveau, t);
                }
                // 5 alignés + niveau 2+ → BOMBE (EXTENSION)
                else if (longueur == 5 && niveau->numeroNiveau >= 2) {
                    effetBombe(niveau, i, j + 2); // Centre de l'alignement
                }
                // 3 ou 4 alignés → supprime juste la suite (normal)
                
                modif = 1;
                j += longueur - 1; // On saute les cases déjà traitées
            }
        }
    }

    // --- 2. Vérification Verticale ---
    for (int j = 0; j < COLONNES; j++) {
        for (int i = 0; i < LIGNES - 2; i++) {
            int t = niveau->grille[i][j].type;
            if (t == VIDE) continue;
            
            // On compte combien de cases identiques à la suite
            int longueur = 1;
            while (i + longueur < LIGNES && niveau->grille[i + longueur][j].type == t) {
                longueur++;
            }
            
            // Si 3 ou plus → on marque à détruire
            if (longueur >= 3) {
                for (int k = 0; k < longueur; k++) {
                    aDetruire[i + k][j] = 1;
                }
                
                // 6 alignés → supprime toute la couleur (CDC page 5)
                if (longueur >= 6) {
                    effetCouleur(niveau, t);
                }
                // 5 alignés + niveau 2+ → BOMBE (EXTENSION)
                else if (longueur == 5 && niveau->numeroNiveau >= 2) {
                    effetBombe(niveau, i + 2, j); // Centre de l'alignement
                }
                // 3 ou 4 alignés → supprime juste la suite (normal)
                
                modif = 1;
                i += longueur - 1; // On saute les cases déjà traitées
            }
        }
    }

    // --- 3. Suppression réelle ---
    if (modif) {
        for (int i = 0; i < LIGNES; i++) {
            for (int j = 0; j < COLONNES; j++) {
                if (aDetruire[i][j]) {
                    supprimerCase(niveau, i, j);
                }
            }
        }
    }

    return modif; // Renvoie 1 si on a cassé des trucs
}

void faireTomberEtRemplir(Niveau *niveau) {
    // Pour chaque colonne...
    for (int j = 0; j < COLONNES; j++) {
        // GRAVITÉ : On remonte de bas en haut
        // On place un curseur d'écriture 'w' en bas
        int w = LIGNES - 1; 
        
        // On parcourt la colonne de bas en haut avec un curseur de lecture 'r'
        for (int r = LIGNES - 1; r >= 0; r--) {
            if (niveau->grille[r][j].type != VIDE) {
                // Si la case n'est pas vide, on la fait tomber à la position 'w'
                niveau->grille[w][j] = niveau->grille[r][j];
                w--; // On remonte le curseur d'écriture
            }
        }

        // REMPLISSAGE : Tout ce qui est au-dessus de 'w' est vide, on remplit !
        for (int k = w; k >= 0; k--) {
            niveau->grille[k][j].type = (rand() % NB_TYPES) + 1;
            niveau->grille[k][j].estSelectionne = 0;
        }
    }
}

void gererCombos(Niveau *niveau) {
    int combo = 0;
    // Boucle tant qu'il y a des alignements (Réaction en chaîne)
    do {
        // 1. On supprime les lignes de 3
        combo = detecterEtSupprimerAlignements(niveau);
        
        if (combo) {
            // 2. Si on a supprimé, on fait tomber et on remplit
            // Petite astuce visuelle : Si on veut voir l'animation,
            // il faudrait afficher et faire un Sleep ici, mais on fait simple.
            faireTomberEtRemplir(niveau);
        }
    } while (combo); 
}