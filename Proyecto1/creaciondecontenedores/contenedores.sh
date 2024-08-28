#!/bin/bash

# Lista de imágenes Docker disponibles
IMAGES=("altorendimientoram" "altorendimientocpu" "bajorendimientoram" "bajorendimientocpu")

# Generar 10 contenedores aleatorios
for i in {1..10}
do
  # Generar un nombre aleatorio para el contenedor usando /dev/urandom
  RANDOM_NAME=$(head /dev/urandom | tr -dc A-Za-z0-9 | head -c 13)

  # Seleccionar una imagen aleatoria de la lista
  IMAGE=${IMAGES[$RANDOM % ${#IMAGES[@]}]}

  # Concatenar el nombre aleatorio con el nombre de la imagen
  NAME="${RANDOM_NAME}_${IMAGE}"

  # Crear y ejecutar el contenedor con sudo
  sudo docker run -d --name "$NAME" "$IMAGE"
done
