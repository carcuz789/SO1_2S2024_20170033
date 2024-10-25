package main

import (
	"log"
	"math/rand"
	"net/http"
	"time"
)

func competeHandler(w http.ResponseWriter, r *http.Request) {
	rand.Seed(time.Now().UnixNano())
	isWinner := rand.Intn(2) == 1 // 50% de probabilidad de ganar

	if isWinner {
		log.Println("Estudiante ganó en Natación")
		publishResult("winners", "swimming")
	} else {
		log.Println("Estudiante perdió en Natación")
		publishResult("losers", "swimming")
	}
}

func publishResult(topic string, discipline string) {
	// Conectar a Kafka y publicar resultado (detalles de conexión omitidos)
	log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
}

func main() {
	http.HandleFunc("/compete", competeHandler)
	log.Println("Swimming Service is running on port 8081...")
	log.Fatal(http.ListenAndServe(":8081", nil))
}
