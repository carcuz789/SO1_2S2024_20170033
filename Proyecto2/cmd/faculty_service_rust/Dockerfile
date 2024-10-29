# Etapa de compilación
FROM rust:latest as builder

# Crear el directorio de trabajo
WORKDIR /app

# Copiar archivos y compilar el servicio
COPY . .
RUN cargo build --release

# Etapa final
FROM debian:buster-slim
WORKDIR /app
COPY --from=builder /app/target/release/faculty_service_rust .

# Exponer el puerto del servicio
EXPOSE 8084

# Comando para ejecutar el servicio
CMD ["./faculty_service_rust"]
