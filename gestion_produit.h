#ifndef _GESTION_PRODUIT_H
#define _GESTION_PRODUIT_H

#include <stdint.h> 
#include <time.h>
#include <stdarg.h> // Nécessaire si on expose des variadiques, sinon pour le prototype simple c'est optionnel

/*
    Description de la structure Produit :
    - id : Identifiant unique (uint32_t).
    - nom : Chaîne allouée dynamiquement (max 64 chars).
    - description : Description libre (max 1024 chars).
    - categorie : Type de produit (potion, etc.).
    - quantite : Stock disponible.
    - prix_unitaire : Prix en flottant.
    - date_peremption : Timestamp (0 si non applicable).
    - note : Commentaire libre.
    - suivant : Pointeur vers le maillon suivant.
*/
typedef struct Produit {
    uint32_t id;              
    char *nom;
    char *description;  
    char *categorie;       
    int quantite;            
    float prix_unitaire;    
    char *note;  
    time_t date_peremption;
    struct Produit *suivant;  
} Produit;

Produit* modifier_produit(Produit* produit, const char* nom, const char* description, const char* categorie, int quantite, float prix_unitaire, time_t date_peremption, const char* note);
Produit* creer_produit(uint32_t id, const char* nom, const char* description, const char* categorie, int quantite, float prix_unitaire, time_t date_peremption, const char* note);
int suppression_par_id(Produit** head, uint32_t id);
int insertion(Produit** head, Produit* nouveau_produit);
int affichage(Produit** head);
Produit* rechercher_par_id(Produit* head, uint32_t id);
Produit* free_struct_produit(Produit* head);
void ajouter_log(const char *format, ...);

#endif