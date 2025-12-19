#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  // Pour _getch() et _kbhit()
#include <windows.h> // Pour Sleep()
#include <math.h>    // Pour abs() (calcul de distance)

// On inclut tous nos fichiers
#include "structure.h"
#include "jeu.h"
#include "affichage.h"
#include "sauvegarde.h"

// --- FONCTION CŒUR DU JEU : La boucle de la partie ---
void jouerNiveau(Niveau *monNiveau) {
    int curseurX = 0;
    int curseurY = 0;
    int jeuEnCours = 1;
    int codeTouche;

    // On affiche le cadre statique une seule fois pour éviter le scintillement
    afficherCadre();

    // BOUCLE INFINIE (Tant qu'on ne quitte pas ou qu'on n'a pas perdu)
    while(jeuEnCours) {
        
        // 1. AFFICHAGE
        // On met à jour la grille et le HUD (Score, Temps...)
        afficherGrille(monNiveau, curseurX, curseurY);
        afficherHUD(monNiveau);

        // 2. VERIFICATION DEFAITE
        if (monNiveau->coupsRestants <= 0) {
            gotoligcol(LIGNES/2, COLONNES + 5);
            color(ROUGE, NOIR);
            printf("GAME OVER ! Plus de coups...");
            Sleep(3000); // On laisse le temps de lire
            jeuEnCours = 0;
            break;
        }

        // 3. GESTION DU TEMPS (Si le temps est écoulé)
        // (La fonction tempsRestantSec est dans affichage.c mais on peut vérifier ici)
        if (difftime(time(NULL), monNiveau->startTime) > monNiveau->tempsTotalSec) {
             gotoligcol(LIGNES/2, COLONNES + 5);
             color(ROUGE, NOIR);
             printf("TEMPS ECOULE !");
             Sleep(3000);
             jeuEnCours = 0;
             break;
        }

        // 4. GESTION CLAVIER (Si une touche est appuyée)
        if (_kbhit()) { 
            codeTouche = _getch(); // On lit la touche

            // --- DEPLACEMENT (FLECHES) ---
            if (codeTouche == 224 || codeTouche == 0) {
                codeTouche = _getch(); // On lit le code de la flèche
                switch (codeTouche) {
                    case 72: // HAUT
                        if(curseurY > 0) curseurY--; 
                        break;
                    case 80: // BAS
                        if(curseurY < LIGNES-1) curseurY++; 
                        break;
                    case 75: // GAUCHE
                        if(curseurX > 0) curseurX--; 
                        break;
                    case 77: // DROITE
                        if(curseurX < COLONNES-1) curseurX++; 
                        break;
                }
            }

            // --- ACTION (ESPACE) : SÉLECTION / ÉCHANGE ---
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
                            
                            // 1. On échange !
                            echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                          &monNiveau->grille[curseurY][curseurX]);
                            
                            // 2. On retire la sélection et on compte le coup
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->coupsRestants--;

                            // (Ici, tu pourras ajouter la fonction qui vérifie les alignements Match-3 plus tard)

                        } else {
                            // Trop loin : On annule l'ancienne et on sélectionne la nouvelle
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

            // --- SAUVEGARDE (Touche S) ---
            else if (codeTouche == 's' || codeTouche == 'S') {
                if(sauvegarderPartie(monNiveau)) {
                    // Message temporaire
                    gotoligcol(LIGNES+4, 2); 
                    color(VERT, NOIR);
                    printf(">> PARTIE SAUVEGARDEE ! <<");
                    Sleep(1000); 
                    // On efface le message
                    gotoligcol(LIGNES+4, 2); 
                    printf("                          ");
                    color(BLANC, NOIR);
                }
            }

            // --- QUITTER (Touche X) ---
            else if (codeTouche == 'x' || codeTouche == 'X') {
                jeuEnCours = 0; // On sort de la boucle while
            }
        }

        // Petite pause pour ne pas faire surchauffer le processeur (important !)
        Sleep(10); 
    }
}

// --- PROGRAMME PRINCIPAL (MAIN) ---
int main() {
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
                // On prépare un nouveau niveau 1
                initialiserNiveau(&monNiveau, 1);
                
                // On demande le pseudo (Joli écran noir)
                system("cls");
                gotoligcol(10, 30);
                printf("Entrez votre pseudo : ");
                // scanf un peu brute, mais ça fait le taf pour l'école
                scanf("%s", monNiveau.pseudo); 
                
                // C'est parti !
                jouerNiveau(&monNiveau);
                break;

            case 2: // CHARGER PARTIE
                system("cls");
                if (chargerPartie(&monNiveau)) {
                    gotoligcol(10, 30);
                    color(VERT, NOIR);
                    printf("Chargement reussi ! Bon retour %s.", monNiveau.pseudo);
                    Sleep(2000);
                    color(BLANC, NOIR);
                    
                    // On lance le jeu avec les données chargées
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
        }

    } while (choixMenu != 3); // Tant qu'on ne choisit pas "Quitter"

    return 0;
}