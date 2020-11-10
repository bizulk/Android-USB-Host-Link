# AndroidFilaire

Le but du projet est de mettre en oeuvre une solution de communication entre une Interface développée avec Xamarin pour Android, et une cible ARM implémentant le Virtual COM Port sur un port USB.

Regroupe les projets sur cible, PC et tablette... 

1. TestUSBTerminal : Projet sur cible STM32 Nucléo, embarquant plusieurs projets : test de communication USB, le protocole dllCom, ...
2. IHMTablette : IHM de test tablette de communication sur USB, embarquant le protocole dllCom.
3. dllCom : le protocole de communication 



# TestUSBTerminal 

Le projet est développé avec STMCubeMX et le firmware STMCube, pour une Nucléo à base de STM429ZI.
Sont embarqués dans le projet : 
- un test de console 
- l'implémentation "slave" du protocole dllCom.
Une interface est implémentée, et on sélectionne le projet à compiler avec des varaibles de compilation.

# IHMTablette

L'IHM met en oeuvre le protocole, elle cible la lecture écriture sur un device de deux registres.
La dll est compilée pour Linux/Android et un wrapper est généré avec Swig

Pour le test et débogage, une tablette avec deux ports USB est recommandée.
Aussi on embarque le device "emulslave" pour de l'auto-test.

# dllCom

Le protocole de communication pour la lecture/écriture de registre.
Codée sur deux couches : 
La couche bas niveau qui envoie/reçoit les trames, avec un découplage sur l'interface de gestion des E/S.
La couche haut niveau qui implémente les échanges nécessaire pour l'abstraction : on écrit ou lit un registre.


**Pour tout travaux il est recommandé d'utiliser des branches afin de gérer proprement leur intégration.**
