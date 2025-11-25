#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h> 
#include <time.h>
#include <string.h>
#include <stdbool.h>


typedef struct Produit {
    /*
    Structure representant un produit dans l'inventaire
    id :              Identifiant unique attribué automatiquement par la bibliothèque (uint32_t)
    nom :             Nom court du produit, chaîne allouée dynamiquement (max. 64 caractères)
    description :     Description libre (max. 1024 caractères)
    quantite :        Nombre d’unités disponibles en stock
    prix_unitaire :   Prix unitaire (flottant positif)
    categorie :       Type du produit (potion, pansement, etc.)
    date_peremption : Date d’expiration (en time_t), 0 si non applicable
    note :            Note libre (max. 256 caractères)
    suivant :         Pointeur vers le produit suivant dans la liste chaînée
    */


    // Donnees     
    uint32_t id;              
    char *nom;
    char *description;  
    
    char *categorie;       
    int quantite;            
    float prix_unitaire;    
    char *note;  

    time_t date_peremption;   // Date d'expiration, (0 si non applicable)

    // Pointeur vers la suivant
    struct Produit *suivant;  

} Produit;

int insertion(Produit** head, Produit* nouveau_produit) {
    /*
    Argument:
        head: pointeur vers le pointeur de tete de liste des produits
        nouveau_produit: pointeur vers le nouveau produit a inserer
    But:
        inserer le nouveau produit au debut de la liste
    Retour:
        0 si l'insertion a reussi, -1 sinon
    */
    if (head == NULL || nouveau_produit == NULL) {
        return -1; 
    }

    nouveau_produit->suivant = *head; // remplace le pointeur "suivant" de "nouveau_produit" pour qu'il pointe vers l'ancien premier element "head"
    *head = nouveau_produit; // met a jour "head" pour qu'il pointe vers "nouveau_produit", le nouvel element en tete de liste (nouveau head)

    return 0; 
}

int affichage(Produit* head) {
    /*
    Argument:
        head: pointeur vers le premier element de la liste des produits
    But:
        afficher tous les produits de la liste
    Retour:
        0 si l'affichage a reussi, -1 sinon
    */
    Produit* actu = head;
    while (actu != NULL) {
        printf("ID: %u\n", actu->id);
        printf("Nom: %s\n", actu->nom);
        printf("Categorie: %s\n", actu->categorie);
        printf("Description: %s\n", actu->description);    
        printf("Prix Unitaire: %.2f\n", actu->prix_unitaire);    
        printf("Quantite: %d\n", actu->quantite);             
        printf("Date de Peremption: %s\n", ctime(&actu->date_peremption)); // ctime transforme le time_t en une chaine lisible
        printf("-------------------------\n");
        actu = actu->suivant;
    }
    return 0; 
}

Produit* rechercher_par_id(Produit* head, uint32_t id) {
    /*
    Argument:
        head: pointeur vers le premier element de la liste des produits
        id: identifiant du produit a rechercher
    But:
        rechercher le produit avec l'identifiant id dans la liste
    Retour:
        pointeur vers le produit trouve, ou NULL si non trouve
    */
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
        head: pointeur vers le pointeur de tete de liste des produits
        id: identifiant du produit a supprimer
    But:
        supprimer le produit avec l'identifiant id de la liste
    Retour:
        0 si la suppression a reussi, -1 sinon
    */
    if (head == NULL || *head == NULL) {
        return -1; 
    }

    Produit* actu = *head;
    Produit* actu_moins_1 = NULL; 

    // Recherche du produit a supprimer (on garde aussi le precedent)
    while (actu != NULL && actu->id != id) {
        actu_moins_1 = actu;       
        actu = actu->suivant;      
    }

    // Si le produit n'a pas ete trouve
    if (actu == NULL) {
        return -1; 
    }

    // Suppression du produit
    if (actu_moins_1 == NULL) {
        // cas ou le produit a supprimer est le premier de la liste
        *head = actu->suivant; 
    } else {
        // cas general
        actu_moins_1->suivant = actu->suivant; 
    }

    free(actu->nom);
    free(actu->description);
    free(actu->categorie);
    free(actu->note);
    free(actu); 
    return 0; 
}

