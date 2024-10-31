package main

import (
	"context"
	"log"
	"math/rand"
	"net/http"
	"os"
	"time"

	"github.com/segmentio/kafka-go"
)

// Configuración de Kafka
var (
	kafkaBroker = os.Getenv("KAFKA_BROKER")
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
	// Conectar a Kafka
	writer := kafka.NewWriter(kafka.WriterConfig{
		Brokers: []string{kafkaBroker},
		Topic:   topic,
	})
	defer writer.Close()

	// Publicar mensaje en Kafka
	err := writer.WriteMessages(context.Background(),
		kafka.Message{
			Key:   []byte("discipline"),
			Value: []byte(discipline),
		},
	)
	if err != nil {
		log.Printf("Error al publicar en Kafka: %v", err)
	} else {
		log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
	}
}

func main() {
	// Configuración y ejecución del servidor
	http.HandleFunc("/compete", competeHandler)
	log.Println("Boxing Service is running on port 8083...")
	log.Fatal(http.ListenAndServe(":8083", nil))
}
