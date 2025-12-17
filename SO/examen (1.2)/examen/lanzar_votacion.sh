#!/bin/bash

Number(){
    #----------------------------------
    # NO TOCAR - PODÉIS USARLA EN CheckArguments() para determinar si un parámetro es numérico
    # Parámetros:
    #    $1 : parámetro a comprobar
    # Salida:
    #    0  : si es numérico
    #    1  : si no es numérico
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
    # Muestra la sintaxis de uso del programa, con la explicación de los parámetros.
    echo "----------------------------------------------------------------------------------------------"
    echo "Sintaxis:                                                                                     "
    echo " ./lanzar_votacion.sh <archivoComunidades> <archivoVotantes> <partidosPoliticos>              "
    echo "   archivoComunidades: ruta a fichero que contiene en cada linea el nombre de la comunidad    "
    echo "                        autónoma y su número de habitantes totales, separado por comas.       "
    echo "   archivoVotantes:    ruta a fichero que donde se guardarán en cada línea el nombre de la    "
    echo "                        comunidad autónoma y el censo electoral (número de votantes totales). "
    echo "   archivoResultados:  ruta a fichero que donde se recogerá el resultado de las votaciones    "
    echo "                        generado por el programa <votar> de C.                                "
    echo "   partidosPoliticos:  debe ser un número entre 2 y 6 (ambos incluidos).                      "
    echo "----------------------------------------------------------------------------------------------"
}

#***************************************************************************
# Completar las siguientes funciones según el enunciado y los comentarios.
#***************************************************************************

