#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  // Pour _getch() et _kbhit()
#include <windows.h> // Pour Sleep()
#include <math.h>    // Pour abs() (calcul de distance)
#include <string.h>  // Pour memset()

// On inclut tous nos fichiers
#include "structure.h"
#include "jeu.h"
#include "affichage.h"
#include "sauvegarde.h"

// ============== ECRANS VICTOIRE / DEFAITE ==============

// Affiche l'écran de victoire après un niveau réussi
void afficherVictoire(Niveau *niveau) {
    system("cls");
    
    color(VERT, NOIR);
    gotoligcol(8, 25);
    printf("=============================");
    gotoligcol(9, 25);
    printf("     NIVEAU %d TERMINE !     ", niveau->numeroNiveau);
    gotoligcol(10, 25);
    printf("=============================");
    
    color(BLANC, NOIR);
    gotoligcol(12, 25);
    printf("Score : %d", niveau->score);
    gotoligcol(13, 25);
    printf("Vies restantes : %d", niveau->vies);
    
    if (niveau->numeroNiveau < 3) {
        color(JAUNE, NOIR);
        gotoligcol(15, 25);
        printf("Prochain niveau dans 3 secondes...");
    } else {
        color(MAGENTA, NOIR);
        gotoligcol(15, 25);
        printf("FELICITATIONS ! JEU TERMINE !");
        gotoligcol(16, 25);
        printf("Score final : %d", niveau->score);
    }
    
    color(BLANC, NOIR);
    Sleep(3000);
}

// Affiche l'écran de défaite (plus de coups ou temps écoulé)
void afficherDefaite(Niveau *niveau, char* raison) {
    system("cls");
    
    color(ROUGE, NOIR);
    gotoligcol(8, 25);
    printf("=============================");
    gotoligcol(9, 25);
    printf("         GAME OVER !         ");
    gotoligcol(10, 25);
    printf("=============================");
    
    color(BLANC, NOIR);
    gotoligcol(12, 25);
    printf("Raison : %s", raison);
    gotoligcol(13, 25);
    printf("Score : %d", niveau->score);
    gotoligcol(14, 25);
    printf("Niveau : %d", niveau->numeroNiveau);
    
    if (niveau->vies > 1) {
        color(JAUNE, NOIR);
        gotoligcol(16, 25);
        printf("Il vous reste %d vies !", niveau->vies - 1);
        gotoligcol(17, 25);
        printf("Appuyez sur une touche pour reessayer...");
    } else {
        color(ROUGE, NOIR);
        gotoligcol(16, 25);
        printf("Plus de vies ! Retour au menu...");
    }
    
    color(BLANC, NOIR);
    Sleep(2000);
    if (niveau->vies > 1) _getch();
}

// ============== FIN ECRANS ==============

