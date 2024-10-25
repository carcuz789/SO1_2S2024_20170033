package main

import (
	"log"
	"math/rand"
	"net/http"
	"time"
)

func competeHandler(w http.ResponseWriter, r *http.Request) {
	rand.Seed(time.Now().UnixNano())
	isWinner := rand.Intn(2) == 1

	if isWinner {
		log.Println("Estudiante ganó en Atletismo")
		publishResult("winners", "track")
	} else {
		log.Println("Estudiante perdió en Atletismo")
		publishResult("losers", "track")
	}
}

func publishResult(topic string, discipline string) {
	// Conectar a Kafka y publicar resultado (detalles de conexión omitidos)
	log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
}

func main() {
	http.HandleFunc("/compete", competeHandler)
	log.Println("Track Service is running on port 8082...")
	log.Fatal(http.ListenAndServe(":8082", nil))
}
