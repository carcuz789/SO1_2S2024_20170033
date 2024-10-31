# Usar una imagen base de Golang
FROM golang:1.23 AS builder

# Establecer el directorio de trabajo en el contenedor
WORKDIR /app

# Copiar solo la carpeta del servicio
COPY . .

# Descargar dependencias y construir el ejecutable
RUN go mod tidy && go build -o discipline_boxing

# Usar una imagen más ligera para el contenedor final
FROM alpine:latest

# Crear un directorio en el contenedor final
WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/discipline_boxing .

# Exponer el puerto del servicio
EXPOSE 8083

# Comando para ejecutar el servicio
CMD ["./discipline_boxing"]