// --- FONCTION CŒUR DU JEU : La boucle de la partie ---
// Retourne : 1 (victoire), 0 (défaite), -1 (quitter)
int jouerNiveau(Niveau *monNiveau) {
    int curseurX = 0;
    int curseurY = 0;
    int jeuEnCours = 1;
    int codeTouche;
    int resultat = 0; // 0 = défaite par défaut

    // On affiche le cadre statique une seule fois
    afficherCadre();

    // Sauvegarde le score et les vies (pour les garder entre niveaux)
    int scoreAvant = monNiveau->score;
    int viesAvant = monNiveau->vies;

    // Au lancement, on nettoie les combos existants
    gererCombos(monNiveau);
    
    // Reset des objectifs après gererCombos (les combos auto comptent pas)
    for (int k = 0; k <= NB_TYPES; k++) {
        monNiveau->contrat.quantiteActuelle[k] = 0;
    }
    monNiveau->score = scoreAvant;
    monNiveau->vies = viesAvant;

    // BOUCLE INFINIE
    while(jeuEnCours) {
        
        // 1. AFFICHAGE
        afficherGrille(monNiveau, curseurX, curseurY);
        afficherHUD(monNiveau);

        // 2. VERIFICATION VICTOIRE
        if (verifierContrat(monNiveau)) {
            afficherVictoire(monNiveau);
            resultat = 1; // Victoire
            jeuEnCours = 0;
            break;
        }

        // 3. VERIFICATION DEFAITE (Plus de coups)
        if (monNiveau->coupsRestants <= 0) {
            afficherDefaite(monNiveau, "Plus de coups !");
            resultat = 0;
            jeuEnCours = 0;
            break;
        }

        // 4. GESTION DU TEMPS
        if (difftime(time(NULL), monNiveau->startTime) > monNiveau->tempsTotalSec) {
            afficherDefaite(monNiveau, "Temps ecoule !");
            resultat = 0;
            jeuEnCours = 0;
            break;
        }

        // 5. GESTION CLAVIER
        if (_kbhit()) { 
            codeTouche = _getch();

            // --- DEPLACEMENT (FLECHES) ---
            if (codeTouche == 224 || codeTouche == 0) {
                codeTouche = _getch();
                switch (codeTouche) {
                    case 72: if(curseurY > 0) curseurY--; break;        // HAUT
                    case 80: if(curseurY < LIGNES-1) curseurY++; break; // BAS
                    case 75: if(curseurX > 0) curseurX--; break;        // GAUCHE
                    case 77: if(curseurX < COLONNES-1) curseurX++; break;// DROITE
                }
            }

            // --- ACTION (ESPACE) : LE COEUR DU JEU ---
            else if (codeTouche == 32) { 
                
                // On cherche si une case est déjà sélectionnée ailleurs
                int xPrecedent = -1, yPrecedent = -1;
                int dejaSelectionne = 0;

                for(int i=0; i<LIGNES; i++) {
                    for(int j=0; j<COLONNES; j++) {
                        if (monNiveau->grille[i][j].estSelectionne) {
                            dejaSelectionne = 1;
                            yPrecedent = i;
                            xPrecedent = j;
                        }
                    }
                }

                if (dejaSelectionne) {
                    // Cas A : On clique sur la MÊME case -> On désélectionne
                    if (xPrecedent == curseurX && yPrecedent == curseurY) {
                        monNiveau->grille[curseurY][curseurX].estSelectionne = 0;
                    }
                    // Cas B : On clique sur une AUTRE case -> Tentative d'échange
                    else {
                        // On vérifie qu'elles sont voisines (distance de 1)
                        if (abs(curseurX - xPrecedent) + abs(curseurY - yPrecedent) == 1) {
                            
                            // 1. On tente l'échange
                            echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                          &monNiveau->grille[curseurY][curseurX]);
                            
                            // 2. On enlève la sélection tout de suite
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 0;

                            // 3. LE TEST CRUCIAL : Est-ce que ça aligne 3 fruits ?
                            if (detecterEtSupprimerAlignements(monNiveau)) {
                                // OUI ! C'est un bon coup
                                monNiveau->coupsRestants--; 
                                faireTomberEtRemplir(monNiveau); 
                                gererCombos(monNiveau); 
                            } 
                            else {
                                // NON ! Ça ne fait rien, on annule.
                                echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                              &monNiveau->grille[curseurY][curseurX]);
                            }

                        } else {
                            // Trop loin : On sélectionne la nouvelle case
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 1;
                        }
                    }
                } 
                else {
                    // Cas C : Rien n'était sélectionné -> On sélectionne la case actuelle
                    monNiveau->grille[curseurY][curseurX].estSelectionne = 1;
                }
            }

            // --- QUITTER (Touche X) ---
            else if (codeTouche == 'x' || codeTouche == 'X') {
                
                // On efface le bas pour poser la question
                gotoligcol(LIGNES+4, 2); 
                color(JAUNE, NOIR);
                printf("Voulez-vous sauvegarder avant de quitter ? (O/N) ");
                
                char reponse = _getch();
                
                if (reponse == 'o' || reponse == 'O') {
                    if(sauvegarderPartie(monNiveau)) {
                        gotoligcol(LIGNES+4, 2); 
                        color(VERT, NOIR);
                        printf(">> SAUVEGARDE REUSSIE ! A BIENTOT <<       ");
                        Sleep(1500); 
                    }
                }
                else {
                    // Supprime la sauvegarde si on quitte sans sauvegarder
                    remove("sauvegarde.txt");
                }
                
                resultat = -1;
                jeuEnCours = 0;
            }
        } // Fin du if(_kbhit)

        // Petite pause CPU (Important !)
        Sleep(10);

    } // Fin du while(jeuEnCours)
    
    return resultat;
}

