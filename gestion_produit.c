/*
Nom du fichier : gestion_produit.c
Fait par : Erwann GIRAULT
But : Contient la logique pour la manipulation de la structure Produit
*/




#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h> 
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "gestion_produit.h"

void ajouter_log(const char *format, ...) {
    /*
    Argument:
        format : Chaîne de caractère 
        ...    : Arguments variables 
    But:
        Ouvrir "historique.log" en mode ajout (append), générer un horodatage actuel,
        et écrire le message formaté directement dans le fichier.
    Retour:
        Aucun
    */
    FILE *f = fopen("historique.log", "a");
    if (f == NULL) {
        fprintf(stderr, "Impossible d'ecrire dans historique.log\n");
        return;
    }

    time_t now = time(NULL);
    char *date_str = ctime(&now);
    date_str[strcspn(date_str, "\n")] = 0; // Retire le \n du ctime
    
    fprintf(f, "[%s] ", date_str);

    va_list args;
    va_start(args, format);
    vfprintf(f, format, args);
    va_end(args);

    fprintf(f, "\n");
    fclose(f);
}

int insertion(Produit** head, Produit* nouveau_produit) {
    /*
    Argument:
        head: pointeur d'un pointeur vers la tête de la liste des produits
        nouveau_produit: Pointeur vers le nouveau produit à insérer
    But:
        Insérer le nouveau produit au début de la liste chaînée
    Retour:
        0 si l'insertion a réussi, -1 sinon
    */
    if (head == NULL || nouveau_produit == NULL) {
        return -1; 
    }
    else {
        nouveau_produit->suivant = *head; // Le nouveau pointe vers l'ancien premier
        *head = nouveau_produit; // La tête pointe vers le nouveau

        ajouter_log("[+] Ajout du produit ID %u : %s (Qte: %d)", 
                    nouveau_produit->id, 
                    nouveau_produit->nom, 
                    nouveau_produit->quantite);
    
    }
    return 0; 
}

int affichage(Produit** head) {
    /*
    Argument:
        head: pointeur du pointeur vers le premier élément de la liste
    But:
        Afficher tous les produits de la liste sur la sortie standard
    Retour:
        0 si l'affichage a réussi
    */
    Produit* actu = *head;
    while (actu != NULL) {
        printf("ID: %u\n", actu->id);
        printf("Nom: %s\n", actu->nom);
        printf("Categorie: %s\n", actu->categorie);
        printf("Description: %s\n", actu->description);    
        printf("Prix Unitaire: %.2f\n", actu->prix_unitaire);    
        printf("Quantite: %d\n", actu->quantite);             
        printf("Date de Peremption: %s", ctime(&actu->date_peremption)); // ctime ajoute déjà un \n
        printf("-------------------------\n");
        actu = actu->suivant;
    }
    return 0; 
}

Produit* rechercher_par_id(Produit* head, uint32_t id) {
    /*
    Argument:
        head: Pointeur vers le premier élément de la liste
        id: Identifiant du produit à rechercher
    But:
        Rechercher un produit spécifique grâce à son identifiant unique
    Retour:
        Pointeur vers le produit trouvé, ou NULL si non trouvé
    */
    if (id <= 0) {
        return NULL; 
    }
    Produit* actu = head;
    while (actu != NULL) {
        if (actu->id == id) {
            return actu; 
        }
        actu = actu->suivant;
    }
    return NULL; 
}

int suppression_par_id(Produit** head, uint32_t id) {
    /*
    Argument:
        head: pointeur d'un pointeur vers la tête de la liste
        id: Identifiant du produit à supprimer
    But:
        Supprimer un produit de la liste et libérer toute la mémoire associée
    Retour:
        0 si la suppression a réussi, -1 sinon (liste vide ou ID introuvable)
    */
    if (head == NULL || *head == NULL) {
        return -1; 
    }

    Produit* actu = *head;
    Produit* actu_moins_1 = NULL; 

    // Recherche du produit a supprimer
    while (actu != NULL && actu->id != id) {
        actu_moins_1 = actu;       
        actu = actu->suivant;      
    }

    // Si le produit n'a pas ete trouve
    if (actu == NULL) {
        return -1; 
    }
    ajouter_log("[-] Suppression du produit ID %u : %s", actu->id, actu->nom);

    if (actu_moins_1 == NULL) {
        // cas ou le produit a supprimer est le premier de la liste
        *head = actu->suivant; 
    } else {
        // cas general
        actu_moins_1->suivant = actu->suivant; 
    }

    // Libération de la mémoire
    free(actu->nom);
    free(actu->description);
    free(actu->categorie);
    free(actu->note);
    free(actu); 
    return 0; 
}

