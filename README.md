# Protocole Mégaphone

Le protocole Mégaphone est un protocole de communication de type client/serveur permettant à des utilisateurs de se connecter à un serveur pour suivre des fils de discussions, en créer de nouveaux, ou enrichir des fils existants en postant des messages (billets) ou des fichiers. Les utilisateurs peuvent également s'abonner à des fils pour recevoir des notifications.

## Fonctionnalités

Un utilisateur, une fois inscrit auprès du serveur, doit pouvoir effectuer les actions suivantes :

- **Poster un message ou un fichier sur un nouveau fil**
- **Poster un message ou un fichier sur un fil existant**
- **Demander la liste des `n` derniers billets sur chaque fil**
- **Demander les `n` derniers billets d’un fil donné**
- **S’abonner à un fil et recevoir les notifications de ce fil**

Le serveur est responsable de répondre aux demandes du client et d'envoyer des notifications.

## Description des échanges

Dans un premier temps, les échanges entre les clients et le serveur sont décrits de façon globale. Le format exact des messages du client et du serveur sont ensuite décrits précisément.

### Échanges globaux

Les échanges entre les clients et le serveur suivent un modèle simple où les clients envoient des requêtes et le serveur répond avec les informations demandées ou des confirmations.

### Formats des messages

Les messages échangés entre les clients et le serveur suivent un format défini pour garantir une communication claire et précise. Les détails sur le format des messages seront décrits plus en détail dans le fichier de [protocole](protocoles-projet.pdf).

## Installation

1. **Cloner le dépôt du projet** :
    ```bash
    git clone <URL_DU_DEPOT>
    ```
2. **Accéder au répertoire du projet** :
    ```bash
    cd megaphone-protocol
    ```
3. **Compiler le projet** :
    ```bash
    make
    ```

## Utilisation

### Lancer le serveur

Pour démarrer le serveur, exécutez la commande suivante :
```bash
./main_serveur
```
### Lancer le client

Pour démarrer le client, exécutez la commande suivante :
```bash
./main_client
```
## Licence

Ce projet est sous la [GNU General Public License v3.0 (GPL v3)](LICENSE). Pour plus de détails, veuillez consulter le fichier `LICENSE` dans ce répertoire.

## Auteurs
- **Ange KOUE-HEMAZRO**
- **Henri TRAN**
- **Mouloud AMARA**
