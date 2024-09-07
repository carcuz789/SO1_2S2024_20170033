from fastapi import FastAPI, HTTPException, Request
import matplotlib.pyplot as plt
import json
import os

app = FastAPI()

# Ruta donde se almacenarán los logs en el volumen compartido
LOGS_PATH = "/logs/logs.json"

# Cargar logs previos si existen
if os.path.exists(LOGS_PATH):
    with open(LOGS_PATH, "r") as f:
        logs = json.load(f)
else:
    logs = []

@app.post('/logs')
async def receive_logs(request: Request):
    data = await request.json()
    if data:
        logs.append(data)
        # Guardar los logs en el archivo JSON
        with open(LOGS_PATH, "w") as f:
            json.dump(logs, f)
        print(f"Log recibido: {data}")
        return {"message": "Log recibido exitosamente"}
    else:
        raise HTTPException(status_code=400, detail="Error al recibir el log")

@app.post('/generate_graphs')
async def generate_graphs():
    if not logs:
        raise HTTPException(status_code=400, detail="No hay logs para generar gráficos")

    # Suponiendo que los logs contienen información de CPU y memoria
    cpu_usages = [log.get('cpu_usage', 0) for log in logs]
    memory_usages = [log.get('memory_usage', 0) for log in logs]
    process_names = [log.get('name', 'Unknown') for log in logs]
    vsz_kb = [log.get('vsz_kb', 0) for log in logs]
    rss_kb = [log.get('rss_kb', 0) for log in logs]

    # Crear gráficos
    plt.figure(figsize=(10, 5))

    # Gráfico de uso de CPU
    plt.subplot(2, 2, 1)
    plt.bar(process_names, cpu_usages, color='b')
    plt.xlabel('Procesos')
    plt.ylabel('Uso de CPU (%)')
    plt.title('Uso de CPU por Proceso')
    plt.xticks(rotation=45)

    # Gráfico de uso de Memoria
    plt.subplot(2, 2, 2)
    plt.bar(process_names, memory_usages, color='r')
    plt.xlabel('Procesos')
    plt.ylabel('Uso de Memoria (MB)')
    plt.title('Uso de Memoria por Proceso')
    plt.xticks(rotation=45)

    
    # Gráfico de uso de VSZ en KB (como línea)
    plt.subplot(2, 2, 3)
    plt.plot(process_names, vsz_kb, marker='o', linestyle='-', color='g')
    plt.xlabel('Procesos')
    plt.ylabel('Uso de VSZ (KB)')
    plt.title('Uso de VSZ KB por Proceso')
    plt.xticks(rotation=45)

    # Gráfico de uso de RSS en KB (como línea)
    plt.subplot(2, 2, 4)
    plt.plot(process_names, rss_kb, marker='o', linestyle='-', color='m')
    plt.xlabel('Procesos')
    plt.ylabel('Uso de RSS (KB)')
    plt.title('Uso de RSS KB por Proceso')
    plt.xticks(rotation=45)



    # Guardar gráfico como archivo
    plt.tight_layout()
    if not os.path.exists("/logs/output"):
        os.makedirs("/logs/output")
    plt.savefig("/logs/output/logs_graph.png")

    return {"message": "Gráficas generadas exitosamente"}
