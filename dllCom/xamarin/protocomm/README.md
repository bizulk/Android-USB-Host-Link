# Avant compilation : 

	* Mettre à jour le fichier protocomm_wrap.c généré par SWIG (void dans xamarin/swig).

# Compilation en ligne de commande

Le fichier buildall.bat appelle la compilation pour toutes les plateformes
Il lui faut le chemin vers l'environnement de votre IDE, définir un fichier buildall_user.bat avec le contenu comme suivant :

```
set VSDEVCMD="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
```