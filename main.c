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
    clrscr(); // on nettoie completement l'ecran
    
    set_color(GREEN, BLACK); // on met le texte en vert
    gotoxy(25, 8); // on place le curseur
    printf("============================="); // cadre haut
    gotoxy(25, 9); // ligne suivante
    printf("     NIVEAU %d TERMINE !     ", niveau->numeroNiveau); // message bravo
    gotoxy(25, 10); // ligne suivante
    printf("============================="); // cadre bas
    
    set_color(WHITE, BLACK); // on remet en blanc
    gotoxy(25, 12); // un peu plus bas
    printf("Score : %d", niveau->score); // on montre le score
    gotoxy(25, 13); // ligne d'apres
    printf("Vies restantes : %d", niveau->vies); // on montre les vies
    
    // on verifie si c'est un niveau intermediaire
    if (niveau->numeroNiveau < 3) {
        set_color(YELLOW, BLACK); // texte en jaune
        gotoxy(25, 15); // positionnement
        printf("Prochain niveau dans 3 secondes..."); // info joueur
    } else {
        // sinon c'est la fin du jeu complet
        set_color(MAGENTA, BLACK); // texte magenta pour la fete
        gotoxy(25, 15); // positionnement
        printf("FELICITATIONS ! JEU TERMINE !"); // message final
        gotoxy(25, 16); // ligne suivante
        printf("Score final : %d", niveau->score); // score total
    }
    
    set_color(WHITE, BLACK); // on remet le blanc
    Sleep(3000); // on bloque tout pendant 3 secondes pour lire
}

// fonction pour afficher quand on perd
void afficherDefaite(Niveau *niveau, char* raison) {
    clrscr(); // on efface l'ecran
    
    set_color(RED, BLACK); // texte en rouge (danger)
    gotoxy(25, 8); // positionnement
    printf("============================="); // cadre
    gotoxy(25, 9); // ligne suivante
    printf("          GAME OVER !          "); // message perdu
    gotoxy(25, 10); // ligne suivante
    printf("============================="); // cadre
    
    set_color(WHITE, BLACK); // texte blanc
    gotoxy(25, 12); // positionnement
    printf("Raison : %s", raison); // on affiche pourquoi (temps/coups)
    gotoxy(25, 13); // ligne suivante
    printf("Score : %d", niveau->score); // score final
    gotoxy(25, 14); // ligne suivante
    printf("Niveau : %d", niveau->numeroNiveau); // niveau atteint
    
    // on regarde s'il reste des vies
    if (niveau->vies > 1) {
        set_color(YELLOW, BLACK); // texte jaune (espoir)
        gotoxy(25, 16); // positionnement
        printf("Il vous reste %d vies !", niveau->vies - 1); // vies restantes -1
        gotoxy(25, 17); // ligne suivante
        printf("Appuyez sur une touche pour reessayer..."); // instruction
    } else {
        // plus aucune vie, c'est fini
        set_color(RED, BLACK); // texte rouge
        gotoxy(25, 16); // positionnement
        printf("Plus de vies ! Retour au menu..."); // message retour
    }
    
    set_color(WHITE, BLACK); // texte blanc
    Sleep(2000); // pause de 2 secondes
    
    // si on peut rejouer, on attend une touche
    if (niveau->vies > 1) _getch(); 
}

// ============== JEU PRINCIPAL ==============

