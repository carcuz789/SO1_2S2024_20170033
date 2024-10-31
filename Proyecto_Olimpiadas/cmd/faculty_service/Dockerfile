# Usar una imagen base de Golang para construir el ejecutable
FROM golang:1.23 AS builder

WORKDIR /app

# Copiar el código fuente
COPY . .

# Descargar dependencias y construir el ejecutable
RUN go mod tidy && go build -o faculty_service

# Usar una imagen base de Debian para el contenedor final
FROM debian:latest

WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/faculty_service .

# Exponer el puerto del servicio
EXPOSE 8080

# Ejecutar el servicio
CMD ["./faculty_service"]
