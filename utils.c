/*
Nom du fichier : utils.c
Fait par : Erwann GIRAULT
But : Contient des fonctions utilitaires pour gérer les entrées utilisateurs 
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include "utils.h"

void vider_buffer_stdin(void) {
    /*
    Argument:
        Aucun.
    But:
        Vider le buffer d'entrée
        Lit caractère par caractère jusqu'au saut de ligne ou EOF
    Retour:
        Aucun
    */
    int c;
    // EOF = End Of File (fin de flux)
    while ((c = getchar()) != '\n' && c != EOF); 
}

bool lire_chaine_securisee(char *buffer, int taille_max) {
    /*
    Argument:
        buffer: Tableau de caractères de destination
        taille_max: Taille max du buffer
    But:
        Lire une chaîne
        Supprime le '\n' final et vide le buffer si l'entrée était trop longue
    Retour:
        true si la lecture est valide et non vide, false sinon
    */
    if (fgets(buffer, taille_max, stdin) == NULL) {
        return false; 
    }
    if (strlen(buffer) == 0) {
        return false;
    }

    // Recherche du saut de ligne
    char *p = strchr(buffer, '\n');
    if (p != NULL) {
        *p = '\0'; // On remplace \n par \0
    } else {
        // \n non trouvé -> l'utilisateur a entré plus que la taille max
        // on vide le buffer pour éviter les fuites sur la prochaine lecture
        vider_buffer_stdin();
    }
    return true;
}

bool lire_long_securise(long *valeur) {
    /*
    Argument:
        valeur: Pointeur pour stocker l'entier long converti
    But:
        Lire et convertir une entrée utilisateur en long
        Vérifie les dépassements et les caractères invalides
    Retour:
        true si la conversion est un succès, false sinon
    */
    char buffer[64]; 
    if (!lire_chaine_securisee(buffer, 64)) {
        return false;
    }

    char *endptr;
    errno = 0; // contient le code de la dernière erreur déclenchée
    long nombre = strtol(buffer, &endptr, 10);
    

    // 1. errno != 0 : erreur (overflow/underflow)
    // 2. endptr == buffer : aucun chiffre trouvé
    // 3. *endptr != '\0' : caractères invalides restants
    if (errno != 0 || endptr == buffer || *endptr != '\0') {
        return false;
    }

    if (nombre > INT_MAX || nombre < 0) return false;

    *valeur = nombre;
    return true;
}

bool lire_double_securise(double *valeur) {
    /*
    Argument:
        valeur: Pointeur pour stocker le nombre flottant converti
    But:
        Lire et convertir une entrée utilisateur en double 
    Retour:
        true si la conversion est un succès, false sinon
    */
    char buffer[64];
    if (!lire_chaine_securisee(buffer, 64)) {
        return false;
    }

    char *endptr;
    errno = 0;
    double nombre = strtod(buffer, &endptr);

    if (errno != 0 || endptr == buffer || *endptr != '\0') {
        return false;
    }

    *valeur = nombre;
    return true;
}