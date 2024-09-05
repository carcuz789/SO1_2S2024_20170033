# Utilizar una imagen de Python con FastAPI
FROM tiangolo/uvicorn-gunicorn-fastapi:python3.9

# Crear un directorio para la aplicación
WORKDIR /app

# Instalar dependencias adicionales
RUN pip install matplotlib

# Copiar los archivos de la aplicación al directorio de trabajo del contenedor
COPY app.py /app/app.py

# Crear un directorio para los logs
RUN mkdir /logs

# Exponer el puerto
EXPOSE 8081

# Ejecutar el servidor de FastAPI
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8081"]
