#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  
#include <windows.h> // pour la pause
#include <math.h>    // pour les calculs
#include <string.h> 
#include <time.h>    // pour le temps

// inclusion de nos fichiers a nous
#include "structure.h"
#include "jeu.h"
#include "affichage.h"
#include "sauvegarde.h"
#include "affichage_console.h" // fichier du prof

// ============== ECRANS FIN DE NIVEAU ==============

// fonction pour afficher quand on gagne
void afficherVictoire(Niveau *niveau) {
    clrscr(); // on efface tout
    
    set_color(GREEN, BLACK); // texte vert
    gotoxy(25, 8); // on se place au milieu
    printf("=============================");
    gotoxy(25, 9);
    printf("     NIVEAU %d TERMINE !     ", niveau->numeroNiveau);
    gotoxy(25, 10);
    printf("=============================");
    
    set_color(WHITE, BLACK); // texte blanc
    gotoxy(25, 12);
    printf("Score : %d", niveau->score); // affiche le score
    gotoxy(25, 13);
    printf("Vies restantes : %d", niveau->vies); // affiche les vies
    
    // si c'est pas le dernier niveau
    if (niveau->numeroNiveau < 3) {
        set_color(YELLOW, BLACK); // texte jaune
        gotoxy(25, 15);
        printf("Prochain niveau dans 3 secondes...");
    } else {
        // si c'est la fin du jeu
        set_color(MAGENTA, BLACK);
        gotoxy(25, 15);
        printf("FELICITATIONS ! JEU TERMINE !");
        gotoxy(25, 16);
        printf("Score final : %d", niveau->score);
    }
    
    set_color(WHITE, BLACK);
    Sleep(3000); // on attend 3 secondes
}

// fonction pour afficher quand on perd
void afficherDefaite(Niveau *niveau, char* raison) {
    clrscr(); // efface ecran
    
    set_color(RED, BLACK); // texte rouge
    gotoxy(25, 8);
    printf("=============================");
    gotoxy(25, 9);
    printf("         GAME OVER !         "); // perdu
    gotoxy(25, 10);
    printf("=============================");
    
    set_color(WHITE, BLACK);
    gotoxy(25, 12);
    printf("Raison : %s", raison); // pourquoi on a perdu
    gotoxy(25, 13);
    printf("Score : %d", niveau->score);
    gotoxy(25, 14);
    printf("Niveau : %d", niveau->numeroNiveau);
    
    // s'il reste des vies
    if (niveau->vies > 1) {
        set_color(YELLOW, BLACK);
        gotoxy(25, 16);
        printf("Il vous reste %d vies !", niveau->vies - 1);
        gotoxy(25, 17);
        printf("Appuyez sur une touche pour reessayer...");
    } else {
        // plus de vies, retour menu
        set_color(RED, BLACK);
        gotoxy(25, 16);
        printf("Plus de vies ! Retour au menu...");
    }
    
    set_color(WHITE, BLACK);
    Sleep(2000); // pause
    if (niveau->vies > 1) _getch(); // attend une touche si on continue
}

// ============== JEU PRINCIPAL ==============

