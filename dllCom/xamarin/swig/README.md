** Auteur : Marie-Agnès PEREL, CIO Informatique Embarqués
** Date : 2020/05/15
** Bref : Présenter le développement du wrapper Xamarin pour la DLL dllCom.

On utilise SWIG pour exporter la lib dans C#
SWIG doit être installé (chocolatey installe la version 3.0x).
Les fichiers générés ont été ajoutés aux projets :
	- dans la lib, le fichier C wrapper protocomm_wrap.c
	- Dans l'IHM, les fichiers C# regroupé dans out.
Dans le premier cas VS crée un lien vers le fichier, dans le second les fichiers sont copiés.
Pour chacun des projets un répertoire/'filtre' est crée pour clairement identifier ces fichiers générés et les séparer du code.

A chaque modification de la DLL il faut : 
	- vérifier que le fichier d'interface SWIG protocomm_master.h est toujours compatible et à jour.
	- Inclure les fichiers additionnel générés 
	- copier les fichiers dans les projets
	- copier la dll à jour dans dllcom.

Elément d'entrée pour la génération :
	- Le fichier d'interface SWIG.

commande à appeler : swig -Wall -csharp -I..\..\libcomm -outdir out protocomm.i
Préférer l'utilisation du batch qui fait tout le nécessaire.
