/*
Nom du fichier : main.c
Fait par : Erwann GIRAULT
But : Point d'entrée de l'application BGRS. Gère la boucle principale,
      le menu CLI et la gestion entre les modules.
*/



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include "gestion_produit.h"
#include "gestion_db.h"
#include "utils.h" 

static void afficher(Produit** head);
static void ajouter(Produit** head, uint32_t* max_id);
static void supprimer(Produit** head);
static void modifier(Produit* head);
static void rechercher(Produit* head);
static void sauvegarder(Produit* head);
static void charger(Produit** head, uint32_t* max_id);
static uint32_t recalculer_max_id(Produit* head);
static void generer_loot(Produit** head, uint32_t* max_id);
static void supprimer_perimes(Produit** head);


int main() {
    /*
    But :
        Fonction principale du programme
        Initialise les structures
        lance le menu et gère la fermeture 
    Arguments :
        Aucun
    Retour :
        0 si le programme s'est terminé correctement.
    */
    Produit* head = NULL;
    uint32_t max_id = 0;
    bool running = true;
    long choix;

    while (running) {
        printf("\n=== Bureau de Gestion des Ressources de Soin (BGRS) ===\n");
        printf("1. Afficher l'inventaire\n");
        printf("2. Ajouter un produit\n");
        printf("3. Supprimer un produit\n");
        printf("4. Modifier un produit\n");
        printf("5. Rechercher un produit (par Nom)\n");
        printf("6. Sauvegarder l'inventaire\n");
        printf("7. Charger un inventaire\n");
        printf("8. Charger le loot de depart\n");
        printf("9. Quitter\n");
        printf("-------------------------------------------------------\n");
        printf("Votre choix : ");

        if (!lire_long_securise(&choix)) {
            printf("Erreur : En-trée invalide. Veuillez entrer un chiffre.\n");
            continue;
        }
        supprimer_perimes(&head);

        switch (choix) {
            case 1: afficher(&head); break;
            case 2: ajouter(&head, &max_id); break;
            case 3: supprimer(&head); break;
            case 4: modifier(head); break;
            case 5: rechercher(head); break;
            case 6: sauvegarder(head); break;
            case 7: charger(&head, &max_id); break;
            case 8: generer_loot(&head, &max_id); break;
            case 9:
                printf("Fermeture du BGRS...\n");
                free_struct_produit(head); 
                running = false;
                break;
            default:
                printf("Option inconnue. Veuillez choisir entre 1 et 8.\n");
        }
    }
    return 0;
}


static void afficher(Produit** head) {
    /*
    Argument:
        head: pointeur d'un pointeur vers la tête de liste
    But:
        appeler la fonction d'affichage de la bibliothèque et gérer le cas vide
    Retour:
        Aucun
    */
    if (*head == NULL) {
        printf("Inventaire vide.\n");
    } else {
        printf("\n--- Inventaire Complet ---\n");
        affichage(head);
    }
}

static void ajouter(Produit** head, uint32_t* max_id) {
    /*
    Argument:
        head:  pointeur d'un pointeur vers la tête de liste
        max_id: Pointeur vers le compteur d'ID global
    But:
        Gérer l'interface utilisateur pour la saisie sécurisée des attributs d'un nouveau produit
        Crée le produit et l'insère dans la liste
    Retour:
        Aucun
    */
    char nom[64];
    char desc[1024];
    char cat[64];
    char note[256];
    long qte_long;
    double prix_double;
    long date_long;

    printf("\n--- Nouveau Produit ---\n");

    // Saisie Nom
    do {
        printf("Nom (max 63 chars) : ");
        if (!lire_chaine_securisee(nom, 64)) printf("Entrée invalide.\n");
    } while (strlen(nom) == 0);

    // Saisie Description
    do {
        printf("Description (max 1023 chars) : ");
        if (!lire_chaine_securisee(desc, 1024)) printf("Entrée invalide.\n");
    } while (strlen(desc) == 0);

    // Saisie Catégorie
    do {
        printf("Categorie : ");
        if (!lire_chaine_securisee(cat, 64)) printf("Entrée invalide.\n");
    } while (strlen(cat) == 0);

    // Saisie Quantité 
    do {
        printf("Quantite : ");
    } while (!lire_long_securise(&qte_long) || qte_long < 0);

    // Saisie Prix 
    do {
        printf("Prix unitaire : ");
    } while (!lire_double_securise(&prix_double) || prix_double < 0);

    // Saisie Date
    printf("Date peremption (Timestamp, 0 si aucune) : ");
    if (!lire_long_securise(&date_long) || date_long < 0) {
        date_long = 0; 
    }

    // Saisie Note
    printf("Note (facultatif) : ");
    lire_chaine_securisee(note, 256);

    (*max_id)++;

    // Création
    Produit* nouveau = creer_produit(*max_id, nom, desc, cat, qte_long, prix_double, date_long, note);
    
    if (nouveau == NULL) {
        printf("Erreur critique : Échec allocation mémoire.\n");
        (*max_id)--; 
    } else {
        insertion(head, nouveau);
        printf("[+] Produit ajoute avec l'ID %u.\n", *max_id);
    }
}