CheckArguments(){
    # Muestra la sintaxis cuando no se pasan argumentos.
    if [ $# -eq 0 ]; then
        Sintaxis
        exit 1
    fi

    #----------------------------------
    # NOTA: Podeis implementarlo siguiendo las siguientes comprobaciones o 
    # hacerlo de cualquier otra forma que cumpla lo pedido.
    #----------------------------------
    # BEGIN TODO 1
    #----------------------------------
        # Comprobamos que el número de parámetros es 4
        if [ $# -ne 4 ]; then
            echo "Error: número de parámetros incorrecto ($#), deben ser 4."
            Sintaxis
            exit 1
        fi

        # Comprobamos que el parámetro $1 es fichero y además existe
        if [ ! -f "$1" ]; then
            echo "El parámetro <archivoComunidades> debe ser un fichero existente."
            Sintaxis
            exit 1
        fi

        # Comprobamos que el parámetro $4 es numérico
        if ! Number "$4"; then
            echo "El parámetro <partidosPoliticos> ($4) debe ser un número."
            Sintaxis
            exit 1
        fi

        # Comprobamos que el parámetro $4 esté entre 2 y 6
        if [ "$4" -lt 2 ] || [ "$4" -gt 6 ]; then
            echo "El valor del parámetro <partidosPoliticos> debe estar entre 2 y 6."
            Sintaxis
            exit 1
        fi
    #----------------------------------
    # END TODO 1
    #----------------------------------
}

NumAleatorio() {
    #----------------------------------
    # NO TOCAR - PODÉIS USARLA EN NumAleatorioRango() 
    # Parámetros: sin parametros
    # Salida:
    #   Número real aleatorio entre 0 y 1.
    #----------------------------------
    echo "scale=2; $RANDOM / 32767" | bc
}

DecimalAEntero() {
    #----------------------------------
    # NO TOCAR - PODÉIS USARLA EN NumAleatorioRango() Y
    # EN ProcesarArchivoComunidades()
    # Parámetros:
    #   $1: número decimal (ej 34.14)
    # Salida:
    #   Número $1 truncado al entero inferior.
    #----------------------------------
    echo "scale=0; $1/1" | bc
}

NumAleatorioRango() {
    #----------------------------------
    # BEGIN TODO 2
    #----------------------------------
    # Parámetros:
    #   $1: valor mínimo del número aleatorio (incluido)
    #   $2: valor máximo del número aleatorio (incluido)
    # Salida:
    #   Numero aleatorio entero entre $1 y $2.
    #----------------------------------
    local decimal=$(echo "$1 + ($(NumAleatorio) * (1 + $2 - $1))" | bc)
    echo $(DecimalAEntero $decimal)
    #----------------------------------
    # END TODO 2
    #----------------------------------
}

ProcesarArchivoComunidades() {
    #----------------------------------
    # BEGIN TODO 3
    #----------------------------------
    # Esta función lee en un fichero los nombres de las comunidad autónoma y
    # su población y devuelve otro fichero que contiene una estimación
    # aleatoria del censo electoral de cada comunidad.
    # El censo electoral se calcula como un número aleatorio entre 70 y 85
    # (ambos incluidos) multiplicado por la población de la comunidad en
    # cuestión.
    #
    # Cada fila contiene el nombre de una comunidad autónoma
    # seguida de su población, separados por una coma.
    # ej:
    #   Murcia,1581072
    #
    # La salida sigue el mismo formato:
    # ej:
    #   Murcia,1264857
    #
    # Parámetros:
    #    $1: nombre del fichero a tratar
    #    $2: nombre del fichero de salida
    #----------------------------------
    > $2
    while read line; do
        local comunidad=$(echo $line | cut -d ',' -f1)
        local poblacion=$(echo $line | cut -d ',' -f2)
        local minimo_censo=$(DecimalAEntero $(echo "$poblacion * 0.70" | bc))
        local maximo_censo=$(DecimalAEntero $(echo "$poblacion * 0.85" | bc))
        local censo=$(NumAleatorioRango $minimo_censo $maximo_censo)
        echo "$comunidad,$censo" >> $2
    done < $1
    #----------------------------------
    # END TODO 3
    #----------------------------------
}


#----------------------------------------------
# MAIN
#----------------------------------------------

#----------------------------------
# BEGIN TODO MAIN 1
#----------------------------------
# Comprobar los argumentos llamando a CheckArguments apropiadamente
CheckArguments $*

# Fijamos las variables (no tocar)
archivoComunidades=$1 # archivo que contiene las comunidades y su población separadas por una coma.
archivoVotantes=$2    # archivo que contiene las comunidades y su número de votantes separados por una coma.
archivoResultados=$3  # archivo que contiene las comunidades y los resultados electorales de cada partida separados por comas.
partidosPoliticos=$4  # número de partidos políticos que se presentan a las elecciones


# Obtener el censo electoral de cada provincia con ProcesarArchivoComunidades
ProcesarArchivoComunidades $archivoComunidades $archivoVotantes

#----------------------------------
# END TODO MAIN 1
#----------------------------------


# Lanzamos las elecciones! 
#----------------------------------
# BEGIN TODO MAIN 2
#----------------------------------
./votar $archivoVotantes $archivoResultados $partidosPoliticos 
#----------------------------------
# END TODO MAIN 2
#----------------------------------

# Una vez los votos emitidos y el recuento hecho para cada provincia,
# vamos a calcular el ganador nacional y su porcentaje de votos.
# Para ello leeremos $archivoResultados e iremos acumulando el total de los
# votos de cada partido.
#----------------------------------
# BEGIN TODO MAIN 3
#----------------------------------
# declaramos el array de total de votos para cada partido
resultados=()

# inicializamos el array de votos a 0
for ((i=0; i<$partidosPoliticos; i++)); do
    resultados[i]=0
done

# Calculamos la suma de votos de cada partido y lo guardamos en el array $resultados.
# El índice de $resultados representa el partido político correspondiente.
while read line; do
    comunidad=$(echo $line | cut -d ',' -f1)
    for ((i=0; i<$partidosPoliticos; i++)); do
        resultados[i]=$((resultados[i] + $(echo $line | cut -d ',' -f$((i+2)))))
    done
done < $archivoResultados

# buscamos el valor máximo del array y su indice
# para conocer el ganador y sus votos
max_votos=${resultados[0]}      # inicializamos al primer partido
max_indice=0                    # inicializamos al primer partido
total_votos=${resultados[0]}
for ((i=0; i<$partidosPoliticos; i++)); do
    if (( resultados[i] > max_votos )); then
        max_votos=${resultados[i]}
        max_indice=$i
    fi
    total_votos=$(($total_votos + ${resultados[i]}))
done

# obtenemos el ganador (de 1 a 6) y su porcentaje de votos
partido_ganador=$(($max_indice+1))
porcentaje_ganador=$(echo "scale=2; 100 * $max_votos / $total_votos" | bc)

echo "El partido más votado es el $partido_ganador con un $porcentaje_ganador% de los votos"
#----------------------------------
# END TODO MAIN 3
#----------------------------------
