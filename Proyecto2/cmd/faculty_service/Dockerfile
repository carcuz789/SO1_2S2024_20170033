# Usar una imagen base de Golang
FROM golang:1.16 AS builder

# Establecer el directorio de trabajo
WORKDIR /app

# Copiar los archivos del proyecto
COPY . .

# Descargar las dependencias y construir el ejecutable
RUN go mod tidy && go build -o faculty_service ./cmd/faculty_service

# Usar una imagen más ligera para el contenedor final
FROM alpine:latest

# Crear un directorio en el contenedor
WORKDIR /app

# Copiar el ejecutable desde la imagen de construcción
COPY --from=builder /app/faculty_service .

# Exponer el puerto usado por el servicio
EXPOSE 8080

# Comando para ejecutar el servicio
CMD ["./faculty_service"]
