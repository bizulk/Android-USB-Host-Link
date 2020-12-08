/*	CIO Informatique Embarquée, Selso LIBERADO & Marie-Agnes PEREL
	Ajout d'un générateur de code renvoyant le sizeof d'un type particulier.
	API C# Sizeof déclarée obsolète et pas de gestion en natif dans SWIG 3.0

*/
%define %_sizeof(TYPE)
%inline %{
int sizeof_##TYPE(void) {
   return sizeof(TYPE);
}
%}
%enddef