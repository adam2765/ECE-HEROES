#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  // Pour _getch() et _kbhit()
#include <windows.h> // Pour Sleep()
#include <math.h>    // Pour abs() (calcul de distance)

// On inclut tous nos fichiers
#include "structure.h" // Attention, vérifie si c'est structure.h ou structures.h (avec un s) dans ton dossier
#include "jeu.h"
#include "affichage.h"
#include "sauvegarde.h"



// --- FONCTION CŒUR DU JEU : La boucle de la partie ---
void jouerNiveau(Niveau *monNiveau) {
    int curseurX = 0;
    int curseurY = 0;
    int jeuEnCours = 1;
    int codeTouche;

    // On affiche le cadre statique une seule fois
    afficherCadre();

    // Au lancement, on nettoie les combos existants
    gererCombos(monNiveau);

    // BOUCLE INFINIE
    while(jeuEnCours) {
        
        // 1. AFFICHAGE
        afficherGrille(monNiveau, curseurX, curseurY);
        afficherHUD(monNiveau);

        // 2. VERIFICATION DEFAITE
        if (monNiveau->coupsRestants <= 0) {
            gotoligcol(LIGNES/2, COLONNES + 5);
            color(ROUGE, NOIR);
            printf("GAME OVER ! Plus de coups...");
            Sleep(3000);
            jeuEnCours = 0;
            break;
        }

        // 3. GESTION DU TEMPS
        if (difftime(time(NULL), monNiveau->startTime) > monNiveau->tempsTotalSec) {
             gotoligcol(LIGNES/2, COLONNES + 5);
             color(ROUGE, NOIR);
             printf("TEMPS ECOULE !");
             Sleep(3000);
             jeuEnCours = 0;
             break;
        }

        // 4. GESTION CLAVIER
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

                            // 3. LE TEST CRUCIAL : Est-ce que ça aligne 3 bonbons ?
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
                
                jeuEnCours = 0; // On arrête la boucle
            }
        } // Fin du if(_kbhit)

        // Petite pause CPU (Important !)
        Sleep(10);

    } // Fin du while(jeuEnCours) <--- C'ETAIT ICI QU'IL MANQUAIT L'ACCOLADE
} // Fin de la fonction jouerNiveau <--- ET ICI

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
                initialiserNiveau(&monNiveau, 1);
                
                system("cls");
                gotoligcol(10, 30);
                printf("Entrez votre pseudo : ");
                scanf("%s", monNiveau.pseudo); 
                
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

    } while (choixMenu != 3);

    return 0;
}