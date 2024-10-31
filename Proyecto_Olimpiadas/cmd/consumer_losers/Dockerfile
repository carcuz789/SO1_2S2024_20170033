# Usar una imagen base de Golang para construir el ejecutable
FROM golang:1.23 AS builder

WORKDIR /app

# Copiar los archivos de código
COPY . .

# Descargar dependencias y construir el ejecutable
RUN go mod tidy && go build -o consumer_losers

# Usar una imagen base de Debian para la etapa final
FROM debian:latest

WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/consumer_losers .

# Exponer el puerto del servicio
EXPOSE 8086

# Ejecutar el servicio
CMD ["./consumer_losers"]