// c'est la fonction principale qui gere tout le niveau
int jouerNiveau(Niveau *monNiveau) {
    int curseurX = 0; // position X du curseur (colonne)
    int curseurY = 0; // position Y du curseur (ligne)
    int jeuEnCours = 1; // variable pour garder la boucle active
    int codeTouche; // pour stocker la touche clavier
    int resultat = 0; // 0=perdu, 1=gagné, -1=quitté

    // on dessine le cadre du jeu
    afficherCadre();

    // on sauvegarde l'etat actuel au cas ou on perd
    int scoreAvant = monNiveau->score; // score actuel
    int viesAvant = monNiveau->vies;   // vies actuelles

    // on prepare le terrain
    gererCombos(monNiveau); // on supprime les alignements existants
    
    // on remet les compteurs d'objectifs a zero
    for (int k = 0; k <= NB_TYPES; k++) {
        monNiveau->contrat.quantiteActuelle[k] = 0; // reset compteur
    }
    
    // on restaure les stats propres
    monNiveau->score = scoreAvant; // remet le score
    monNiveau->vies = viesAvant;   // remet les vies

    // BOUCLE INFINIE : tant que le jeu tourne
    while(jeuEnCours) {
        
        // 1. AFFICHAGE
        afficherGrille(monNiveau, curseurX, curseurY); // on affiche la grille + curseur
        afficherHUD(monNiveau); // on affiche les infos a droite

        // 2. VERIFICATION VICTOIRE
        if (verifierContrat(monNiveau)) { // est-ce que les objectifs sont atteints ?
            afficherVictoire(monNiveau); // on affiche l'ecran gagne
            resultat = 1; // code 1 pour dire "gagné"
            jeuEnCours = 0; // on arrete la boucle
            break; // on sort du while
        }

        // 3. VERIFICATION DEFAITE (COUPS)
        if (monNiveau->coupsRestants <= 0) { // si plus de coups
            afficherDefaite(monNiveau, "Plus de coups !"); // ecran perdu
            resultat = 0; // code 0 pour dire "perdu"
            jeuEnCours = 0; // on arrete la boucle
            break; // on sort
        }

        // 4. VERIFICATION TEMPS
        // on compare l'heure actuelle avec l'heure de depart
        if (difftime(time(NULL), monNiveau->startTime) > monNiveau->tempsTotalSec) {
            afficherDefaite(monNiveau, "Temps ecoule !"); // ecran perdu temps
            resultat = 0; // code 0 perdu
            jeuEnCours = 0; // on arrete
            break; // on sort
        }

        // 5. GESTION DES TOUCHES
        if (_kbhit()) { // est-ce qu'une touche est appuyee ?
            codeTouche = _getch(); // on lit la touche

            // --- SI C'EST UNE FLECHE ---
            if (codeTouche == 224 || codeTouche == 0) {
                codeTouche = _getch(); // on lit le code de la fleche
                switch (codeTouche) {
                    case 72: if(curseurY > 0) curseurY--; break;        // Fleche HAUT : on monte
                    case 80: if(curseurY < LIGNES-1) curseurY++; break; // Fleche BAS : on descend
                    case 75: if(curseurX > 0) curseurX--; break;        // Fleche GAUCHE : on va a gauche
                    case 77: if(curseurX < COLONNES-1) curseurX++; break;// Fleche DROITE : on va a droite
                }
            }
            // --- TOUCHE ESPACE (ACTION) ---
           else if (codeTouche == 32) { 
                
                // on prepare des variables temporaires
                int xPrecedent = -1, yPrecedent = -1; // positions precedente
                int dejaSelectionne = 0; // indicateur si une case est deja rouge

                // on scanne toute la grille pour trouver une selection
                for(int i=0; i<LIGNES; i++) {
                    for(int j=0; j<COLONNES; j++) {
                        if (monNiveau->grille[i][j].estSelectionne) {
                            dejaSelectionne = 1; // on a trouve une selection !
                            yPrecedent = i; // on garde la ligne
                            xPrecedent = j; // on garde la colonne
                        }
                    }
                }

                if (dejaSelectionne) {
                    // si on clique exactement sur la meme case
                    if (xPrecedent == curseurX && yPrecedent == curseurY) {
                        monNiveau->grille[curseurY][curseurX].estSelectionne = 0; // on annule la selection
                    }
                    else {
                        // si on clique sur une case differente
                        // on calcule la distance (doit etre egale a 1)
                        if (abs(curseurX - xPrecedent) + abs(curseurY - yPrecedent) == 1) {
                            
                            // on fait l'echange physique des cases
                            echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                          &monNiveau->grille[curseurY][curseurX]);
                            
                            // on retire la couleur rouge de selection
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0;
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 0;

                            // LE MOMENT IMPORTANT : on verifie si ca fait un alignement
                            if (detecterEtSupprimerAlignements(monNiveau)) {
                                monNiveau->coupsRestants--; // ca a marche, on use un coup
                                faireTomberEtRemplir(monNiveau); // on fait tomber les bonbons
                                gererCombos(monNiveau); // on verifie les reactions en chaine
                            } 
                            else {
                                // si ca ne fait rien, on annule tout
                                echangerCases(&monNiveau->grille[yPrecedent][xPrecedent], 
                                                &monNiveau->grille[curseurY][curseurX]); // retour arriere
                            }

                        } else {
                            // si la case est trop loin
                            monNiveau->grille[yPrecedent][xPrecedent].estSelectionne = 0; // on oublie l'ancienne
                            monNiveau->grille[curseurY][curseurX].estSelectionne = 1; // on selectionne la nouvelle
                        }
                    }
                } 
                else {
                    // si rien n'etait selectionne avant
                    monNiveau->grille[curseurY][curseurX].estSelectionne = 1; // on selectionne la case actuelle
                }
            }
            // --- TOUCHE X (QUITTER) ---
           else if (codeTouche == 'x' || codeTouche == 'X') {
                
                // on affiche la question en bas
                gotoxy(2, LIGNES+4); 
                set_color(YELLOW, BLACK); // texte jaune
                printf("Voulez-vous sauvegarder avant de quitter ? (O/N) ");
                
                char reponse = _getch(); // on attend que le joueur reponde
                
                if (reponse == 'o' || reponse == 'O') {
                    // on lance la sauvegarde
                    if(sauvegarderPartie(monNiveau)) {
                        gotoxy(2, LIGNES+4); 
                        set_color(GREEN, BLACK); // texte vert
                        printf(">> SAUVEGARDE REUSSIE ! A BIENTOT <<       "); // confirmation
                        Sleep(1500); // petite pause
                    }
                }
                else {
                    // si il veut pas sauvegarder, on supprime le fichier pour eviter la triche
                    remove("sauvegarde.txt");
                }
                
                resultat = -1; // code special pour quitter
                jeuEnCours = 0; // on arrete la boucle
            }
        } 
        Sleep(10); // petite pause pour soulager le processeur
    } 
    return resultat; // on renvoie le resultat final
}

