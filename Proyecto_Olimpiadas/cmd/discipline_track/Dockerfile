# Usar una imagen base de Golang para construir el ejecutable
FROM golang:1.23 AS builder

WORKDIR /app

# Copiar el código fuente
COPY . .

# Descargar dependencias y construir el ejecutable
RUN go mod tidy && go build -o discipline_track

# Usar una imagen base de Debian para el contenedor final (mejor compatibilidad)
FROM debian:latest

WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/discipline_track .

# Exponer el puerto del servicio
EXPOSE 8082

# Ejecutar el servicio
CMD ["./discipline_track"]
