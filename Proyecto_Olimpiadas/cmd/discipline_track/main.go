package main

import (
	"context"
	"fmt"
	"log"
	"math/rand"
	"net/http"
	"os"
	"time"

	"github.com/segmentio/kafka-go"
)

// Configuración para Kafka
var (
	kafkaBroker = os.Getenv("KAFKA_BROKER") // Dirección del broker de Kafka desde las variables de entorno
	ctx         = context.Background()
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
	writer := kafka.NewWriter(kafka.WriterConfig{
		Brokers: []string{kafkaBroker},
		Topic:   topic,
	})
	defer writer.Close()

	err := writer.WriteMessages(ctx, kafka.Message{
		Value: []byte(fmt.Sprintf("Resultado de %s: %s", discipline, topic)),
	})
	if err != nil {
		log.Printf("Error al publicar en Kafka - Tópico: %s, Error: %v", topic, err)
		return
	}
	log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
}

func main() {
	http.HandleFunc("/compete", competeHandler)
	log.Println("Track Service is running on port 8082...")
	log.Fatal(http.ListenAndServe(":8082", nil))
}