static void supprimer(Produit** head) {
    /*
    Argument:
        head:  pointeur d'un pointeur vers la tête de liste
    But:
        Demander un ID à l'utilisateur et déclencher la suppression
    Retour:
        Aucun
    */
    long id_suppr;
    printf("ID du produit a supprimer : ");
    if (lire_long_securise(&id_suppr)) {
        if (suppression_par_id(head, id_suppr) == 0) {
            printf("[-] Produit supprime.\n");
        } else {
            printf("ID introuvable.\n");
        }
    }
}

static void modifier(Produit* head) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
    But:
        Permettre à l'utilisateur de modifier les champs d'un produit existant
        Gère la conservation des anciennes valeurs si l'utilisateur appuie sur Entrée
    Retour:
        Aucun
    */
    long id_modif;
    printf("ID du produit a modifier : ");
    if (!lire_long_securise(&id_modif)) return;

    Produit* p = rechercher_par_id(head, (uint32_t)id_modif);
    if (p == NULL) {
        printf("Produit introuvable.\n");
        return;
    }

    printf("[~] Modification de '%s' (Appuyez sur Entree pour conserver la valeur actuelle)\n", p->nom);
    
    char nom[64], desc[1024], cat[64], note[256];
    long qte;
    double prix;
    long date;

    printf("Nouveau Nom [%s] : ", p->nom);
    if (!lire_chaine_securisee(nom, 64) || strlen(nom) == 0) snprintf(nom, 64, "%s", p->nom);
    printf("Nouvelle Description [%s] : ", p->description);
    if (!lire_chaine_securisee(desc, 1024) || strlen(desc) == 0) snprintf(desc, 1024, "%s", p->description);
    printf("Nouvelle Catégorie [%s] : ", p->categorie);
    if (!lire_chaine_securisee(cat, 64) || strlen(cat) == 0) snprintf(cat, 64, "%s", p->categorie);
    printf("Nouvelle Quantité [%d] : ", p->quantite);
    if (!lire_long_securise(&qte)) qte = p->quantite;

    printf("Nouveau Prix [%.2f] : ", p->prix_unitaire);
    if (!lire_double_securise(&prix)) prix = p->prix_unitaire;
    
    printf("Nouvelle Date (Timestamp) [%ld] : ", p->date_peremption);
    if (!lire_long_securise(&date)) date = p->date_peremption;

    printf("Nouvelle Note [%s] : ", p->note);
    if (!lire_chaine_securisee(note, 256) || strlen(note) == 0) snprintf(note, 256, "%s", p->note);


    if (modifier_produit(p, nom, desc, cat, qte, prix, date, note) != NULL) {
        printf("[~] Modification reussie.\n");
    } else {
        printf("Erreur modification.\n");
    }
}

static void rechercher(Produit* head) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
    But:
        Rechercher et afficher les produits (insensible à la casse)
    Retour:
        Aucun
    */
    char recherche[64];
    printf("Entrez le nom à rechercher : ");
    if (!lire_chaine_securisee(recherche, 64)) return;

    char recherche_lower[64];
    for (int i = 0; i < 64; i++) {
        recherche_lower[i] = tolower((unsigned char)recherche[i]);
        if (recherche[i] == '\0') break;
    }
    recherche_lower[63] = '\0'; 

    bool trouve = false;
    Produit* actu = head;
    printf("\n--- Resultats de recherche ---\n");
    
    while (actu != NULL) {
        char nom_lower[64]; 
        for (int i = 0; i < 64; i++) {
            nom_lower[i] = tolower((unsigned char)actu->nom[i]);
            if (actu->nom[i] == '\0') break;
        }
        nom_lower[63] = '\0';

        // Comparaison sur les versions normalisées
        if (strstr(nom_lower, recherche_lower) != NULL) { 
            printf("[%u] %s (Qte: %d) - %s\n", actu->id, actu->nom, actu->quantite, actu->categorie);
            trouve = true;
        }
        actu = actu->suivant;
    }

    if (!trouve) printf("Aucun produit contenant '%s' trouve.\n", recherche);
}
static void sauvegarder(Produit* head) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
    But:
        Déclencher la sauvegarde de l'inventaire dans le fichier par défaut
    Retour:
        Aucun
    */
    char fichier[64] = "inventaire_sauvegarde.txt";
    sauvegarde(head, fichier);
    printf("Sauvegarde terminee dans %s\n", fichier);
}

