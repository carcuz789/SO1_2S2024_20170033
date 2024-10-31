# Etapa de construcción
FROM rust:latest AS builder

# Crear el directorio de trabajo
WORKDIR /app

# Copiar el código fuente de `faculty_service_rust`
COPY . .

# Compilar el servicio en Rust en modo release
RUN cargo build --release

# Usar una imagen base de Debian para el contenedor final
FROM debian:latest

# Crear el directorio de trabajo
WORKDIR /app

# Copiar el ejecutable desde la etapa de construcción
COPY --from=builder /app/target/release/faculty_service_rust .

# Exponer el puerto del servicio
EXPOSE 8084

# Comando para ejecutar el servicio
CMD ["./faculty_service_rust"]
