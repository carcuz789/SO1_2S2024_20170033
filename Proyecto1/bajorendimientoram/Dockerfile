FROM ubuntu:latest
RUN apt-get update && apt-get install -y stress

# Definir variables de entorno por defecto (pueden ser sobrescritas al ejecutar el contenedor)
ENV CONTAINER_NAME="default_name"
ENV CONTAINER_ID="default_id"

CMD ["/bin/bash", "-c", "stress --vm 1 --vm-bytes 128M --timeout 60; echo \"Container Name: $CONTAINER_NAME\"; echo \"Container ID: $CONTAINER_ID\"; tail -f /dev/null"]
