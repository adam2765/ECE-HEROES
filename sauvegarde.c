#include <stdio.h>        // Pour FILE, fopen, fclose, fprintf, fscanf
#include <time.h>         // Pour time(), difftime() (gestion du chrono)
#include "sauvegarde.h"   // Contient le type Niveau + constantes (LIGNES, COLONNES, NB_TYPES)

// Sauvegarde l'état de la partie dans un fichier texte
// Retourne 1 si tout s'est bien passé, 0 sinon

int sauvegarderPartie(Niveau *niveau) {                         // On reçoit l'adresse du niveau à sauvegarder
    FILE *fichier = fopen("sauvegarde.txt", "w");              // On ouvre (ou crée) le fichier en écriture
    if (fichier == NULL) return 0;                             // Si ouverture impossible -> échec direct

    // 1. Calcul du temps restant réel au moment de la sauvegarde
   
    int tempsEcoule = (int)difftime(time(NULL), niveau->startTime); // Temps passé depuis le début du niveau
    int tempsRestant = niveau->tempsTotalSec - tempsEcoule;         // Temps restant = total - écoulé
    if (tempsRestant < 0) tempsRestant = 0;                         // Si on dépasse, on évite les valeurs négatives

    // 2. Ecriture des variables simples
    // On sauvegarde : Niveau, Vies, Score, Coups, TempsRestant
   
    fprintf(fichier, "%d %d %d %d %d\n",                       // On écrit tout sur une ligne pour relire facilement
            niveau->numeroNiveau,                               // Numéro du niveau actuel
            niveau->vies,                                       // Nombre de vies restantes
            niveau->score,                                      // Score actuel
            niveau->coupsRestants,                              // Coups restants
            tempsRestant);                                      // On sauvegarde le temps RESTANT, pas le total initial

    // 3. Ecriture du Pseudo (chaine de caractères)
    // Astuce : Si le pseudo est vide, on écrit "Joueur"
    if (niveau->pseudo[0] == '\0') fprintf(fichier, "Joueur\n"); // Si le pseudo n'a pas été rempli -> valeur par défaut
    else fprintf(fichier, "%s\n", niveau->pseudo);               // Sinon on écrit le pseudo tel quel

    // 4. Ecriture du Contrat (Objectifs)
    for (int k = 1; k <= NB_TYPES; k++) {                       // On parcourt tous les types d'items (1..NB_TYPES)
        fprintf(fichier, "%d %d ",                              // Pour chaque type, on écrit 2 nombres
                niveau->contrat.quantiteCible[k],               // Objectif à atteindre pour ce type
                niveau->contrat.quantiteActuelle[k]);           // Progression actuelle (combien on en a déjà détruit)
    }
    fprintf(fichier, "\n");                                     // Fin de ligne du contrat

    // 5. Ecriture de la Grille
    for(int i = 0; i < LIGNES; i++) {                           // On parcourt toutes les lignes de la grille
        for(int j = 0; j < COLONNES; j++) {                     // Puis toutes les colonnes
            // On écrit juste le type (pas besoin de savoir si c'est sélectionné)
            fprintf(fichier, "%d ", niveau->grille[i][j].type); // On sauvegarde uniquement le type de l'item
        }
        fprintf(fichier, "\n");                                 // Fin de ligne de la grille
    }

    fclose(fichier);                                            // On ferme le fichier (important pour valider l'écriture)
    return 1;                                                   // Succès
}

// Charge l'état de la partie depuis le fichier texte
// Retourne 1 si chargé, 0 si pas de fichier / erreur
int chargerPartie(Niveau *niveau) {                             // On reçoit l'adresse du niveau à remplir
    FILE *fichier = fopen("sauvegarde.txt", "r");              // On ouvre le fichier en lecture
    if (fichier == NULL) return 0;                             // Si le fichier n'existe pas -> pas de sauvegarde

    int tempsRestantSauvegarde = 0;                            // Variable temporaire pour récupérer le temps restant

    // 1. Lecture des variables simples
    fscanf(fichier, "%d %d %d %d %d",                          // On relit exactement dans le même ordre qu'on a écrit
           &niveau->numeroNiveau,                               // On remplit numeroNiveau
           &niveau->vies,                                       // On remplit vies
           &niveau->score,                                      // On remplit score
           &niveau->coupsRestants,                              // On remplit coupsRestants
           &tempsRestantSauvegarde);                            // On récupère le temps restant sauvegardé

    // Réinitialisation du chrono
    niveau->tempsTotalSec = tempsRestantSauvegarde;            // On considère que "temps total" devient le temps restant
    niveau->startTime = time(NULL);                            // On relance le chrono à partir de maintenant

    // 2. Lecture du Pseudo
    fscanf(fichier, "%s", niveau->pseudo);                     // Lit un mot (sans espaces) dans pseudo

    // 3. Lecture du Contrat
    for (int k = 1; k <= NB_TYPES; k++) {                      // Pour chaque type d'item
        fscanf(fichier, "%d %d",                               // On relit les 2 valeurs comme à la sauvegarde
               &niveau->contrat.quantiteCible[k],              // Objectif
               &niveau->contrat.quantiteActuelle[k]);          // Avancement
    }

    // 4. Lecture de la Grille
    for(int i = 0; i < LIGNES; i++) {                          // On parcourt la grille
        for(int j = 0; j < COLONNES; j++) {
            int typeLu;                                        // Variable temporaire pour lire le type
            fscanf(fichier, "%d", &typeLu);                    // On lit un entier depuis le fichier
            niveau->grille[i][j].type = typeLu;                // On le met dans la case correspondante
            niveau->grille[i][j].estSelectionne = 0;           // On remet à zéro la sélection (pas utile en sauvegarde)
        }
    }

    fclose(fichier);                                           // On ferme le fichier
    return 1;                                                  // Succès
}
