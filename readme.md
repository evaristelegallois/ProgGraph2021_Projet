# Programmation graphique - Projet - FARAJI Mahya & PAREL Gabriel

## Compilation et exécution

Le projet est exécutable depuis Qt Creator, à partir du fichier .pro.

## Utilisation

Un pédalier de vélo, en mouvement, est affiché sur la scène. Il est possible de modifier certains paramètres de l'animation (vitesse de l'animation, démarrage...) à l'aide des touches indiquées dans la console, ainsi qu'à l'aide de la fenêtre de paramètres accessibles depuis le bouton "Settings".

## Difficultés rencontrées

Nous ne sommes pas parvenus à trouver le problème de connexion entre les signaux/slots de la fenêtre principale et la fenêtre de paramètres, si bien que les boutons de cette dernière ne fonctionnent pas.

De plus, la trajectoire des maillons est incorrecte, car le placement des maillons n'est pas centré sur les dents des roues. La trajectoire au niveau des transitions entre la rotation autour d'une roue et la translation d'une roue à une autre peut également être améliorée.

Enfin, bien que nous ayons chargé des textures pour tenter un affichage de ces dernières, nous ne sommes pas parvenus à afficher couleurs et textures en même temps depuis le fragment shader. En effet, afficher les textures annule l'affichage des couleurs et inversement, car les deux types de données sont stockées dans vec4.
