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

var (
	kafkaBroker = os.Getenv("KAFKA_BROKER") // Dirección del broker de Kafka (usar variable de entorno)
)

// Configura un contexto de fondo para Kafka
var ctx = context.Background()

// Configura el cliente de Kafka
func connectToKafka(topic string) *kafka.Writer {
	return &kafka.Writer{
		Addr:     kafka.TCP(kafkaBroker),
		Topic:    topic,
		Balancer: &kafka.LeastBytes{},
	}
}

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
	// Conectar a Kafka y publicar el resultado
	writer := connectToKafka(topic)
	defer writer.Close()

	message := kafka.Message{
		Value: []byte(fmt.Sprintf("Disciplina: %s - Resultado: %s", discipline, topic)),
	}

	err := writer.WriteMessages(ctx, message)
	if err != nil {
		log.Printf("Error al publicar en Kafka - Tópico: %s, Error: %v", topic, err)
	} else {
		log.Printf("Resultado publicado en Kafka - Tópico: %s, Disciplina: %s", topic, discipline)
	}
}

func main() {
	http.HandleFunc("/compete", competeHandler)
	log.Println("Swimming Service is running on port 8081...")
	log.Fatal(http.ListenAndServe(":8081", nil))
}
