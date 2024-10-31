from locust import HttpUser, task, between
import random

class FacultyServiceEngineeringUser(HttpUser):
    wait_time = between(1, 3)
    host = "http://35.226.180.252"  # IP pública de `faculty-service` (Ingeniería)

    @task
    def participate_engineering(self):
        # Crear una solicitud de participación para Ingeniería
        participation_request = {
            "faculty": "ingenieria",
            "discipline": random.choice([1, 2, 3])  # 1: Natación, 2: Atletismo, 3: Boxeo
        }
        
        # Enviar solicitud POST al endpoint `/participate` de `faculty-service`
        self.client.post(
            "/participate",
            json=participation_request,  # Usar `json=` para asegurarse de que el contenido se codifique correctamente como JSON
            headers={"Content-Type": "application/json"}
        )

class FacultyServiceAgronomyUser(HttpUser):
    wait_time = between(1, 3)
    host = "http://34.30.122.122"  # IP pública de `faculty-service-rust` (Agronomía)

    @task
    def participate_agronomy(self):
        # Crear una solicitud de participación para Agronomía
        participation_request = {
            "faculty": "agronomia",
            "discipline": random.choice([1, 2, 3])  # 1: Natación, 2: Atletismo, 3: Boxeo
        }
        
        # Enviar solicitud POST al endpoint `/participate` de `faculty-service-rust`
        self.client.post(
            "/participate",
            json=participation_request,  # Usar `json=` en lugar de `data=` para JSON
            headers={"Content-Type": "application/json"}
        )
