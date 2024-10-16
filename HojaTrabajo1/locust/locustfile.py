from locust import HttpUser, task

class LoadTest(HttpUser):
    @task
    def send_student_data(self):
        json_data = {
            "student": "Juan Perez",
            "age": 20,
            "faculty": "INGENIERIA",
            "discipline": 1
        }
        self.client.post("/student", json=json_data)