Produit* creer_produit(uint32_t id, char* nom, char* description, char* categorie, int quantite, float prix_unitaire, time_t date_peremption, char* note) {
    /*
    Argument:
        id: identifiant unique du produit
        nom: nom du produit
        description: description du produit
        categorie: categorie du produit
        quantite: quantite du produit
        prix_unitaire: prix unitaire du produit
        date_peremption: date de peremption du produit
        note: note sur le produit
    But:
        creer un nouveau produit avec les informations fournies
    Retour:
        pointeur vers le nouveau produit cree, ou NULL en cas d'erreur
    */
    Produit* nouveau_produit = (Produit*)malloc(sizeof(Produit));
    if (nouveau_produit == NULL) {
        return NULL; 
    }

    nouveau_produit->nom = (char*)malloc((strlen(nom) + 1) * sizeof(char));
    if (nouveau_produit->nom == NULL) {
        free(nouveau_produit);
        return NULL;
    }
    strcpy(nouveau_produit->nom, nom);

    nouveau_produit->description = (char*)malloc((strlen(description) + 1) * sizeof(char));
    if (nouveau_produit->description == NULL) {
        free(nouveau_produit->nom);
        free(nouveau_produit);
        return NULL;
    }
    strcpy(nouveau_produit->description, description);

    nouveau_produit->categorie = (char*)malloc((strlen(categorie) + 1) * sizeof(char));
    if (nouveau_produit->categorie == NULL) {
        free(nouveau_produit->nom);
        free(nouveau_produit->description);
        free(nouveau_produit);
        return NULL;
    }
    strcpy(nouveau_produit->categorie, categorie);

    nouveau_produit->note = (char*)malloc((strlen(note) + 1) * sizeof(char));
    if (nouveau_produit->note == NULL) {
        free(nouveau_produit->nom);
        free(nouveau_produit->description);
        free(nouveau_produit->categorie);
        free(nouveau_produit);
        return NULL;
    }
    strcpy(nouveau_produit->note, note);

    nouveau_produit->id = id;
    nouveau_produit->quantite = quantite;
    nouveau_produit->prix_unitaire = prix_unitaire;
    nouveau_produit->date_peremption = date_peremption;
    nouveau_produit->suivant = NULL;

    return nouveau_produit; 
}

Produit* modfier_produit(Produit* produit, char* nom, char* description, char* categorie, int quantite, float prix_unitaire, time_t date_peremption, char* note) {
    /*
    Argument:
        produit: pointeur vers le produit a modifier
        nom: nouveau nom du produit
        description: nouvelle description du produit
        categorie: nouvelle categorie du produit
        quantite: nouvelle quantite du produit
        prix_unitaire: nouveau prix unitaire du produit
        date_peremption: nouvelle date de peremption du produit
        note: nouvelle note sur le produit
    But:
        modifier les informations du produit avec les nouvelles valeurs fournies
    Retour:
        pointeur vers le produit modifie, ou NULL en cas d'erreur
    */
    if (produit == NULL) {
        return NULL; 
    }
    free(produit->nom);
    free(produit->description);
    free(produit->categorie);
    free(produit->note);

    produit->nom = (char*)malloc((strlen(nom) + 1) * sizeof(char));
    if (produit->nom == NULL) {
        return NULL;
    }
    strcpy(produit->nom, nom);

    produit->description = (char*)malloc((strlen(description) + 1) * sizeof(char));
    if (produit->description == NULL) {
        free(produit->nom);
        return NULL;
    }
    strcpy(produit->description, description);

    produit->categorie = (char*)malloc((strlen(categorie) + 1) * sizeof(char));
    if (produit->categorie == NULL) {
        free(produit->nom);
        free(produit->description);
        return NULL;
    }
    strcpy(produit->categorie, categorie);

    produit->note = (char*)malloc((strlen(note) + 1) * sizeof(char));
    if (produit->note == NULL) {
        free(produit->nom);
        free(produit->description);
        free(produit->categorie);
        return NULL;
    }
    strcpy(produit->note, note);

    if (produit->quantite < 0) {
        printf("Quantite invalide. Elle doit etre positive.\n");
        return NULL;
    } else {
        produit->quantite = quantite;
    }
    
    if (prix_unitaire < 0) {
        printf("Prix unitaire invalide. Il doit etre positif.\n");
        return NULL;
    } else {
        produit->prix_unitaire = prix_unitaire;
    }

    if (date_peremption < 0) {
        printf("Date de peremption invalide.\n");
        return NULL;
    } else {
        produit->date_peremption = date_peremption;
    }


    return produit;
}


