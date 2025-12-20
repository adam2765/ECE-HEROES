#include <stdio.h>                 // Pour afficher si on en a besoin
#include <stdlib.h>                // Pour le hasard avec rand()
#include <time.h>                  // Pour récupérer l'heure
#include "jeu.h"                   // Nos structures et constantes


// Fonction pour démarrer la partie et remplir les données
void initialiserNiveau(Niveau *niveau, int numNiveau) {         // On initialise un niveau complet

    // 1. On active le hasard
    srand(time(NULL));                                          // Comme ça la grille change à chaque partie


    // 2. Initialisation des variables simples
    niveau->numeroNiveau = numNiveau;                           // On retient quel niveau on joue
    niveau->vies = 3;                                           // 3 vies au départ
    niveau->score = 0;                                          // Score à zéro au départ
    niveau->coupsRestants = 20;                                 // Coups de base (ça peut être modifié après)


    // --- IMPORTANT POUR L'AFFICHAGE ---
    niveau->tempsTotalSec = 120;                                // Temps total du niveau en secondes
    niveau->startTime = time(NULL);                             // On lance le chrono maintenant
    // ----------------------------------


    // On met le pseudo à vide
    niveau->pseudo[0] = '\0';                                   // Chaîne vide pour éviter une vieille valeur


    // 3. Initialisation du Contrat (Objectifs à 0 au début)
    for (int k = 0; k <= NB_TYPES; k++) {                       // On remet tous les compteurs à zéro
        niveau->contrat.quantiteCible[k] = 0;                   // Objectif = 0 pour ce type
        niveau->contrat.quantiteActuelle[k] = 0;                // Avancement = 0 pour ce type
    }


    // Contrats différents selon le niveau (difficulté croissante)
    if (numNiveau == 1) {                                       // Niveau 1
        niveau->contrat.quantiteCible[ITEM_1] = 10;             // Objectif simple
        niveau->coupsRestants = 30;                             // Plus de coups pour commencer
        niveau->tempsTotalSec = 120;                            // 2 minutes
    }
    else if (numNiveau == 2) {                                  // Niveau 2
        niveau->contrat.quantiteCible[ITEM_1] = 15;             // Objectif item 1
        niveau->contrat.quantiteCible[ITEM_2] = 15;             // Objectif item 2
        niveau->coupsRestants = 25;                             // Moins de coups
        niveau->tempsTotalSec = 90;                             // 1 min 30
    }
    else if (numNiveau == 3) {                                  // Niveau 3
        niveau->contrat.quantiteCible[ITEM_1] = 20;             // Objectif plus élevé
        niveau->contrat.quantiteCible[ITEM_2] = 20;             // Objectif plus élevé
        niveau->contrat.quantiteCible[ITEM_3] = 10;             // Objectif en plus
        niveau->coupsRestants = 20;                             // Peu de coups
        niveau->tempsTotalSec = 60;                             // 1 minute
    }


    // 4. Remplissage de la grille avec des items aléatoires
    for(int i = 0; i < LIGNES; i++) {                           // Parcours des lignes
        for(int j = 0; j < COLONNES; j++) {                     // Parcours des colonnes

            // On tire un nombre entre 1 et 5
            int typeAleatoire = (rand() % NB_TYPES) + 1;        // Type aléatoire entre 1 et NB_TYPES

            niveau->grille[i][j].type = typeAleatoire;          // On met l'item dans la case
            niveau->grille[i][j].estSelectionne = 0;            // Rien n'est sélectionné au départ
        }
    }
}


// Fonction pour échanger deux cases
void echangerCases(Case *case1, Case *case2) {                  // Échange 2 cases quand le joueur swap

    Case temp;                                                  // Variable temporaire
    temp = *case1;                                              // On garde la première case
    *case1 = *case2;                                            // On met la deuxième dans la première
    *case2 = temp;                                              // On remet l'ancienne première dans la deuxième
}