// --- MAIN (DEBUT DU PROGRAMME) ---
int main() {
    SetConsoleOutputCP(65001); // pour afficher les caracteres speciaux
    hide_cursor();             // on cache le curseur qui clignote
    
    Niveau monNiveau; // creation de la structure niveau
    int choixMenu = 0; // variable pour le menu

    // boucle du menu principal
    do {
        choixMenu = afficherMenuPrincipal(); // on affiche le menu et recupere le choix

        switch (choixMenu) {
            case 1: // CAS 1 : NOUVELLE PARTIE
            {
                // on vide toute la memoire de la structure pour etre propre
                memset(&monNiveau, 0, sizeof(Niveau));
                
                clrscr(); // nettoyage ecran
                gotoxy(30, 10); // position
                printf("Entrez votre pseudo : "); // demande pseudo
                scanf("%s", monNiveau.pseudo); // lecture pseudo
                
                // initialisation des variables de jeu
                int niveauActuel = 1; // on commence niveau 1
                int continuer = 1; // booleen pour continuer
                int scoreTotal = 0; // score global
                int viesRestantes = 3; // 3 vies au depart
                
                // boucle pour enchainer les niveaux 1, 2, 3
                while (niveauActuel <= 3 && continuer) {
                    // on initialise le plateau pour le niveau en cours
                    initialiserNiveau(&monNiveau, niveauActuel);
                    // on reprend le score et les vies la ou on en etait
                    monNiveau.score = scoreTotal;
                    monNiveau.vies = viesRestantes;
                    
                    // on lance la partie !
                    int resultat = jouerNiveau(&monNiveau);
                    
                    if (resultat == 1) { // si on a gagné
                        scoreTotal = monNiveau.score; // on garde le score
                        viesRestantes = monNiveau.vies; // on garde les vies
                        niveauActuel++; // on passe au niveau suivant
                    } 
                    else if (resultat == 0) { // si on a perdu
                        viesRestantes--; // on retire une vie
                        if (viesRestantes <= 0) continuer = 0; // si 0 vie, game over
                    }
                    else { // si on a quitté avec X
                        continuer = 0; // on arrete tout
                    }
                }
            }
            break; // fin du case 1

            case 2: // CAS 2 : CHARGER PARTIE
                clrscr(); // efface ecran
                // on essaie de charger le fichier
                if (chargerPartie(&monNiveau)) {
                    gotoxy(30, 10); // position
                    set_color(GREEN, BLACK); // vert
                    printf("Chargement reussi ! Bon retour %s.", monNiveau.pseudo); // succes
                    Sleep(2000); // pause
                    set_color(WHITE, BLACK); // blanc
                    
                    // on relance le jeu directement
                    jouerNiveau(&monNiveau);
                } else {
                    gotoxy(30, 10); // position
                    set_color(RED, BLACK); // rouge
                    printf("Erreur : Aucune sauvegarde trouvee."); // erreur
                    Sleep(2000); // pause
                    set_color(WHITE, BLACK); // blanc
                }
                break; // fin du case 2

            case 3: // CAS 3 : QUITTER
                clrscr(); // efface ecran
                gotoxy(30, 10); // position
                printf("Merci d'avoir joue ! A bientot."); // au revoir
                Sleep(1000); // petite pause
                break; // fin du case 3
                
            case 4: // CAS 4 : REGLES
                clrscr(); // efface ecran
                set_color(CYAN, BLACK); // cyan
                gotoxy(30, 2); // position titre
                printf("=== REGLES DU JEU ==="); // titre
                
                set_color(WHITE, BLACK); // blanc
                gotoxy(5, 4); // position
                printf("BUT : Aligner 3 fruits ou plus pour les detruire et remplir le contrat.");
                
                // affichage des commandes
                gotoxy(5, 6);  printf("COMMANDES :");
                gotoxy(10, 7); printf("- Fleches : Deplacer le curseur");
                gotoxy(10, 8); printf("- Espace : Selectionner / Echanger");
                gotoxy(10, 9); printf("- X : Quitter / Sauvegarder");
                
                // affichage des figures speciales
                gotoxy(5, 11); printf("FIGURES SPECIALES :");
                gotoxy(10, 12); printf("- 6 alignes : Supprime tous les fruits de cette couleur");
                gotoxy(10, 13); printf("- Croix de 9 : Supprime la ligne et la colonne");
                gotoxy(10, 14); printf("- Carre 4x4 : Supprime le carre entier");
                gotoxy(10, 15); printf("- 5 alignes (niveau 2+) : Bombe 3x3");
                
                gotoxy(5, 17); // position
                printf("OBJECTIF : Remplir le contrat avant la fin du temps ou des coups !");
                
                set_color(YELLOW, BLACK); // jaune
                gotoxy(25, 20); // position
                printf("Appuyez sur une touche pour revenir...");
                
                set_color(WHITE, BLACK); // blanc
                _getch(); // on attend que le joueur appuie
                break; // fin du case 4
        }

    } while (choixMenu != 3); // tant qu'on ne choisit pas de quitter

    return 0; // fin du programme
}
