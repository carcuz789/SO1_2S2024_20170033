# 1. Google Kubernetes Engine (GKE)
Utilicé Google Kubernetes Engine (GKE) para desplegar y gestionar una aplicación contenerizada. GKE es una plataforma de orquestación de contenedores que me permitió crear un clúster, donde gestioné la ejecución de aplicaciones en contenedores de manera automática.

Creación de Clústeres: Creé un clúster de GKE con 3 nodos en la zona us-central1-a usando el comando gcloud container clusters create. Para asegurarme de que los recursos fueran adecuados para las cargas de trabajo, asigné un tamaño de disco de 50 GB por nodo.
Configuración de los nodos: Verifiqué que los nodos estuvieran activos utilizando kubectl get nodes.
# 2. Aplicación Contenerizada en Go
Desarrollé una aplicación en Go que recibe datos en formato JSON a través de una API HTTP y los muestra en pantalla. Esta aplicación fue clave para recibir tráfico de prueba durante las pruebas de carga.

Definición de la API: Creé un servidor en Go que escucha en el puerto 8080 y expone un endpoint /student. Este endpoint recibe datos en formato JSON, los procesa y responde con un mensaje de confirmación.

Dockerización de la Aplicación: Para desplegar la aplicación en GKE, creé un archivo Dockerfile. Este archivo define el entorno en el que se ejecuta la aplicación Go. El proceso de construcción de la imagen Docker incluyó:

Etapa de compilación: Compilé la aplicación utilizando go build dentro de un contenedor.
Creación de la imagen: Subí la imagen Docker al Google Container Registry utilizando docker push, lo que me permitió desplegarla en el clúster de GKE.
# 3. Despliegue en Kubernetes
Después de construir la imagen de la aplicación, la desplegué en el clúster de Kubernetes utilizando archivos YAML que definen recursos como el Deployment y el Service.

Deployment.yaml: Definí cómo y cuántas réplicas de la aplicación se deben ejecutar en el clúster.
Service.yaml: Definí un servicio de tipo LoadBalancer que expone la aplicación al público a través de una IP externa. Esto permitió que el tráfico de prueba de Locust llegara a la aplicación Go desplegada en el clúster.
# 4. Pruebas de Carga con Locust
Utilicé Locust como herramienta para realizar pruebas de carga, simulando múltiples usuarios que interactúan con la aplicación web y midiendo su rendimiento bajo diferentes cargas.

Archivo locustfile.py: Este archivo define las tareas que simulan el comportamiento de los usuarios. Usé la clase HttpUser para enviar solicitudes POST al endpoint /student con datos JSON.
Ejecución de Locust: Ejecuté Locust localmente con locust -f locustfile.py --host=http://<IP-de-la-aplicación>, lo que me permitió configurar el número de usuarios y la tasa de solicitudes por segundo a través de la interfaz web.
Durante la prueba, monitoreé métricas como tiempos de respuesta, número de solicitudes procesadas y tasas de error, lo que me permitió analizar el comportamiento de la aplicación bajo carga.

# 5. Resultados y Análisis
El despliegue de la aplicación Go en GKE y las pruebas de carga con Locust me permitieron obtener datos valiosos sobre el rendimiento y la escalabilidad de la aplicación. Recopilé métricas clave como tiempos de respuesta promedio, máximos y mínimos, y la capacidad de la aplicación para manejar múltiples usuarios concurrentes.

![Descripción de la imagen](./IMG/Captura%20desde%202024-09-01%2023-40-37.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-09-01%2023-40-51.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-10%2023-20-35.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-15%2023-04-59.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-15%2023-09-05.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-15%2023-15-36.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-15%2023-16-33.png)
![Descripción de la imagen](./IMG/Captura%20desde%202024-10-15%2023-16-53.png)
