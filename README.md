# Le protocole Mégaphone

## Description 
Un protocole de communication de type client/serveur. Des utilisateurs se connectent à un serveur avec un client et peuvent ensuite suivre des fils de discussions, en créer de nouveaux ou enrichir des fils existants en postant un nouveau message (billet), s’abonner à un fil pour recevoir des notifications.

## Compilation et lancement

- se déplacer dans le repertoire megaphone `cd megaphone`
- `make` lancera la compilation pour produire les fichiers `.o`
  et les deux fichiers exécutables `main_client` et `main_serveur`.
- `./main_serveur <num_port>` pour lancer le serveur. 
- `./main_client <adresse> <num_port>` pour se connecter au serveur qui tourne sur l'adresse `adresse` et le numéro de port `num_port`
- les fichiers qui sont envoyés par le client sont stockés dans le répertoire `serveur/fichiers`.
- les fichiers qui sont téléchargés par le client sont stockés dans le répertoire `client/fichiers`.
- `make clean` pour supprimer les fichiers `.o` , `main_client`, `main_serveur` ainsi que les fichiers qui ont été envoyés ou téléchargés.