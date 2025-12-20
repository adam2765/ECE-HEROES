#include <stdio.h>        
#include <time.h>         // Gestion du temps
#include "sauvegarde.h"   // Structures du jeu 


// Sauvegarde l'état actuel de la partie
// Renvoie 1 si tout se passe bien, 0 sinon
int sauvegarderPartie(Niveau *niveau) {

    FILE *fichier = fopen("sauvegarde.txt", "w");   // On ouvre le fichier en écriture
    if (fichier == NULL) return 0;                  // Si ouverture impossible, on arrête


    // Calcul du temps écoulé depuis le début du niveau
    int tempsEcoule = (int)difftime(time(NULL), niveau->startTime); // Temps passé

    // Calcul du temps restant réel
    int tempsRestant = niveau->tempsTotalSec - tempsEcoule;         // Temps restant

    // Sécurité : on évite d'avoir un temps négatif
    if (tempsRestant < 0) tempsRestant = 0;


    // Sauvegarde des informations principales du jeu
    fprintf(fichier, "%d %d %d %d %d\n",   // Tout sur une ligne pour simplifier la lecture
            niveau->numeroNiveau,          // Numéro du niveau
            niveau->vies,                  // Nombre de vies restantes
            niveau->score,                 // Score actuel
            niveau->coupsRestants,         // Nombre de coups restants
            tempsRestant);                 // Temps restant à jouer


    // Sauvegarde du pseudo du joueur
    if (niveau->pseudo[0] == '\0')         // Si le pseudo est vide
        fprintf(fichier, "Joueur\n");      // On écrit un pseudo par défaut
    else
        fprintf(fichier, "%s\n", niveau->pseudo); // Sinon on écrit le vrai pseudo


    // Sauvegarde du contrat (objectifs du niveau)
    for (int k = 1; k <= NB_TYPES; k++) {  // Pour chaque type d'item
        fprintf(fichier, "%d %d ",         // On écrit deux valeurs
                niveau->contrat.quantiteCible[k],     // Objectif à atteindre
                niveau->contrat.quantiteActuelle[k]); // Progression actuelle
    }
    fprintf(fichier, "\n");                // Fin de la ligne du contrat


    // Sauvegarde de la grille de jeu
    for (int i = 0; i < LIGNES; i++) {     // Parcours des lignes
        for (int j = 0; j < COLONNES; j++) { // Parcours des colonnes
            fprintf(fichier, "%d ",        // On écrit le type de l'item
                    niveau->grille[i][j].type); // Pas besoin de sauver la sélection
        }
        fprintf(fichier, "\n");            // Retour à la ligne pour la grille
    }

    fclose(fichier);                       // On ferme le fichier (important)
    return 1;                              // Sauvegarde réussie
}


// Charge une partie sauvegardée depuis le fichier
// Renvoie 1 si succès, 0 s'il n'y a pas de sauvegarde
int chargerPartie(Niveau *niveau) {

    FILE *fichier = fopen("sauvegarde.txt", "r"); // Ouverture du fichier en lecture
    if (fichier == NULL) return 0;                 // Si pas de fichier, on arrête

    int tempsRestantSauvegarde = 0;                // Variable temporaire pour le temps


    // Lecture des informations principales
    fscanf(fichier, "%d %d %d %d %d",  // Lecture dans le même ordre que la sauvegarde
           &niveau->numeroNiveau,      // Numéro du niveau
           &niveau->vies,              // Vies restantes
           &niveau->score,             // Score
           &niveau->coupsRestants,     // Coups restants
           &tempsRestantSauvegarde);   // Temps restant sauvegardé


    // Réinitialisation du chrono
    niveau->tempsTotalSec = tempsRestantSauvegarde; // Le temps restant devient le nouveau total
    niveau->startTime = time(NULL);                 // Le chrono repart à maintenant


    // Lecture du pseudo
    fscanf(fichier, "%s", niveau->pseudo);          // On recharge le pseudo


    // Lecture du contrat
    for (int k = 1; k <= NB_TYPES; k++) {            // Pour chaque type d'item
        fscanf(fichier, "%d %d",                     // On lit les deux valeurs
               &niveau->contrat.quantiteCible[k],    // Objectif
               &niveau->contrat.quantiteActuelle[k]); // Progression
    }


    // Lecture de la grille
    for (int i = 0; i < LIGNES; i++) {               // Parcours des lignes
        for (int j = 0; j < COLONNES; j++) {         // Parcours des colonnes
            int typeLu;                              // Variable temporaire
            fscanf(fichier, "%d", &typeLu);          // Lecture du type
            niveau->grille[i][j].type = typeLu;      // Affectation dans la grille
            niveau->grille[i][j].estSelectionne = 0; // On enlève toute sélection
        }
    }

    fclose(fichier);                                 // Fermeture du fichier
    return 1;                                        // Chargement réussi
}
