#!/bin/bash

log2() {
	#----------------------------------
	# NO TOCAR
	#----------------------------------
	#Devuelve el log2 del numero recibido como parametro
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

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
	echo "lanzar.sh <numPlayers> <bestOf>"
	echo "   numPlayers >= 16 and numPlayers <= 128 and numPllayers should be multiple of 16.  Accepted values: 16, 32, 64, 128"
	echo "   bestOf >=3 and bestOf<= 9 and bestOf should be odd. Accepted values:  3, 5, 7, 9 "
	echo "-------------------------------------------------------------------------------------------"
}

#***************************************************************************
# Completar las siguientes funciones según el enunciado y los comentarios.
#***************************************************************************

CheckArguments(){
	#Para sacar Sintaxis cuando no me pasan argumentos.
	if [ $# -eq 0 ]; then
		Sintaxis
		exit 1
	fi

	#----------------------------------
	# NOTA: Podeis implementarlo siguiendo las siguientes comprobaciones o hacerlo de cualquier otra fora que cumpla lo pedido.
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
		#Comprobamos que el número de parametros es 2
		if [ $# -ne 2 ]; then
			echo "Error número de parametros incorrecto ($#), debe ser 2"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el paramatro $1 es un es numerico
		if ! Number $1 ; then
			echo "El parametro <numPlayers> debe ser numerico"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parámetro $2 es un número 
		if ! Number $2 ; then
			echo "El parámetro <bestOf> ($2) debe ser un número"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parámetro $1 es mayor o igual que 16
		if ! [ $1 -ge 16 ]; then
			echo "El parametro <numPlayers> debe ser mayor o igual que 16"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parámetro $1 es menor o igual que 128
		if ! [ $1 -le 128 ]; then
			echo "El parametro <numPlayers> debe ser menor o igual que 128"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parametro $1 es multiplo de 16
		if ! [ $(($1%16)) -eq 0 ]
		then
			echo "El parametro <numPlayers> debe ser multiplo de 16
			Sintaxis"
			exit 1
		fi

		#Comprobamos que el parámetro $2 es mayor o igual que 3
		if ! [ $2 -ge 3 ]; then
			echo "El parametro <bestOf> debe ser mayor o igual que 3"
			Sintaxis
		fi

		#Comprobamos que el parámetro $2 es menor o igual que 9
		if ! [ $2 -le 9 ]; then
			echo "El parametro <bestOf> debe ser menor o igual que 9"
			Sintaxis
			exit 1
		fi

		#Comprobamos que el parametro $2 es impar
		if [ $(($2%2)) -eq 0 ]
		then
			echo "El parametro <bestOf> debe ser impar
			Sintaxis"
			exit 1
		fi


	#----------------------------------
	# END TODO
	#----------------------------------
}


InitData(){
	#Recibe como primer parametro el numero de jugadores
	#Primero crea el array players con el numero de jugadores y despues se ordena aleatoriamente
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
		#Crear el array players inicializando cada elemento con el numero de jugador.
		for (( i=0; i<$1; i++ ))
		do
			players[$i]=$i
		done
	#----------------------------------
	# END TODO
	#----------------------------------


	#----------------------------------
	# BEGIN TODO
	#----------------------------------
		#Ordenar aleatoriamente el array de players
        players=( $(shuf -e "${players[@]}") )
	#----------------------------------
	# END TODO
	#----------------------------------

}


ShowPlayers(){
	echo -n "Players : "
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
	#Mostramos en una linea los jugadores que estan en el array players
	for (( i=0; i<$1; i++ )); do
		echo -n "${players[$i]} "
	done
	echo 
	#----------------------------------
	# END TODO
	#----------------------------------
}


Winner(){
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
	# Leer el fichero recibido como parametro y devolver (con echo) su última fila, que indica el ganador
	while read line; do 
		:
	done < $1

	echo $line
	#----------------------------------
	# END TODO
	#----------------------------------
}



ShowMatch(){
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
	# Recibe como parametro el fichero de una partida
	# Debe mostrar por pantalla solo las lineas donde se juega, es decir, ni la primera ni la última
	numLines=$(($bestOf+2));
	file=$1
	numLine=0
	while read line; do
		numLine=$(($numLine+1))

		#Salimos del bucle si es la ultima linea del fichero, la que indica el ganador
		if [ $numLine -eq $numLines ]; then
			break;
		fi

		#Si no es la primera linea la mostramos por pantalla
		if [ $numLine -gt 1 ]; then
			echo $line
		fi

	done < $file
	#----------------------------------
	# END TODO
	#----------------------------------
}

RemoveMatchFiles(){
	#----------------------------------
	# BEGIN TODO
	#----------------------------------
	# Borrar todos los ficheros con patron *vs*.dat del directorio actual
	for file in ./*vs*.dat; do
		rm $file 2>/dev/null
	done
	#----------------------------------
	# END TODO
	#----------------------------------
}


#----------------------------------------------
# MAIN
#----------------------------------------------

#----------------------------------
# BEGIN TODO MAIN 1
#----------------------------------
	#Comprobar los argumentos llamando a CheckArguments apropiadamente
	CheckArguments $*

	numPlayers=$1
	bestOf=$2
	round=1  #Primera ronda del torneo. Contador de rondas.

	#Inicializar el array de jugadores llamando a InitData
	InitData $numPlayers

	#Mostramos los jugadores que inician el torneo
	echo "Players in random order are: "
	ShowPlayers $numPlayers

	#Borramos los ficheros ya procesados
	RemoveMatchFiles
#----------------------------------
# END TODO
#----------------------------------

numRounds=$(log2 $numPlayers)  #numero de rondas que tendra el torneo en funcion de los jugadores



#De 2 en 2 cogidos del array players, lanzamos las partidas para los jugadores en esas posiciones
#Mientras la ronda actual $round sea menor que $numRounds estamos en rondas intermedias.
#Al salir del bucle estaremos en la ronda final.
#----------------------------------
# BEGIN TODO MAIN 
#----------------------------------
# Mientras estemos en rondas intermedias
while [ $round -lt $numRounds ]; do 

	echo "Startring round $round ---------------------------" 

	#Lanzamos todas las partidas en bucle, tomando de 2 en 2 los jugadores del array players
	#En cada iteracion llamamos al ejecutable rps con sus parametros adecuados.
	for i in `seq 0 2 $(($numPlayers -2))`; do 
		playerA=${players[$i]}
		playerB=${players[$(($i+1))]}
		rps $playerA $playerB $bestOf
	done


	# Guardamos en el array players, empezando por el principio los ganadores de cada partida.
	# Es decir, reutilizamos el array players para poner los ganadores.
	# Para saber el ganador de cada partida tenemos que leerlo del fichero de la partida.
	ixWinner=0;
	for file in ./*vs*.dat; do
		winner=$(Winner $file)
		echo "$file, winner is player $winner"
		players[ixWinner]=$winner
		ixWinner=$((ixWinner+1))
	done

	#Borramos los ficheros ya procesados
	RemoveMatchFiles

	#Dividimos la variable numPlayes por dos, pues los ganadores de cada ronda son la mitad 
	numPlayers=$(( $numPlayers / 2 ))

	#Mostramos los jugadores de la siguiente ronda
	echo "Number of Players for next round : $numPlayers"
	ShowPlayers $numPlayers
	echo "--------------------------------------------------"

	#Incrementamos en contador de rondas
	round=$(($round+1))
done
#----------------------------------
# END TODO
#----------------------------------

#Estamos en la ronda final, los jugadores implicados son los dos primeros del array de players
playerA=${players[0]}
playerB=${players[1]}
matchFile="${playerA}vs${playerB}.dat"

echo "Launching Final Match for player $playerA vs player $playerB"
rps $playerA $playerB $bestOf

#Mostramos los resultados de la final
ShowMatch $matchFile $playerA $playerB

#Mostramos el ganador
winner=$(Winner $matchFile)
echo "The winner is player $winner"

