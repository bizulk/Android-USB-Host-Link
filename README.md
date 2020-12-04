# AndroidFilaire

please consult the power point english presentation.

Regroupe les projets sur cible, PC et tablette, et la documentation (retour d'expérience, compte-rendu de projet).

1. TestUSBTerminal : Projet sur cible STM32 Nucléo, embarquant plusieurs projets : test de communication USB, le protocole dllCom, ...
2. IHMTablette : IHM de test tablette de communication sur USB, embarquant le protocole dllCom.
3. dllCom : le protocole de communication 


# dllCom 

Custom procotol implementation for slave/master R/W
This is a simplement protocol for demonstration purpose.

# doc 

All documentation : tech & Project report, analyses.

# TestUSBTerminal 

Le projet est développé avec STMCubeMX et le firmware STMCube, pour une Nucléo à base de STM429ZI.
Sont embarqués dans le projet : 
- un test de console 
- l'implémentation "slave" du protocole dllCom.
Une interface est implémentée, et on sélectionne le projet à compiler avec des varaibles de compilation.

This is the device project implemented with STMCubeMX, STMCube, for a STM Nucleo F439ZI.
The project is design to embbed and select several app : 
- console test
- "slave" implementation of the dllCom protocol for interfacing with the tablet.

# MobileApplication

This is the mobile Android application developped with Xamarin.
The app embbed the wrapped dllCom library.

For test and debug purpose, a tablet with two USB port is recommended.

# dllCom

All the dllCom related project : 
- implementation
- unit tests 
- bindings.


