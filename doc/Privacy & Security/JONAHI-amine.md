# Définition des objectifs et du champ de l'étude
Mesurer la distance entre les parois rocheuses pour permettre le monitoring de ces derniers, la distance mesurée est envoyée via le réseau LOARAWAN.

# Security Compliance

## Authorization and Authentication
Pour accéder aux données de ce système il faut passer par le serveur campus Iot, par conséquent il faut avoir un compte sur le serveur. En effet notre système en soi ne dispose pas de contrôle d'accès aux informations qu'il fournit, il se contente d'envoyer les informations sur le réseau mentionné dans le programme principal.

## Data Protection
Notre système ne collecte ni stocke en aucun cas des informations des utilisateurs, en fait il collecte les informations des capteurs, ces derniers présentent des mesures effectuées dans l'environnement où il se trouve.

## Flexibility & Remote Patching
La mise en oeuvre d'un système de mise à jour à distance n'est pas implémentée dans ce système. En revanche pour modifier la façon dont le système traite, collecte et envoie les informations, il faut se déplacer puis se connecter au système.

## Intrusion Detection
Le système ne dispose pas d'une fonctionnalité de détection d'intrusion, il doit être mis dans une boîte pour ne pas autoriser un accès physique (Câble USB) aux processeur programmable afin de ne pas fausser les mesures.a
