echo off

rem Auteur : Marie-Agnès PEREL, CIO Informatique Embarquée
rem bref : Apppeler la génération du wrapper C# de dllCom
mkdir out
del /Q /S out
swig -Wall -csharp -I..\..\libcomm -I..\..\libcomm\devices -DSWIG_FFI_BINDING -DLOG_USE_FIFO -outdir out protocomm.i
pause