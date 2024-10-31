package main

import (
	"context"
	"fmt"
	"log"
	"os"

	"github.com/go-redis/redis/v8"
	"github.com/segmentio/kafka-go"
)

var (
	ctx         = context.Background()
	kafkaBroker = os.Getenv("KAFKA_BROKER") // Dirección del broker de Kafka desde entorno
	kafkaTopic  = os.Getenv("KAFKA_TOPIC")  // Tópico de Kafka desde entorno
)

func connectToRedis() *redis.Client {
	redisHost := os.Getenv("REDIS_HOST")
	if redisHost == "" {
		redisHost = "redis" // Dirección predeterminada de Redis
	}
	rdb := redis.NewClient(&redis.Options{
		Addr: fmt.Sprintf("%s:6379", redisHost),
	})
	// Prueba de conexión a Redis
	_, err := rdb.Ping(ctx).Result()
	if err != nil {
		log.Fatalf("No se pudo conectar a Redis: %v", err)
	}
	log.Println("Conexión a Redis establecida.")
	return rdb
}

func main() {
	// Conectar a Redis
	rdb := connectToRedis()
	defer rdb.Close()

	// Configurar conexión a Kafka
	if kafkaBroker == "" {
		kafkaBroker = "kafka-cluster-kafka-bootstrap.kafka.svc.cluster.local:9092"
	}
	if kafkaTopic == "" {
		kafkaTopic = "winners"
	}

	kafkaReader := kafka.NewReader(kafka.ReaderConfig{
		Brokers: []string{kafkaBroker},
		Topic:   kafkaTopic,
		GroupID: "consumer-winners-group",
	})
	defer kafkaReader.Close()
	log.Println("Conexión a Kafka establecida y consumidor de ganadores iniciado...")

	for {
		// Leer mensaje de Kafka
		message, err := kafkaReader.ReadMessage(ctx)
		if err != nil {
			log.Printf("Error al leer mensaje de Kafka: %v", err)
			continue
		}

		// Procesar mensaje y guardarlo en Redis
		studentID := string(message.Value)
		err = rdb.Set(ctx, fmt.Sprintf("winner:%s", studentID), "true", 0).Err()
		if err != nil {
			log.Printf("Error al guardar en Redis: %v", err)
		} else {
			log.Printf("Ganador guardado en Redis: %s", studentID)
		}
	}
}
