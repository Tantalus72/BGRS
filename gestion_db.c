/*
Nom du fichier : gestion_db.c
Fait par : Erwann GIRAULT
But : Gère la persistance des données. Contient les fonctions pour sauvegarder
      la liste dans un fichier et utiliser un fichier pour le chargement
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "gestion_produit.h"
#include "gestion_db.h"

#define DELIMITER "|"
#define MAX_LINE_LENGTH 2048 // taille nécessaire pour contenir une ligne complète 

char *separateur_chaine(char** str, const char* delim) {
    /*
    Argument:
        str: pointeur d'un pointeur vers le debut d'une chaine
        delim: pointeur vers le délimiteur
    But:
        à partir d'une chaine, on remplace les délimiteur par des \0 pour separer les élément entre eux
    Retour:
        Aucun
    */
    char *start = *str;
    char *p;

    if (start == NULL) {
        return NULL;
    }
    p = strpbrk(start, delim);

    if (p == NULL) {
        // cas du dernier token
        *str = NULL; 
    } else {
        // Délimiteur trouvé => on le remplace par \0 pour le "couper"
        *p = '\0';
        *str = p + 1;
    }

    return start;
}

void sauvegarde(Produit* head, const char* nom_fichier) {
    /*
    Argument:
        head: Pointeur vers la tête de la liste à sauvegarder
        nom_fichier: Chemin du fichier de destination
    But:
        Sérialiser l'inventaire dans un fichier texte 
    Retour:
        Aucun 
    */
    FILE* fichier = fopen(nom_fichier, "w"); 
    if (fichier == NULL) {
        fprintf(stderr, "[!] Erreur : Impossible d'ouvrir %s pour écriture.\n", nom_fichier);
        return;
    }

    Produit* actu = head;
    while (actu != NULL) {
        fprintf(fichier, "%u|%s|%s|%s|%d|%.2f|%ld|%s\n", 
                actu->id, 
                (actu->nom ? actu->nom : "Inconnu"), 
                (actu->description ? actu->description : ""), 
                (actu->categorie ? actu->categorie : "Divers"), 
                actu->quantite, 
                actu->prix_unitaire, 
                (long)actu->date_peremption, 
                (actu->note ? actu->note : ""));
        
        actu = actu->suivant;
    }

    fclose(fichier);
}

void charger_fichier(Produit** head, char* nom_fichier) {
    /*
    Argument:
        head: pointeur d'un pointeur vers la tête de la liste (pour insertion)
        nom_fichier: Chemin du fichier source à lire
    But:
        Lire un fichier ligne par ligne, parser les champs et reconstruire la liste chaînée
        Gère les erreurs de formatage et les lignes corrompues
    Retour:
        Aucun
    */
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("[i] Info : Aucun fichier de sauvegarde trouvé ou erreur d'ouverture.\n");
        return; 
    }

    char buffer[MAX_LINE_LENGTH];
    int ligne_count = 0;

    while (fgets(buffer, MAX_LINE_LENGTH, fichier)) {
        ligne_count++;
        
        // strcspn retourne l'indice de \n dans le buffer
        buffer[strcspn(buffer, "\n")] = 0;
        char* temp = buffer;
        

        // On extrait les champs un par un
        char* token_id = separateur_chaine(&temp, DELIMITER);
        char* token_nom = separateur_chaine(&temp, DELIMITER);
        char* token_desc = separateur_chaine(&temp, DELIMITER);
        char* token_cat = separateur_chaine(&temp, DELIMITER);
        char* token_qte = separateur_chaine(&temp, DELIMITER);
        char* token_prix = separateur_chaine(&temp, DELIMITER);
        char* token_date = separateur_chaine(&temp, DELIMITER);
        char* token_note = separateur_chaine(&temp, DELIMITER);

        // Si un des champs obligatoires est NULL, la ligne est corrompue donc abandonnée
        if (!token_id || !token_nom || !token_desc || !token_cat || !token_qte || !token_prix || !token_date || !token_note) {
            printf("Warning : Ligne %d corrompue (champs manquants), ignorée.\n", ligne_count);
            continue;
        }
        uint32_t id = (uint32_t)strtoul(token_id, NULL, 10);
        int quantite = (int)strtol(token_qte, NULL, 10);
        float prix = strtof(token_prix, NULL);
        
        time_t date_peremption = 0;
        if (token_date) {
            date_peremption = (time_t)strtol(token_date, NULL, 10);
        }

        char* note_safe = (token_note != NULL) ? token_note : "";

        Produit* nouveau_produit = creer_produit(id, token_nom, token_desc, token_cat, quantite, prix, date_peremption, note_safe);
        
        if (nouveau_produit != NULL) {
            insertion(head, nouveau_produit);
        } else {
            fprintf(stderr, "[!] Erreur : Echec allocation mémoire pour la ligne %d.\n", ligne_count);
        }
    }

    fclose(fichier);
}

