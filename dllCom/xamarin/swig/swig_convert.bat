echo off

rem Auteur : Marie-Agnès PEREL, CIO Informatique Embarquée
rem bref : Apppeler la génération du wrapper C# de dllCom
mkdir out
del /Q /S out
swig -DSWIG_FFI_BINDING -Wall -csharp -I..\..\libcomm -outdir out protocomm.i
pause