void admin_menu() {
    /*
    Argument:
        aucun
    But:
        afficher un menu de différentes fonctionnalités pour l'admin
    Retour:
        aucun
    */


    char input[100];
    bool continu = true;
    Produit* head = NULL; 

    while (continu) {
        printf("\n");
        printf("=== Bureau de Gestion des Ressources de Soin (BGRS) ===\n");
        printf("1. Afficher l’inventaire complet\n2. Ajouter un produit\n3. Supprimer un produit\n4. Modifier un produit\n5. Rechercher un produit\n6. Sauvegarder l’inventaire\n7. Charger un inventaire\n8. Quitter\n");
        fflush(stdout); //force l'affichage

        if (!fgets(input, 100, stdin)) {
            printf("Input invalid.\n");
            continue; //passe à l'itération suivante
        }

        int choice = atoi(input); //convertit en int
        

        printf("\n");
        switch (choice) {
            case 1: {
                // Afficher l’inventaire complet
                if (head == NULL) {
                    printf("L'inventaire est vide.\n");                    
                }
                else {   
                    printf("Inventaire :\n");   
                    printf("\n");
                    printf("-------------------------\n");      
                    affichage(head);
                    
                }
                break;
            }
            case 2: 
                // Ajouter un produit
                char* nom_temp = (char*)malloc(64 * sizeof(char));
                char* description_temp = (char*)malloc(1024* sizeof(char));
                char* categorie_temp = (char*)malloc(64* sizeof(char));
                int quantite_temp = 0;
                float prix_unitaire_temp= 0.0;
                char* note_temp= (char*)malloc(256* sizeof(char));
                time_t date_peremption_temp= 0;

                printf("Entrez le nom du produit : ");
                fgets(nom_temp, 64, stdin);
                printf("Entrez la description du produit : ");
                fgets(description_temp, 1024, stdin);
                printf("Entrez la categorie du produit : ");
                fgets(categorie_temp, 64, stdin);
                printf("Entrez la quantite du produit : ");
                fgets(input, 100, stdin);
                quantite_temp = atoi(input);
                printf("Entrez le prix unitaire du produit : ");
                fgets(input, 100, stdin);
                prix_unitaire_temp = atof(input);
                printf("Entrez la date de peremption du produit (timestamp, 0 si non applicable) : ");
                fgets(input, 100, stdin);
                // Convertir la chaine en time_t
                date_peremption_temp = (time_t)atol(input);
                printf("Entrez une note sur le produit : ");
                fgets(note_temp, 256, stdin);

                // incrementer l'id à partir du dernier id utilisé
                static uint32_t dernier_id_utilise = 0;
                dernier_id_utilise++;   
                Produit* nouveau_produit = creer_produit(dernier_id_utilise, nom_temp, description_temp, categorie_temp, quantite_temp, prix_unitaire_temp, date_peremption_temp, note_temp);
                if (nouveau_produit == NULL) {
                    printf("Erreur lors de la creation du produit.\n");
                } else {
                    insertion(&head, nouveau_produit);
                    printf("Produit ajoute avec succes.\n");
                }
                           
                break;
            case 3:
                // Supprimer un produit par ID
                printf("Entrez l'ID du produit a supprimer : ");
                fgets(input, 100, stdin);
                uint32_t id_a_supprimer = (uint32_t)atoi(input);
                if (suppression_par_id(&head, id_a_supprimer) == 0) {
                    printf("Produit supprime avec succes.\n");
                } else {
                    printf("Produit avec l'ID %u non trouve.\n", id_a_supprimer);
                }

                break;
            case 4:
                // Modifier un produit
                printf("Entrez l'ID du produit a modifier : ");
                fgets(input, 100, stdin);
                uint32_t id_a_modifier = (uint32_t)atoi(input);
                Produit* produit_a_modifier = rechercher_par_id(head, id_a_modifier);
                if (produit_a_modifier == NULL) {
                    printf("Produit avec l'ID %u non trouve.\n", id_a_modifier);
                }
                else {
                    char* nom_temp = (char*)malloc(64 * sizeof(char));
                    char* description_temp = (char*)malloc(1024* sizeof(char));
                    char* categorie_temp = (char*)malloc(64* sizeof(char));
                    int quantite_temp = 0;
                    float prix_unitaire_temp= 0.0;
                    char* note_temp= (char*)malloc(256* sizeof(char));
                    time_t date_peremption_temp= 0;

                    printf("Entrez le nouveau nom du produit : ");
                    fgets(nom_temp, 64, stdin);
                    printf("Entrez la nouvelle description du produit : ");
                    fgets(description_temp, 1024, stdin);
                    printf("Entrez la nouvelle categorie du produit : ");
                    fgets(categorie_temp, 64, stdin);
                    printf("Entrez la nouvelle quantite du produit : ");
                    fgets(input, 100, stdin);
                    quantite_temp = atoi(input);
                    printf("Entrez le nouveau prix unitaire du produit : ");
                    fgets(input, 100, stdin);
                    prix_unitaire_temp = atof(input);
                    printf("Entrez la nouvelle date de peremption du produit (timestamp, 0 si non applicable) : ");
                    fgets(input, 100, stdin);
                    // Convertir la chaine en time_t
                    date_peremption_temp = (time_t)atol(input);
                    printf("Entrez une nouvelle note sur le produit : ");
                    fgets(note_temp, 256, stdin);
                    

                    if (modfier_produit(produit_a_modifier, nom_temp, description_temp, categorie_temp, quantite_temp, prix_unitaire_temp, date_peremption_temp, note_temp) != NULL) {
                        printf("Produit modifie avec succes.\n");
                    } else {
                        printf("Erreur lors de la modification du produit.\n");
                    }
                }
                
                break;
            case 5:
                // Rechercher un produit
                printf("Entrez l'ID du produit a rechercher : ");
                fgets(input, 100, stdin);
                uint32_t id_a_rechercher = (uint32_t)atoi(input);
                Produit* produit_trouve = rechercher_par_id(head, id_a_rechercher);
                if (produit_trouve != NULL) {
                    printf("Produit trouve :\n");
                    printf("ID: %u\n", produit_trouve->id);
                    printf("Nom: %s\n", produit_trouve->nom);
                    printf("Categorie: %s\n", produit_trouve->categorie);
                    printf("Description: %s\n", produit_trouve->description);
                    printf("Prix Unitaire: %.2f\n", produit_trouve->prix_unitaire);
                    printf("Quantite: %d\n", produit_trouve->quantite);
                    printf("Date de Peremption: %s\n", ctime(&produit_trouve->date_peremption));
                } else {
                    printf("Produit avec l'ID %u non trouve.\n", id_a_rechercher);
                }
                break;
            case 6:
                // Sauvegarder l’inventaire

                break;
            case 7:
                // Charger un inventaire
                break;
            case 8:
                // Quitter
                
                continu = false;
                break; 
                
            default:
                printf("Mauvais input, veuillez choisir un nombre en 1 et 8\n");
                break;
        }
    }
}







int main() {
    // Main
    admin_menu();
    
    return 0;
}