Produit* creer_produit(uint32_t id, const char* nom, const char* description, const char* categorie, int quantite, float prix_unitaire, time_t date_peremption, const char* note) {
    /*
    Argument:
        id: Identifiant unique
        nom, description, categorie, note: Chaînes de caractères 
        quantite: Stock disponible (doit être positif)
        prix_unitaire: Prix (doit être positif)
        date_peremption: Timestamp 
    But:
        Allouer et initialiser un nouveau produit avec une copie des chaînes
    Retour:
        Pointeur vers le nouveau produit créé, ou NULL en cas d'erreur d'allocation ou de paramètre invalide
    */
    
    if (quantite < 0 || prix_unitaire < 0) {
        return NULL; 
    }
    if (strlen(nom) > 64 || strlen(description) > 1024) return NULL;
    Produit* np = (Produit*)calloc(1, sizeof(Produit));
    if (np == NULL) return NULL;
    
    // Allocation dynamique des champs string (+1 pour le \0)
    np->nom = (char*)malloc((strlen(nom) + 1) * sizeof(char));
    np->description = (char*)malloc((strlen(description) + 1) * sizeof(char));
    np->categorie = (char*)malloc((strlen(categorie) + 1) * sizeof(char));
    np->note = (char*)malloc((strlen(note) + 1) * sizeof(char));

    // Vérification atomique des allocations
    if (np->nom == NULL || np->description == NULL || np->categorie == NULL || np->note == NULL) {
        free(np->nom);
        free(np->description);
        free(np->categorie);
        free(np->note);
        free(np);
        return NULL;
    }

    // Copie des données
    snprintf(np->nom, strlen(nom) + 1, "%s", nom);
    snprintf(np->description, strlen(description) + 1, "%s", description);
    snprintf(np->categorie, strlen(categorie) + 1, "%s", categorie);
    snprintf(np->note, strlen(note) + 1, "%s", note);
    

    np->id = id;
    np->quantite = quantite;
    np->prix_unitaire = prix_unitaire;
    np->date_peremption = date_peremption;
    np->suivant = NULL;

    return np; 
}

Produit* modifier_produit(Produit* produit, const char* nom, const char* description, const char* categorie, int quantite, float prix_unitaire, time_t date_peremption, const char* note) {
    /*
    Argument:
        produit: Pointeur vers le produit à modifier
        nom, description, categorie, note: Nouvelles données textuelles
        quantite, prix_unitaire, date_peremption: Nouvelles données numériques
    But:
        Modifier les informations d'un produit existant 
    Retour:
        Pointeur vers le produit modifié, ou NULL en cas d'erreur
    */

    // Phase de vérification 
    if (produit == NULL) return NULL;
    if (strlen(nom) > 64 || strlen(description) > 1024) return NULL;
    if (quantite < 0 || prix_unitaire < 0) {
        return NULL; 
    }

    // Allocation temporaire pour garantir l'ancien produit
    char* temp_nom = (char*)malloc((strlen(nom) + 1) * sizeof(char));
    char* temp_desc = (char*)malloc((strlen(description) + 1) * sizeof(char));
    char* temp_cat = (char*)malloc((strlen(categorie) + 1) * sizeof(char));
    char* temp_note = (char*)malloc((strlen(note) + 1) * sizeof(char));

    if (!temp_nom || !temp_desc || !temp_cat || !temp_note) {
        free(temp_nom);
        free(temp_desc);
        free(temp_cat);
        free(temp_note);
        return NULL;
    }

    snprintf(temp_nom, strlen(nom) + 1, "%s", nom);
    snprintf(temp_desc, strlen(description) + 1, "%s", description);
    snprintf(temp_cat, strlen(categorie) + 1, "%s", categorie);
    snprintf(temp_note, strlen(note) + 1, "%s", note);

    // Phase de remplacement (libération ancienne mémoire)
    free(produit->nom);
    free(produit->description);
    free(produit->categorie);
    free(produit->note);

    produit->nom = temp_nom;
    produit->description = temp_desc;
    produit->categorie = temp_cat;
    produit->note = temp_note;

    produit->quantite = quantite;
    produit->prix_unitaire = prix_unitaire;
    produit->date_peremption = date_peremption;
    
    ajouter_log("[~] Modification du produit ID %u (Nouveau Nom: %s)", produit->id, produit->nom);

    return produit;
}

Produit* free_struct_produit(Produit* head) {
    /*
    Argument:
        head: Pointeur vers le premier élément de la liste
    But:
        Libérer toute la mémoire de la liste (structure + champs dynamiques) pour éviter les fuites
    Retour:
        NULL (Pour réinitialiser le pointeur de tête)
    */
    Produit* actu = head;
    Produit* temp_suivant = NULL;

    while (actu != NULL) {
        temp_suivant = actu->suivant; // On "sauve" l'addresse pour continuer la suppression

        free(actu->nom);
        free(actu->description);
        free(actu->categorie);
        free(actu->note);

        free(actu);

        actu = temp_suivant;
    }

    return NULL; 
}