// c'est la fonction qui fait tourner le niveau
int jouerNiveau(Niveau *monNiveau) {
    int curseurX = 0; // position horizontale curseur
    int curseurY = 0; // position verticale curseur
    int jeuEnCours = 1; // pour la boucle
    int codeTouche; // touche appuyee
    int resultat = 0; // resultat du niveau

    // affiche le cadre autour (une seule fois)
    afficherCadre();

    // on garde le score et vies d'avant au cas ou on perd
    int scoreAvant = monNiveau->score;
    int viesAvant = monNiveau->vies;

    // nettoyage du plateau au debut
    gererCombos(monNiveau);
    // remise a zero des objectifs
    for (int k = 0; k <= NB_TYPES; k++) {
        monNiveau->contrat.quantiteActuelle[k] = 0;
    }
    // on remet les stats propres
    monNiveau->score = scoreAvant;
    monNiveau->vies = viesAvant;

    // BOUCLE INFINIE DU JEU
    while(jeuEnCours) {
        
        // 1. AFFICHAGE
        afficherGrille(monNiveau, curseurX, curseurY); // dessine les fruits
        afficherHUD(monNiveau); // dessine le score a droite

        // 2. VERIF VICTOIRE
        if (verifierContrat(monNiveau)) { // si objectifs remplis
            afficherVictoire(monNiveau);
            resultat = 1; // gagne
            jeuEnCours = 0; // on sort de la boucle
            break;
        }

        // 3. VERIF DEFAITE (COUPS)
        if (monNiveau->coupsRestants <= 0) { // plus de coups
            afficherDefaite(monNiveau, "Plus de coups !");
            resultat = 0; // perdu
            jeuEnCours = 0;
            break;
        }

        // 4. VERIF TEMPS
        // calcul si le temps est depasse
        if (difftime(time(NULL), monNiveau->startTime) > monNiveau->tempsTotalSec) {
            afficherDefaite(monNiveau, "Temps ecoule !");
            resultat = 0; // perdu
            jeuEnCours = 0;
            break;
        }

        // 5. GESTION DES TOUCHES
        if (_kbhit()) { // si une touche est appuyee
            codeTouche = _getch(); // on recupere la touche

            // --- FLECHES DIRECTIONNELLES ---
            if (codeTouche == 224 || codeTouche == 0) {
                codeTouche = _getch(); // code special fleches
                switch (codeTouche) {
                    case 72: if(curseurY > 0) curseurY--; break;        // haut
                    case 80: if(curseurY < LIGNES-1) curseurY++; break; // bas
                    case 75: if(curseurX > 0) curseurX--; break;        // gauche
                    case 77: if(curseurX < COLONNES-1) curseurX++; break;// droite
                }
            }
            // --- TOUCHE ESPACE (ACTION) ---
            else if (codeTouche == 32) { 
                
                // variables pour memoriser la selection
                int xPrecedent = -1, yPrecedent = -1;
                int dejaSelectionne = 0; // booleen

                // on parcourt pour voir si une case est deja en rouge
                for(int i=0; i<LIGNES; i++) {
                    for(int j=0; j<COLONNES; j++) {
                        if (monNiveau->grille[i][j].estSelectionne) {
                            dejaSelectionne = 1; // trouve !
                            yPrecedent = i;
                            xPrecedent = j;
                        }
                    }
                }

                if (dejaSelectionne) {
                    // si on clique sur la meme case
                    if (xPrecedent == curseurX && yPrecedent == curseurY) {
                        monNiveau->grille[curseurY][curseurX].estSelectionne = 0; // on deselectionne
                    }
                    else {
                        // si on clique sur une autre case
                        // on verifie si elles sont a cote (distance = 1)
                        if (abs(curseurX - xPrecedent) + abs(curseurY - yPrecedent) == 1) {
                            
                            // on echange les deux cases
                            echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                          &monNiveau->grille[curseurY][curseurX]);
                            
                            // on enleve la selection rouge
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 0;

                            // TEST IMPORTANT : est-ce que ca fait des points ?
                            if (detecterEtSupprimerAlignements(monNiveau)) {
                                monNiveau->coupsRestants--; // on enleve un coup
                                faireTomberEtRemplir(monNiveau); // gravite
                                gererCombos(monNiveau); // verif combos chainés
                            } 
                            else {
                                // si ca fait rien, on annule l'echange (retour arriere)
                                echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                              &monNiveau->grille[curseurY][curseurX]);
                            }

                        } else {
                            // si trop loin, on change juste la selection
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 1;
                        }
                    }
                } 
                else {
                    // si rien selectionné, on selectionne la case actuelle
                    monNiveau->grille[curseurY][curseurX].estSelectionne = 1;
                }
            }

            // --- TOUCHE X (QUITTER) ---
            else if (codeTouche == 'x' || codeTouche == 'X') {
                
                // pose la question en bas
                gotoxy(2, LIGNES+4); 
                set_color(YELLOW, BLACK);
                printf("Voulez-vous sauvegarder avant de quitter ? (O/N) ");
                
                char reponse = _getch(); // attend reponse
                
                if (reponse == 'o' || reponse == 'O') {
                    // sauvegarde
                    if(sauvegarderPartie(monNiveau)) {
                        gotoxy(2, LIGNES+4); 
                        set_color(GREEN, BLACK);
                        printf(">> SAUVEGARDE REUSSIE ! A BIENTOT <<       ");
                        Sleep(1500); 
                    }
                }
                else {
                    // efface la sauvegarde si on veut pas
                    remove("sauvegarde.txt");
                }
                
                resultat = -1; // code pour dire quitter
                jeuEnCours = 0;
            }
        } 
        Sleep(10); // petite pause pour le processeur
    } 
    return resultat;
}