// ============== EFFETS SPÉCIAUX (Figures du CDC) ==============


// EFFET BOMBE : Détruit zone 3x3 autour de (x,y)
void effetBombe(Niveau *niveau, int x, int y) {                 // Bombe centrée sur (x,y)

    // On parcourt les cases autour
    for (int i = x - 1; i <= x + 1; i++) {                      // 3 lignes
        for (int j = y - 1; j <= y + 1; j++) {                  // 3 colonnes

            // On vérifie qu'on reste dans la grille
            if (i >= 0 && i < LIGNES && j >= 0 && j < COLONNES) {
                niveau->grille[i][j].type = VIDE;               // On supprime la case
                niveau->score += 10;                            // On gagne des points
            }
        }
    }
}


// EFFET LIGNE : Supprimer une ligne entière
void effetLigne(Niveau *niveau, int ligne) {                    // Supprime tout sur la ligne

    // Sécurité
    if (ligne < 0 || ligne >= LIGNES) return;                   // Si la ligne est invalide on sort

    for (int j = 0; j < COLONNES; j++) {                        // Parcours de la ligne
        if (niveau->grille[ligne][j].type != VIDE) {            // Si ce n'est pas déjà vide
            niveau->grille[ligne][j].type = VIDE;               // On supprime
            niveau->score += 10;                                // Score
        }
    }
}


// EFFET COLONNE : Supprimer une colonne entière
void effetColonne(Niveau *niveau, int colonne) {                // Supprime tout sur la colonne

    // Sécurité
    if (colonne < 0 || colonne >= COLONNES) return;             // Si la colonne est invalide on sort

    for (int i = 0; i < LIGNES; i++) {                          // Parcours de la colonne
        if (niveau->grille[i][colonne].type != VIDE) {          // Si ce n'est pas vide
            niveau->grille[i][colonne].type = VIDE;             // On supprime
            niveau->score += 10;                                // Score
        }
    }
}


// EFFET COULEUR : Supprimer toutes les cases d'un type
void effetCouleur(Niveau *niveau, int type) {                   // Supprime tous les items d'un type

    for (int i = 0; i < LIGNES; i++) {                          // Parcours lignes
        for (int j = 0; j < COLONNES; j++) {                    // Parcours colonnes

            // On compare avec .type
            if (niveau->grille[i][j].type == type) {            // Si c'est le type demandé
                niveau->grille[i][j].type = VIDE;               // On supprime
                niveau->score += 10;                            // Score
            }
        }
    }
}


// EFFET CARRE : Supprimer un carré 4x4
void effetCarre4x4(Niveau *niveau, int x, int y) {              // Supprime un bloc 4x4

    for (int i = x; i < x + 4 && i < LIGNES; i++) {             // Sur 4 lignes max
        for (int j = y; j < y + 4 && j < COLONNES; j++) {       // Sur 4 colonnes max

            if (niveau->grille[i][j].type != VIDE) {            // Si pas vide
                niveau->grille[i][j].type = VIDE;               // On supprime
                niveau->score += 10;                            // Score
            }
        }
    }
}


/* =========================================================
   MOTEUR MATCH-3 (DESTRUCTION & GRAVITÉ)
   ========================================================= */


// Fonction interne pour supprimer une case proprement
void supprimerCase(Niveau *niveau, int i, int j) {              // Supprime une case et met à jour

    int type = niveau->grille[i][j].type;                       // On récupère le type
    
    // Si c'est déjà vide, on ne fait rien
    if (type == VIDE) return;                                   // Rien à faire

    // 1. On met à jour le contrat (Objectifs)
    if (type <= NB_TYPES) {                                     // Si c'est un item normal
        niveau->contrat.quantiteActuelle[type]++;               // On compte dans le contrat
    }

    // 2. On augmente le score
    niveau->score += 10;                                        // Score +10

    // 3. On vide la case
    niveau->grille[i][j].type = VIDE;                           // La case devient vide
    niveau->grille[i][j].estSelectionne = 0;                    // On enlève la sélection
}


