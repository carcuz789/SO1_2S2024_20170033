from locust import HttpUser, task, between
import random

class OlympicsUser(HttpUser):
    # Tiempo de espera entre tareas
    wait_time = between(1, 5)

    @task
    def participate(self):
        # Datos aleatorios para simular facultades y disciplinas
        faculties = ["Ingeniería", "Agronomía"]
        disciplines = [1, 2, 3]  # 1 = Natación, 2 = Atletismo, 3 = Boxeo

        # Simulación de una solicitud de participación con datos aleatorios
        self.client.post("/participate", json={
            "Faculty": random.choice(faculties),
            "Discipline": random.choice(disciplines)
        })
