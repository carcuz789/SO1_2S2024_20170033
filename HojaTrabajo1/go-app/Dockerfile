# Etapa 1: Construcción de la aplicación
FROM golang:1.18-alpine AS builder

WORKDIR /app

# Copia todo el código fuente al directorio de trabajo
COPY . .

# Compila la aplicación Go, especificando el archivo principal
RUN go build -o go-app Servidorgo.go

# Etapa 2: Imagen final
FROM alpine:3.15

WORKDIR /app

# Copia el binario de la aplicación desde la etapa de construcción
COPY --from=builder /app/go-app .

EXPOSE 8080

CMD ["./go-app"]
