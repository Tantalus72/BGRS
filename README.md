
# Bureau de Gestion des Ressources de Soin (BGRS)

**Auteur :** Erwann GIRAULT  
**Contexte :** Projet de C Sécurisé / Nation Cyboulette  
**Date :** Décembre 2025

## Description

Le BGRS est une application en ligne de commande (CLI) conçue pour gérer l'inventaire médical stratégique de la nation Cyboulette face à la menace des Uiteurdizuiteur.

Contexte issue du sujet : 
Le peuple des Cyboulettes est en panique ! Les terribles Uiteurdizuiteur sont passés à l’at-
taque et menacent de dominer la nation Cyboulette nuit et jour et pour toute éternité !
Face à cet effroyable danger, les Cyboulettes s’organisent. Mais la défense du pays est
périlleuse et les Cyboulettes finissent tailladés de toutes parts. Leur seule solution est alors
d’utiliser leur matériel médical, qui va de la potion de soin ultime (1èrecatégorie) au pansement
Ecoprix (marque déposée) selon la gravité du problème (et selon les ressources disponibles). Afin
d’organiser leur effort, les Cyboulettes ont mis en place le Bureau de Gestion des Ressources
de Soin (le BGRS).
Et c’est là que vous entrez en jeu. Le BGRS a besoin d’une application ultra solide pour
gérer les ressources et c’est à vous de la réaliser. Cependant, il arrive que des Uiteurdizuiteur
réussissent à s’infiltrer pour tenter de faire des entrées invalides sur le système. Il se peut aussi
que les opérateurs Cyboulettes, par la faute du stress, commettent des erreurs. Il faut donc que
l’application réussisse à gérer tous ces problèmes.
Une dernière précision : la nation n’ayant pas trop de moyens, il a été décidé que les produits
destinés à la réparation des outils utilisés par les Cyboulettes était, en fait, du matériel médical.
Le BGRS se retrouve donc à gérer des produits à destination des Cyboulettes et du matériel
visant à réparer des outils. Attention à ne rien mélanger !

L'application permet de gérer des produits variés (Potions, Pansements, Outils, etc.) avec une persistance des données et une journalisation des actions.

## Instructions de Compilation et de Lancement

Le projet est fourni avec un `Makefile` pour automatiser la compilation.

### Pré-requis

  * GCC (avec support C11)
  * Make
  * Valgrind (optionnel mais utile pour les tests)

### Compilation

Pour compiler le projet et générer l'exécutable `bgrs` :

```bash
make
```

Pour nettoyer les fichiers objets (`.o`) et l'exécutable :

```bash
make clean
```

### Lancement

Pour lancer l'application :

```bash
./bgrs
```

Pour lancer l'application avec Valgrind :

```bash
valgrind --leak-check=full --track-origins=yes ./bgrs
```

## Fonctionnalités Implémentées

Le programme propose un menu interactif permettant les actions suivantes :

1.  **Afficher l'inventaire :** Liste tous les produits avec leurs détails (ID, Nom, Quantité, Prix, etc.).
2.  **Ajouter un produit :** Création dynamique d'un produit. Les champs (Nom, Description, Catégorie) sont alloués dynamiquement.
3.  **Supprimer un produit :** Suppression par ID avec libération de la mémoire.
4.  **Modifier un produit :** Modification des champs d'un produit existant (valeurs par défaut conservées si entrée vide).
5.  **Rechercher un produit :** Recherche par nom avec gestion de la casse (ex: "potion" trouve "Potion de Soin").
6.  **Sauvegarder l'inventaire :** Exportation des données dans le fichier `inventaire_sauvegarde.txt` (format de tableau avec séparateur `|`).
7.  **Charger un inventaire :** Importation depuis le fichier de sauvegarde 
8.  **Charger le "Loot" de départ :** Génération automatique d'un set d'objets de test (Potion de Soin Ultime, Duct tape, etc.).

**Fonctionnalité Automatique :**

  * **Nettoyage des périmés :** À chaque cycle du menu, l'application vérifie et supprime automatiquement les produits dont la date de péremption (Timestamp) est dépassée.
  * **Journalisation :** Les ajouts, suppressions et modifications sont enregistrés dans `historique.log`.

## Structure du Code

Le projet est modulaire :

  * **`main.c`** : Point d'entrée. Gère la boucle principale, le menu et l'orchestration des modules.
  * **`gestion_produit.c`** : Logique de la structure `Produit` et les fonctions vitales et la journalisation.
  * **`gestion_db.c`** : Persistance. Gère la lecture et l'écriture du fichier CSV `inventaire_sauvegarde.txt`. 
  * **`utils.c`** : Fonctions utilitaire pour sécurisé les entrées
  * **`test_auto.py`** : Fichier python permettant de compiler via `make`, tester les fuites de mémoire avec `valgrind`, les cas limites (input invalide par exemple) et la persistance des données. 
  

## Pistes d'Amélioration

Le projet pourrait être amélioré sur les idées suivantes :

* **Sauvegarde :** Actuellement, la sauvegarde écrase directement le fichier. Il serait mieux d'écrire dans un fichier temporaire (`.tmp`) puis le renommer une fois l'écriture validée. Cela éviterait la corruption de la base de données en cas de crash pendant l'écriture.

* **ID Unique Centralisé :** L'ID est actuellement géré par un compteur incrémental dans le `main`. Une gestion centralisée ou persistante des ID (stockée dans le fichier de sauvegarde par exemple) est nécessaire pour garantir l'unicité des ID après plusieurs redémarrages/chargements.
* **Chiffrement des Données :** Les données de sauvegarde sont actuellement stockées en clair. L'implémentation d'un chiffrement similaire à celui utilisé dans le projet du Serveur de Vote permettrait de protéger l'intégrité et la confidentialité de l'inventaire face aux Uiteurdizuiteur. Ce projet semble avoir pour objectif d'être lu et testé par un camarade, je n'ai pas implémenté cette fonctionnalité afin de faciliter la lecture, le test et le débogage (croyez moi, vous avez pas envie de debugger de l'AES 256 ou n'importe quel chiffrement :D )
* **Architecture Client-Serveur :** L'application est actuellement en CLI locale. Une évolution consisterait à séparer la logique en un Serveur (gérant la liste chaînée et la persistance) et des Clients (envoyant des commandes via sockets) permettant une utilisation par plusieurs opérateurs Cyboulettes et une gestion des droits, du stock etc.