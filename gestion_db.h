#ifndef _GESTION_DB_H
#define _GESTION_DB_H

#include "gestion_produit.h"

void charger_fichier(Produit** head, char* nom_fichier);

void sauvegarde(Produit* head, const char* nom_fichier);


#endif