static void charger(Produit** head, uint32_t* max_id) {
    /*
    Argument:
        head:  pointeur d'un pointeur vers la tête de liste
        max_id: Pointeur vers le compteur global d'ID
    But:
        Nettoyer l'inventaire actuel et charger les données depuis le fichier
        Recalcule le max_id 
    Retour:
        Aucun.
    */
    char fichier[64] = "inventaire_sauvegarde.txt";
    
    if (*head != NULL) {
        printf("Nettoyage de l'inventaire actuel...\n");
        free_struct_produit(*head);
        *head = NULL;
    }

    charger_fichier(head, fichier);
    *max_id = recalculer_max_id(*head);
    printf("Chargement termine. Prochain ID : %u\n", *max_id + 1);
}

static uint32_t recalculer_max_id(Produit* head) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
    But:
        Parcourir la liste pour trouver l'ID le plus grand utilisé
        Nécessaire après un chargement pour reprendre la numérotation correctement
    Retour:
        L'ID maximum trouvé (uint32_t)
    */
    uint32_t max = 0;
    Produit* actu = head;
    while (actu != NULL) {
        if (actu->id > max) {
            max = actu->id;
        }
        actu = actu->suivant;
    }
    return max;
}

static void generer_loot(Produit** head, uint32_t* max_id) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
        max_id: Pointeur vers le compteur global d'ID
    But:
        Génère automatiquement les objets du sujet pour faciliter les tests
    Retour:
        Aucun
    */
    printf("\n--- Generation du Loot de Depart ---\n");

    struct {
        char *nom; char *cat; char *desc; int qte; float prix; char *note;
    } items[] = {
        {"Potion de Soin Ultime", "Potion", "Restaure 100% PV. Petillante.", 3, 300.0, "Mythique, souvent falsifie."},
        {"Duct tape", "Consommable", "Repare equipement 50%. Colle aux doigts.", 20, 10.0, ""},
        {"Pansement Ecoprix", "Pansement", "Reduit degats superficiels.", 342, 1.0, "1% chance empoisonne."},
        {"Serum de Vitalite Beta", "Medicament", "Reactif experimental.", 5, 25.0, ""},
        {"WD-40", "Consommable", "Restaure 100% equipement.", 20, 10.0, "Efficace en toute situation."},
        {"Scalpel Photonique", "Outil", "Instrument precision solaire.", 2, 120.0, ""},
        {"Gel Hydroalcoolique", "Consommable", "Elimine 99.99% virus.", 20, 5.0, "Brule les tentacules."}
    };

    int nb_items = 7;
    for (int i = 0; i < nb_items; i++) {
        (*max_id)++;
        Produit* p = creer_produit(*max_id, items[i].nom, items[i].desc, items[i].cat, items[i].qte, items[i].prix, 0, items[i].note);
        
        if (p != NULL) {
            insertion(head, p);
            printf("[+] Ajout auto : %s\n", items[i].nom);
        } else {
            printf("[!] Erreur ajout : %s\n", items[i].nom);
            (*max_id)--;
        }
    }
    printf("Loot genere avec succes !\n");
}


static void supprimer_perimes(Produit** head) {
    /*
    Argument:
        head: Pointeur vers la tête de liste
    But:
        Parcours automatique et suppression des produits périmés (basé sur Timestamp)
    */
    bool restart = true;
    int count = 0;
    time_t now = time(NULL); 

    while (restart) {
        restart = false;
        Produit* actu = *head;

        while (actu != NULL) {
            // Si date existe (>0) ET date passée (< now)
            if (actu->date_peremption != 0 && actu->date_peremption < now) {
                
                printf("[-] Suppression du produit perime ID %u (%s)\n", actu->id, actu->nom);
                
                suppression_par_id(head, actu->id);
                
                count++;
                restart = true; 
                break; 
            }
            actu = actu->suivant;
        }
    }
    if (count > 0) {
        printf("[INFO] %d produits perimes ont ete retires de l'inventaire.\n", count);
    }
}