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
		log.Println("Estudiante ganó en Boxeo")
		publishResult("winners", "boxing")
	} else {
		log.Println("Estudiante perdió en Boxeo")
		publishResult("losers", "boxing")
	}
}

func publishResult(topic string, discipline string) {
	// Conectar a Kafka y publicar resultado (detalles de conexión omitidos)
	log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
}

func main() {
	http.HandleFunc("/compete", competeHandler)
	log.Println("Boxing Service is running on port 8083...")
	log.Fatal(http.ListenAndServe(":8083", nil))
}
