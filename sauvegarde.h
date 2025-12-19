#ifndef SAUVEGARDE_H
#define SAUVEGARDE_H

#include "structure.h"

// Sauvegarde toutes les données du niveau dans "sauvegarde.txt"
// Retourne 1 si réussi, 0 si erreur
int sauvegarderPartie(Niveau *niveau);

// Charge les données depuis "sauvegarde.txt"
// Retourne 1 si réussi, 0 si aucune sauvegarde trouvée
int chargerPartie(Niveau *niveau);

#endif
