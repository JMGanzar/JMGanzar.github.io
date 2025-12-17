#!/bin/bash

sintaxis(){
echo "-----------------------------"
echo "propiedades <opciones> <fichero>"
echo 
echo "Opciones:"
echo "   -help"
echo "   Si se proporciona la opción no se puede pasar el parámetro"
echo 
echo "Busca un fichero por el disco duro, si lo encuentra"
echo "detalla las propiedades del mismo e indica si el usuario"
echo "tiene permisos de ejecución"
echo "Nota: si existe el fichero en más de una ruta se muestran "
echo "      todas pero se proporcina los atributos de la primera ocurrencia"
echo "-----------------------------"
exit
}

#Comprobamos que pasan un fichero
if [ $# -lt 1 ]; then
	echo "Falta el fichero a buscar"
  sintaxis
elif [ $# -gt 1 ]; then
  echo "Sólo se permite un parámetro"
  sintaxis 
fi

if [ "$1" == "-help" ]; then
   sintaxis
fi

# Buscamos el fichero con el comando locate, si lo encontramos procedemos
find / -name $1 >rutas 2>/dev/null
encontradas=`wc -l rutas | cut -d" " -f1`
if [ $encontradas -gt 0 ]; then 
   ruta=$(head -n1 rutas)
   echo "-----------------------------------------------------"
   echo "El fichero se ha encontrado en las siguientes rutas: "
   cat rutas
   echo "-----------------------------------------------------"
   if [ -x $ruta ]; then
		echo "El fichero ES ejecutable"
	else	
		echo "El fichero NO ES ejecutable"
   fi
   echo "   con los siguientes atributos"
   ls -l $ruta
   echo      
else
  echo "No se encuentra el fichero."
fi