// --- PROGRAMME PRINCIPAL (MAIN) ---
int main() {
    SetConsoleOutputCP(65001);
    Niveau monNiveau;
    int choixMenu = 0;

    // 1. Initialisation technique (Console, couleurs...)
    initConsole();

    // 2. Boucle du Menu Principal
    do {
        // On affiche le menu (fonction définie dans affichage.c)
        choixMenu = afficherMenuPrincipal();

        switch (choixMenu) {
            case 1: // NOUVELLE PARTIE
            {
                // Reset complet avant de commencer
                memset(&monNiveau, 0, sizeof(Niveau));
                
                system("cls");
                gotoligcol(10, 30);
                printf("Entrez votre pseudo : ");
                scanf("%s", monNiveau.pseudo);
                
                // ============== BOUCLE DES 3 NIVEAUX ==============
                int niveauActuel = 1;
                int continuer = 1;
                int scoreTotal = 0;
                int viesRestantes = 3;
                
                while (niveauActuel <= 3 && continuer) {
                    // Initialise le niveau
                    initialiserNiveau(&monNiveau, niveauActuel);
                    monNiveau.score = scoreTotal;
                    monNiveau.vies = viesRestantes;
                    
                    // Joue le niveau
                    int resultat = jouerNiveau(&monNiveau);
                    
                    if (resultat == 1) {
                        // Victoire → niveau suivant
                        scoreTotal = monNiveau.score;
                        viesRestantes = monNiveau.vies;
                        niveauActuel++;
                    } 
                    else if (resultat == 0) {
                        // Défaite → perd une vie
                        viesRestantes--;
                        
                        if (viesRestantes <= 0) {
                            // Plus de vies → fin de partie
                            continuer = 0;
                        }
                        // Sinon on rejoue le même niveau
                    }
                    else {
                        // Quitter (-1)
                        continuer = 0;
                    }
                }
                // ============== FIN BOUCLE NIVEAUX ==============
            }
            break;

            case 2: // CHARGER PARTIE
                system("cls");
                if (chargerPartie(&monNiveau)) {
                    gotoligcol(10, 30);
                    color(VERT, NOIR);
                    printf("Chargement reussi ! Bon retour %s.", monNiveau.pseudo);
                    Sleep(2000);
                    color(BLANC, NOIR);
                    
                    jouerNiveau(&monNiveau);
                } else {
                    gotoligcol(10, 30);
                    color(ROUGE, NOIR);
                    printf("Erreur : Aucune sauvegarde trouvee.");
                    Sleep(2000);
                    color(BLANC, NOIR);
                }
                break;

            case 3: // QUITTER
                system("cls");
                gotoligcol(10, 30);
                printf("Merci d'avoir joue ! A bientot.");
                Sleep(1000);
                break;
            case 4: // REGLES DU JEU (NOUVEAU)
                system("cls");
                color(CYAN, NOIR);
                gotoligcol(2, 30);
                printf("=== REGLES DU JEU ===");
                
                color(BLANC, NOIR);
                gotoligcol(4, 5);
                printf("BUT : Aligner 3 fruits ou plus pour les detruire et remplir le contrat.");
                
                gotoligcol(6, 5);
                printf("COMMANDES :");
                gotoligcol(7, 10);
                printf("- Fleches : Deplacer le curseur");
                gotoligcol(8, 10);
                printf("- Espace : Selectionner / Echanger");
                gotoligcol(9, 10);
                printf("- X : Quitter / Sauvegarder");
                
                gotoligcol(11, 5);
                printf("FIGURES SPECIALES :");
                gotoligcol(12, 10);
                printf("- 6 alignes : Supprime tous les fruits de cette couleur");
                gotoligcol(13, 10);
                printf("- Croix de 9 : Supprime la ligne et la colonne");
                gotoligcol(14, 10);
                printf("- Carre 4x4 : Supprime le carre entier");
                gotoligcol(15, 10);
                printf("- 5 alignes (niveau 2+) : Bombe 3x3");
                
                gotoligcol(17, 5);
                printf("OBJECTIF : Remplir le contrat avant la fin du temps ou des coups !");
                
                color(JAUNE, NOIR);
                gotoligcol(20, 25);
                printf("Appuyez sur une touche pour revenir...");
                
                color(BLANC, NOIR);
                _getch();
                break;
        }

    } while (choixMenu != 3);

    return 0;
}