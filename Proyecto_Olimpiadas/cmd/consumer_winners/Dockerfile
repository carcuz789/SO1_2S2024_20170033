# Usar una imagen base de Golang para construir el ejecutable
FROM golang:1.23 AS builder

WORKDIR /app

# Copiar los archivos de código
COPY . .

# Descargar dependencias y construir el ejecutable
RUN go mod tidy && go build -o consumer_winners

# Usar una imagen base de Debian para el contenedor final (mejor compatibilidad)
FROM debian:latest

WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/consumer_winners .

# Exponer el puerto del servicio
EXPOSE 8085

# Ejecutar el servicio
CMD ["./consumer_winners"]
