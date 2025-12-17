#!/bin/bash

ERR_NOT_NUMBER=1
ERR_OUT_OF_RANGE=2

MIN_EXECUTIONS=1
MAX_EXECUTIONS=10

MIN_LAUNCHERS=1
MAX_LAUNCHERS=5

MAX_WORKERS=20
MAX_WORKLOAD=5

declare -a totals

Number(){
	#----------------------------------
	# NO TOCAR - PODEIS USARLA EN CheckArguments() para determinar si un parámetro  es numérico
	# Parámetros: eimo-
	#    $1 : parametro a comprobar
	# Salida:
	#    1  : si es numérico
	#    0  : si no es numérico
	#----------------------------------
	if [ "$1" -eq "$1" ] 2>/dev/null; then
		return 0
	else
		return 1
	fi
}


Sintaxis(){
	#----------------------------------
	# NO TOCAR
	#----------------------------------
	#Debe de mostrar la sintaxis de uso del programa, con la explicación de los parametros.
	echo "-------------------------------------------------------------------------------------------"
	echo "Sintaxis:"
	echo "lanzar.sh <resultsfilename> <numExecutions>"
	echo ""
	echo "El fichero resultsfilename se borrará si existe "
	echo "-------------------------------------------------------------------------------------------"
}

#***************************************************************************
# Completar las siguientes funciones según el enunciado y los comentarios.
#***************************************************************************

ShowResults(){
	#----------------------------------
	# BEGIN TODO 1
	#----------------------------------
		#Lee el fichero $1 y muestra los elmentos leidos en una fila sin retorno de carro
		#Acumula los valores en el array de totals, uno en cada índice del array.
		#Recibe como parámetro el nombre del fichero de resultados, si no existe mostrar error y terminar.
		if ! [ -f "$1" ]; then
			echo "El fichero $1 no existe"
			exit -1
		fi

		ix=0;
		while IFS= read -r line
		do
			totals[$ix]=$((totals[$ix]+$line))
			printf "%8d " $line
			ix=$(($ix+1))
		done < $1
	#----------------------------------
	# END TODO
	#----------------------------------
	# Añadimos la parte final de estadísticas y el retorno de carro.
	printf "  Workers:%2d workload:%2d time:%s\n" $numWorkers $workLoad $elapsed
}


ShowTotals(){
	#----------------------------------
	# BEGIN TODO 2
	#----------------------------------
		#Se muestran los numeros ya calculados en el array de totals
		for (( i=0; i<$numLaunchers; i++ ))
		do
			echo -n "---------" 
		done
		printf "\n"
		for (( i=0; i<$numLaunchers; i++ ))
		do
			printf "%8d " ${totals[$i]}
		done
		#printf "%8d %8d %8d\n"  ${totals[0]}  ${totals[1]}  ${totals[2]}
		printf "\n"
		echo ""
	#----------------------------------
	# END TODO
	#----------------------------------
}


CheckArguments(){
	#Para sacar Sintaxis cuando no me pasan argumentos.
	if [ $# -eq 0 ]; then
		Sintaxis
		exit 0
	fi

	#----------------------------------
	# BEGIN TODO 3
	#----------------------------------
		#Comprobamos que el número de parametros es 3
		if [ $# -ne 3 ]; then
			echo "Error número de parametros incorrecto, debe ser 3"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el paramatro $1 es un string (no es numerico)
		if Number $1 ; then
			echo "El parametro <resultsFileName> debe ser un string"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parámetro $2 es un número 
		if ! Number $2 ; then
			echo "El parámetro <numExecutions> ($2) debe ser un número"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parámetro $3 es un número 
		if ! Number $3 ; then
			echo "El parámetro <numLaunchers> ($3) debe ser un número"
			Sintaxis
			exit 1
		fi

		#Comprobamos el rango del parámetro numExecution
		if [ $2 -lt $MIN_EXECUTIONS ] || [ $2 -gt $MAX_EXECUTIONS ] ; then
			echo "El parametro <numExecutions> debe estar en el intervalo [$MIN_EXECUTIONS..$MAX_EXECUTIONS]"
			Sintaxis
			exit 1
		fi

		#Comprobamos el rango del parámetro numLaunchers
		if [ $3 -lt $MIN_LAUNCHERS ] || [ $3 -gt $MAX_LAUNCHERS ] ; then
			echo "El parametro <numLaunchers> debe estar en el intervalo [$MIN_LAUNCHERS..$MAX_LAUNCHERS]"
			Sintaxis
			exit 1
		fi

	#----------------------------------
	# END TODO
	#----------------------------------
}


InitData(){
	#----------------------------------
	# BEGIN TODO 4
	#----------------------------------
		#El array totals tendrá el acumulado de cada Launcher. Hay $1 Launchers ($1 es elparametro que recibe InitData)
		#Necesitamos inicializar el array totals $1 ceros
		for (( i=0; i<$1; i++ ))
		do
			totals[$i]=0
		done
	#----------------------------------
	# END TODO
	#----------------------------------
}


#----------------------------------------------
# MAIN
#----------------------------------------------

#----------------------------------
# BEGIN TODO MAIN1 5
#----------------------------------
	#Comprobar los argumentos llamando a CheckArguments apropiadamente
	CheckArguments $*
#----------------------------------
# END TODO
#----------------------------------

fileName=$1
numExecutions=$2
numLaunchers=$3

#----------------------------------
# BEGIN TODO MAIN2 6
#----------------------------------
	#Si el parametro $1 es un fichero que exite lo tenemos que borrar. 
	#Sacamos un mensaje por pantalla diciendo de que se ha borrado. Pero no damos error.
	if [ -f "$1" ]; then
		rm $1 > /dev/null
		echo "El fichero [$1] se ha borrado."
	fi
#----------------------------------
# END TODO
#----------------------------------

#LLamamos a la funcion InitData para inicializar el array totals
InitData $numLaunchers

#Bucle para llamar el número correcto de ejecuciones al programa procesar
for (( i=0; i<$numExecutions; i++ ))
do
	#----------------------------------
	# BEGIN TODO MAIN3 7
	#----------------------------------
		# Generamos parametros aleatorios, dentro del rango permitido, para llamar al programa procesar.
		numLaunchers=$(echo $(( $RANDOM % $MAX_LAUNCHERS + 1 )) )
		numWorkers=$(echo $(( $RANDOM % $MAX_WORKERS + 1 )) )
		workLoad=$(echo $(( $RANDOM % $MAX_WORKLOAD + 1 )) )

		#Capturamos el tiempo inicial
		start_time=$(date +%s.%3N)
			
			#LLamamos al programa procesar con los parámetros apropiados
			procesar $numLaunchers $numWorkers $workLoad $fileName
			#procesar $numWorkers $workLoad $fileName

		#Capturamos el tiempo final y calculamos el tiempo que ha pasado desde el tiempo inicial
		end_time=$(date +%s.%3N)
		elapsed=$(echo "scale=3; $end_time - $start_time" | bc)

		#Mostramos los resultados de esta ejecución llamando a ShowResults pasando el parametro apropiado
		ShowResults $fileName 

		#Borramos el fichero de resultados de esta ejecución
		# echo -------------------
		# cat $fileName
		# echo -------------------
		rm $fileName >/dev/null
	#----------------------------------
	# END TODO
	#----------------------------------
done

#Mostramos la linea de totales
ShowTotals

