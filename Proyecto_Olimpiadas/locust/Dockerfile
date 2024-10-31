# Usar una imagen base de Python para Locust
FROM python:3.9

WORKDIR /app

# Copiar archivos de configuración
COPY locustfile.py /app/locustfile.py
COPY requirements.txt /app/requirements.txt

# Instalar dependencias
RUN pip install -r requirements.txt

# Exponer el puerto para acceder a la interfaz de Locust
EXPOSE 8089

# Comando para iniciar Locust
CMD ["locust", "-f", "locustfile.py"]