// Détecte une croix de 9 items identiques
int detecterCroix(Niveau *niveau) {                             // Cherche la forme croix

    int modif = 0;                                              // 0 = rien trouvé
    
    for (int i = 2; i < LIGNES - 2; i++) {                      // On évite les bords
        for (int j = 2; j < COLONNES - 2; j++) {                // On évite les bords

            int t = niveau->grille[i][j].type;                  // Type du centre
            if (t == VIDE) continue;                            // Si vide on passe
            
            int haut = (niveau->grille[i-1][j].type == t && niveau->grille[i-2][j].type == t);     // 2 mêmes en haut
            int bas = (niveau->grille[i+1][j].type == t && niveau->grille[i+2][j].type == t);      // 2 mêmes en bas
            int gauche = (niveau->grille[i][j-1].type == t && niveau->grille[i][j-2].type == t);   // 2 mêmes à gauche
            int droite = (niveau->grille[i][j+1].type == t && niveau->grille[i][j+2].type == t);   // 2 mêmes à droite
            
            if (haut && bas && gauche && droite) {              // Si on a la croix complète
                effetLigne(niveau, i);                          // On supprime la ligne
                effetColonne(niveau, j);                        // On supprime la colonne
                modif = 1;                                      // On note qu'on a modifié la grille
            }
        }
    }
    return modif;                                               // 1 si trouvé, sinon 0
}


// Détecte un carré 4x4 d'items identiques
int detecterCarre4x4(Niveau *niveau) {                          // Cherche la forme carré 4x4

    int modif = 0;                                              // 0 = rien trouvé
    
    for (int i = 0; i <= LIGNES - 4; i++) {                     // Départ possible du carré
        for (int j = 0; j <= COLONNES - 4; j++) {               // Départ possible du carré

            int t = niveau->grille[i][j].type;                  // Type de référence
            if (t == VIDE) continue;                            // Si vide on ignore
            
            int estCarre = 1;                                   // On suppose que c'est un carré

            for (int di = 0; di < 4 && estCarre; di++) {         // On vérifie 4 lignes
                for (int dj = 0; dj < 4 && estCarre; dj++) {     // On vérifie 4 colonnes
                    if (niveau->grille[i + di][j + dj].type != t) { // Si une case est différente
                        estCarre = 0;                           // Alors ce n'est pas un carré
                    }
                }
            }
            
            if (estCarre) {                                     // Si carré valide
                effetCarre4x4(niveau, i, j);                    // On applique l'effet carré
                modif = 1;                                      // On note une modification
            }
        }
    }
    return modif;                                               // 1 si trouvé, sinon 0
}


