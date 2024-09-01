#!/bin/bash

# Lista de imágenes Docker disponibles
IMAGES=("bajo_cpu" "bajo_ram" "alto_ram" "alto_cpu")

# Generar 10 contenedores aleatorios
for i in {1..10}
do
  # Generar un nombre único para el contenedor con una marca de tiempo
  TIMESTAMP=$(date +%s)
  IMAGE=${IMAGES[$RANDOM % ${#IMAGES[@]}]}
  NAME="${IMAGE}_${TIMESTAMP}_${i}"
  
  # Crear y ejecutar el contenedor
  sudo docker run -d --name "$NAME" "$IMAGE" && echo "Created container with name: $NAME" || echo "Failed to create container with name: $NAME"
done