// --- MAIN (DEBUT DU PROGRAMME) ---
int main() {
    SetConsoleOutputCP(65001); // active les emojis
    hide_cursor();             // cache le curseur (fonction prof)
    
    Niveau monNiveau;
    int choixMenu = 0;

    // boucle du menu
    do {
        choixMenu = afficherMenuPrincipal(); // affiche menu

        switch (choixMenu) {
            case 1: // NOUVELLE PARTIE
            {
                // vide la memoire du niveau
                memset(&monNiveau, 0, sizeof(Niveau));
                
                clrscr();
                gotoxy(30, 10);
                printf("Entrez votre pseudo : ");
                scanf("%s", monNiveau.pseudo); // demande nom
                
                // variables de la partie
                int niveauActuel = 1;
                int continuer = 1;
                int scoreTotal = 0;
                int viesRestantes = 3;
                
                // boucle des niveaux (1 a 3)
                while (niveauActuel <= 3 && continuer) {
                    // prepare le niveau
                    initialiserNiveau(&monNiveau, niveauActuel);
                    monNiveau.score = scoreTotal;
                    monNiveau.vies = viesRestantes;
                    
                    // lance le jeu
                    int resultat = jouerNiveau(&monNiveau);
                    
                    if (resultat == 1) { // gagne
                        scoreTotal = monNiveau.score;
                        viesRestantes = monNiveau.vies;
                        niveauActuel++; // passe au suivant
                    } 
                    else if (resultat == 0) { // perdu
                        viesRestantes--; // perd une vie
                        if (viesRestantes <= 0) continuer = 0; // game over
                    }
                    else { // quitter
                        continuer = 0;
                    }
                }
            }
            break;

            case 2: // CHARGER PARTIE
                clrscr();
                // essaie de charger le fichier
                if (chargerPartie(&monNiveau)) {
                    gotoxy(30, 10);
                    set_color(GREEN, BLACK);
                    printf("Chargement reussi ! Bon retour %s.", monNiveau.pseudo);
                    Sleep(2000);
                    set_color(WHITE, BLACK);
                    
                    // reprend le jeu
                    jouerNiveau(&monNiveau);
                } else {
                    gotoxy(30, 10);
                    set_color(RED, BLACK);
                    printf("Erreur : Aucune sauvegarde trouvee.");
                    Sleep(2000);
                    set_color(WHITE, BLACK);
                }
                break;

            case 3: // QUITTER
                clrscr();
                gotoxy(30, 10);
                printf("Merci d'avoir joue ! A bientot.");
                Sleep(1000);
                break;
                
            case 4: // REGLES
                clrscr();
                set_color(CYAN, BLACK);
                gotoxy(30, 2);
                printf("=== REGLES DU JEU ===");
                
                set_color(WHITE, BLACK);
                gotoxy(5, 4);
                printf("BUT : Aligner 3 fruits ou plus pour les detruire et remplir le contrat.");
                
                // affiche les commandes
                gotoxy(5, 6);  printf("COMMANDES :");
                gotoxy(10, 7); printf("- Fleches : Deplacer le curseur");
                gotoxy(10, 8); printf("- Espace : Selectionner / Echanger");
                gotoxy(10, 9); printf("- X : Quitter / Sauvegarder");
                
                // affiche les combos
                gotoxy(5, 11); printf("FIGURES SPECIALES :");
                gotoxy(10, 12); printf("- 6 alignes : Supprime tous les fruits de cette couleur");
                gotoxy(10, 13); printf("- Croix de 9 : Supprime la ligne et la colonne");
                gotoxy(10, 14); printf("- Carre 4x4 : Supprime le carre entier");
                gotoxy(10, 15); printf("- 5 alignes (niveau 2+) : Bombe 3x3");
                
                gotoxy(5, 17);
                printf("OBJECTIF : Remplir le contrat avant la fin du temps ou des coups !");
                
                set_color(YELLOW, BLACK);
                gotoxy(25, 20);
                printf("Appuyez sur une touche pour revenir...");
                
                set_color(WHITE, BLACK);
                _getch(); // attend appui touche
                break;
        }

    } while (choixMenu != 3); // tant qu'on quitte pas

    return 0;
}