int detecterEtSupprimerAlignements(Niveau *niveau) {            // Détecte alignements et les supprime

    int modif = 0;                                              // Sert à savoir si on a supprimé

    int aDetruire[LIGNES][COLONNES] = {0};                      // Tableau qui garde les cases à détruire


    if (detecterCarre4x4(niveau)) modif = 1;                    // On check d'abord les carrés
    if (detecterCroix(niveau)) modif = 1;                       // Puis les croix


    for (int i = 0; i < LIGNES; i++) {                          // Parcours lignes
        for (int j = 0; j < COLONNES - 2; j++) {                // On garde de la place pour 3

            int t = niveau->grille[i][j].type;                  // Type de départ
            if (t == VIDE) continue;                            // Si vide, on ignore
            
            int longueur = 1;                                   // On compte combien à la suite

            while (j + longueur < COLONNES && niveau->grille[i][j + longueur].type == t) {
                longueur++;                                     // On agrandit la suite
            }

            if (longueur >= 3) {                                // Si 3 ou plus

                for (int k = 0; k < longueur; k++) {            // On marque les cases
                    aDetruire[i][j + k] = 1;                    // Case à supprimer
                }

                if (longueur >= 6) {                            // Si 6 alignés
                    effetCouleur(niveau, t);                    // Effet couleur
                }
                else if (longueur == 5 && niveau->numeroNiveau >= 2) {
                    effetBombe(niveau, i, j + 2);               // Bombe au centre
                }

                modif = 1;                                      // Il y aura suppression
                j += longueur - 1;                              // On saute la suite
            }
        }
    }


    for (int j = 0; j < COLONNES; j++) {                        // Parcours colonnes
        for (int i = 0; i < LIGNES - 2; i++) {                  // On garde de la place pour 3

            int t = niveau->grille[i][j].type;                  // Type de départ
            if (t == VIDE) continue;                            // Si vide, on ignore
            
            int longueur = 1;                                   // Longueur de suite

            while (i + longueur < LIGNES && niveau->grille[i + longueur][j].type == t) {
                longueur++;                                     // On agrandit
            }

            if (longueur >= 3) {                                // Alignement valide

                for (int k = 0; k < longueur; k++) {            // On marque
                    aDetruire[i + k][j] = 1;                    // Case à supprimer
                }

                if (longueur >= 6) {                            // 6 alignés
                    effetCouleur(niveau, t);                    // Effet couleur
                }
                else if (longueur == 5 && niveau->numeroNiveau >= 2) {
                    effetBombe(niveau, i + 2, j);               // Bombe au centre
                }

                modif = 1;                                      // Il y a eu suppression
                i += longueur - 1;                              // On saute la suite
            }
        }
    }


    if (modif) {                                                // Si on a marqué des cases
        for (int i = 0; i < LIGNES; i++) {                      // Parcours complet
            for (int j = 0; j < COLONNES; j++) {
                if (aDetruire[i][j]) {                          // Si marqué
                    supprimerCase(niveau, i, j);                // On supprime vraiment
                }
            }
        }
    }

    return modif;                                               // 1 si on a supprimé quelque chose
}


void faireTomberEtRemplir(Niveau *niveau) {                     // Fait tomber les items + remplit en haut

    for (int j = 0; j < COLONNES; j++) {                        // Colonne par colonne

        int w = LIGNES - 1;                                     // Position où on va "écrire" en bas
        
        for (int r = LIGNES - 1; r >= 0; r--) {                 // On lit de bas en haut
            if (niveau->grille[r][j].type != VIDE) {            // Si on trouve un item
                niveau->grille[w][j] = niveau->grille[r][j];    // On le descend
                w--;                                            // On remonte la place suivante
            }
        }

        for (int k = w; k >= 0; k--) {                          // Les cases au-dessus restent vides
            niveau->grille[k][j].type = (rand() % NB_TYPES) + 1;// On remet un item random
            niveau->grille[k][j].estSelectionne = 0;            // Pas sélectionné
        }
    }
}


void gererCombos(Niveau *niveau) {                              // Réaction en chaînes

    int combo = 0;                                              // 0 = plus rien, 1 = encore des suppressions

    do {
        combo = detecterEtSupprimerAlignements(niveau);         // On supprime les alignements
        
        if (combo) {                                            // Si on a supprimé
            faireTomberEtRemplir(niveau);                       // On fait tomber puis on remplis
        }
    } while (combo);                                            // Tant qu'il y a encore des combos
}


// --- VERIFICATION VICTOIRE ---


// Vérifie si tous les objectifs du contrat sont atteints
int verifierContrat(Niveau *niveau) {                           // Vérifie si le niveau est gagné

    for (int k = 1; k <= NB_TYPES; k++) {                       // On regarde chaque objectif
        if (niveau->contrat.quantiteActuelle[k] < niveau->contrat.quantiteCible[k]) {
            return 0;                                           // Si un seul n'est pas atteint, pas gagné
        }
    }
    return 1;                                                   // Sinon, c'est bon -> victoire !
}
