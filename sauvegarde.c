#include <stdio.h>
#include <time.h>
#include "sauvegarde.h"

int sauvegarderPartie(Niveau *niveau) {
    FILE *fichier = fopen("sauvegarde.txt", "w");
    if (fichier == NULL) return 0; // Erreur d'ouverture

    // 1. Calcul du temps restant réel au moment de la sauvegarde
    int tempsEcoule = (int)difftime(time(NULL), niveau->startTime);
    int tempsRestant = niveau->tempsTotalSec - tempsEcoule;
    if (tempsRestant < 0) tempsRestant = 0;

    // 2. Ecriture des variables simples
    // On sauvegarde : Niveau, Vies, Score, Coups, TempsRestant
    fprintf(fichier, "%d %d %d %d %d\n", 
            niveau->numeroNiveau, 
            niveau->vies, 
            niveau->score, 
            niveau->coupsRestants,
            tempsRestant); // On sauvegarde le reste, pas le total initial

    // 3. Ecriture du Pseudo (chaine de caractères)
    // Astuce : Si le pseudo est vide, on écrit "Joueur"
    if (niveau->pseudo[0] == '\0') fprintf(fichier, "Joueur\n");
    else fprintf(fichier, "%s\n", niveau->pseudo);

    // 4. Ecriture du Contrat (Objectifs)
    for (int k = 1; k <= NB_TYPES; k++) {
        fprintf(fichier, "%d %d ", 
                niveau->contrat.quantiteCible[k], 
                niveau->contrat.quantiteActuelle[k]);
    }
    fprintf(fichier, "\n");

    // 5. Ecriture de la Grille
    for(int i = 0; i < LIGNES; i++) {
        for(int j = 0; j < COLONNES; j++) {
            // On écrit juste le type (pas besoin de savoir si c'est sélectionné)
            fprintf(fichier, "%d ", niveau->grille[i][j].type);
        }
        fprintf(fichier, "\n");
    }

    fclose(fichier);
    return 1; // Succès
}

int chargerPartie(Niveau *niveau) {
    FILE *fichier = fopen("sauvegarde.txt", "r");
    if (fichier == NULL) return 0; // Pas de sauvegarde existante

    int tempsRestantSauvegarde = 0;

    // 1. Lecture des variables simples
    fscanf(fichier, "%d %d %d %d %d", 
           &niveau->numeroNiveau, 
           &niveau->vies, 
           &niveau->score, 
           &niveau->coupsRestants,
           &tempsRestantSauvegarde);

    // Réinitialisation du chrono
    niveau->tempsTotalSec = tempsRestantSauvegarde;
    niveau->startTime = time(NULL); // On relance le chrono MAINTENANT

    // 2. Lecture du Pseudo
    fscanf(fichier, "%s", niveau->pseudo);

    // 3. Lecture du Contrat
    for (int k = 1; k <= NB_TYPES; k++) {
        fscanf(fichier, "%d %d", 
               &niveau->contrat.quantiteCible[k], 
               &niveau->contrat.quantiteActuelle[k]);
    }

    // 4. Lecture de la Grille
    for(int i = 0; i < LIGNES; i++) {
        for(int j = 0; j < COLONNES; j++) {
            int typeLu;
            fscanf(fichier, "%d", &typeLu);
            niveau->grille[i][j].type = typeLu;
            niveau->grille[i][j].estSelectionne = 0; // On reset la sélection
        }
    }

    fclose(fichier);
    return 1; // Succès
}
