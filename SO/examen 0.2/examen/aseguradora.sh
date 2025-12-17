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
    echo "---------------------------------------------------------------------------------------------------"
    echo "Sintaxis:                                                                                          "
    echo " ./aseguradora.sh <numTalleres> <archivoVehiculos> <numVehiculos> <archivoRessultadosReparaciones> "
    echo "  numTalleres:                  Entero. Número de talleres mecánicos. Entre 2 y 10.                "
    echo "  archivoVehiculos:             String. Nombre del archivo que contendrá los vehículos.            "
    echo "  numVehiculos:                 Entero. Número de vehículos siniestrados. Mayor o igual que 200.   "
    echo "  archivoResultado:             String. Nombre del archivo donde se volcarán los resultados.       "
    echo "---------------------------------------------------------------------------------------------------"
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
        if [ $# -ne 4 ]; then                                                                   # ...
            echo "Error: número de parámetros incorrecto ($#), deben ser 3."                    # ...
            Sintaxis                                                                            # ...
            exit 1                                                                              # ...
        fi

        # Comprobamos que Los parámetros $1 y $3 son numérico
        if ! Number "$1" || ! Number "$3"; then                                                 # ...
            echo "Los parámetros <numTalleres> ($1) y <numVehiculos> ($3) deben ser un número." # ...
            Sintaxis                                                                            # ...
            exit 1                                                                              # ...
        fi

        # Comprobamos que el parámetro $1 esté entre 2 y 10
        if [ "$1" -lt 2 ] || [ "$1" -gt 10 ]; then                                              # ...
            echo "El valor del parámetro <numTalleres> debe estar entre 2 y 10."                # ...
            Sintaxis                                                                            # ...
            exit 1                                                                              # ...
        fi

        # Comprobamos que el parámetro $3 es mayor o igual que 200
        if [ "$3" -lt 200 ]; then                                                               # ...
            echo "El valor del parámetro <numVehiculos> ($3) debe ser mayor o igual que 200"    # ...
            Sintaxis                                                                            # ...
            exit 1                                                                              # ...
        fi
    #----------------------------------
    # END TODO 1
    #----------------------------------
}

RandInt() {
    #----------------------------------
    # NO TOCAR - PODÉIS USAR ESTA FUNCIÓN EN EL TODO 3 
    # Parámetros:
    #    $1 : entero mayor que 0.
    # Salida:
    #    Número aleatorio entre 0 (incluido) y $1 (excluido).
    #----------------------------------
    local aleatorio=$(echo "scale=2; $RANDOM / 32767" | bc)
    echo "scale=0; $aleatorio * $1 / 1" | bc
}

LETRAS=($(echo {A..Z}))
VOCALES=($(echo "AEIOU" | grep -o .))

#----------------------------------
# BEGIN TODO 2
#----------------------------------
# Declaramos el array CONSONANTES que contendrá todas las consonantes válidas
# en una matrícula de vehículo a motor (una consonante por elemento del array).
CONSONANTES=()

# Medimos la longitud del array CONSONANTES en base a los arrays LETRAS y VOCALES
LEN_CONSONANTES=$((${#LETRAS[@]} - ${#VOCALES[@]}))                 # LEN_CONSONANTES=...

# Inicializamos el array CONSONANTES a 0
for ((i=0; i<$LEN_CONSONANTES; i++)); do                            # ...
    CONSONANTES[i]=0                                                # ...
done                                                                # ...

# Introducimos los valores en el array CONSONANTES.
# Recuerda que puedes utilizar las variables LETRAS y VOCALES
i=0  
for letra in "${LETRAS[@]}"; do 
    es_vocal=false   
    for vocal in "${VOCALES[@]}"; do                                # ...
        if [[ "$letra" == "$vocal" ]]; then                         # ...
            es_vocal=true                                           # ...
        fi                                                          # ...
    done                                                            # ...
    if ! $es_vocal; then   
        CONSONANTES[i]=$letra                                       # ...
        i=$((i+1))                                                  # ...
    fi
done
#----------------------------------
# END TODO 2
#----------------------------------

MatriculaAleatoria() {
    #----------------------------------
    # BEGIN TODO 3
    #----------------------------------
    # Salida:
    #   Una matrícula aleatoria formada por 4 dígitos seguidos de 3 consonantes.
    #   ej: 1234BCD
    #   Se recomienda el uso de la función RandInt.
    #----------------------------------
    local matricula=()

    # Inicializamos el array matrícula a 0
    for ((i=0; i<7; i++)); do                                       # ...
        matricula[i]=0                                              # ...
    done                                                            # ...

    # introducimos los dígitos en el array matricula
    for i in {0..3}; do                                             # ...
        matricula[i]=$(RandInt 10)                                  # ...
    done                                                            # ...

    # introducimos las consonantes en el array matricula
    for i in {4..6}; do                                             # ...
        matricula[i]=${CONSONANTES[$(RandInt $LEN_CONSONANTES)]}    # ...
    done                                                            # ...
    
    # devolvemos la matrícula como string, sin espacios
    printf "%s" "${matricula[@]}"                                   # ...
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
CheckArguments $*                                   # ...

# Fijamos las variables (no tocar)
numTalleres=$1                  # Entero. Número de talleres mecánicos.
archivoVehiculos=$2             # String. Nombre del archivo que contendrá los vehículos.
numVehiculos=$3                 # Entero. Número de vehículos siniestrados. 
archivoResultado=$4             # String. Nombre del archivo donde se volcarán los resultados.

# Escribimos $numVehiculos matriculas en $archivoVehiculos, cada una en una linea.
> $archivoVehiculos                                     # ...
for i in $(seq 1 "$numVehiculos"); do                   # ...
    echo "$(MatriculaAleatoria)" >> $archivoVehiculos   # ...
done                                                    # ...

# Lanzamos el reparto de vehículos siniestrados
./reparar $numTalleres $archivoVehiculos $archivoResultado  # ...
#----------------------------------
# END TODO MAIN 1
#----------------------------------

#----------------------------------
# BEGIN TODO MAIN 2
#----------------------------------
# Una vez los coches reparados, queremos saber que taller ha ganado más dinero
# sabiendo que obtienen 200€ de beneficio por coche reparado.
id_taller=0
vehiculos_reparados=0

i=1                                                 # ...
while read line; do                                 # ...
    if [ "$line" -gt "$vehiculos_reparados" ]; then # ...
        vehiculos_reparados=$line                   # ...
        id_taller=$i                                # ...
    fi                                              # ...
    i=$((i+1))                                      # ...
done < $archivoResultado                            # ...

beneficio=$((vehiculos_reparados * 200))            # beneficio=...

echo "El taller que más coches ha reparado es el $id_taller"
echo "y ha obtenido un beneficio de $beneficio €."
#----------------------------------
# END TODO MAIN 2
#----------------------------------
