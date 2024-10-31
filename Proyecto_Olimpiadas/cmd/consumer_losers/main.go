package main

import (
	"context"
	"fmt"
	"log"
	"os"
	"time"

	"github.com/go-redis/redis/v8"
	"github.com/segmentio/kafka-go"
)

// Contexto global para Redis
var ctx = context.Background()

// Función para conectar a Redis
func connectToRedis() *redis.Client {
	redisHost := os.Getenv("REDIS_HOST")
	if redisHost == "" {
		redisHost = "redis"
	}

	rdb := redis.NewClient(&redis.Options{
		Addr: fmt.Sprintf("%s:6379", redisHost),
	})

	// Verificar conexión con Redis
	_, err := rdb.Ping(ctx).Result()
	if err != nil {
		log.Fatalf("Error al conectar a Redis: %v", err)
	}
	log.Println("Conectado a Redis exitosamente")

	return rdb
}

// Función para conectar a Kafka y verificar conexión
func connectToKafka() *kafka.Reader {
	kafkaBroker := os.Getenv("KAFKA_BROKER")
	if kafkaBroker == "" {
		kafkaBroker = "kafka-cluster-kafka-bootstrap.kafka.svc.cluster.local:9092"
	}

	kafkaTopic := os.Getenv("KAFKA_TOPIC")
	if kafkaTopic == "" {
		kafkaTopic = "losers"
	}

	// Configuración de conexión a Kafka
	kafkaReader := kafka.NewReader(kafka.ReaderConfig{
		Brokers: []string{kafkaBroker},
		Topic:   kafkaTopic,
		GroupID: "consumer-losers-group",
	})

	// Verificar conexión a Kafka con un intento de lectura
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	_, err := kafkaReader.FetchMessage(ctx)
	if err != nil {
		log.Printf("Conexión inicial a Kafka fallida (esperado si no hay mensajes): %v", err)
	} else {
		log.Println("Conectado a Kafka exitosamente")
	}

	return kafkaReader
}

func main() {
	// Conectar a Redis
	rdb := connectToRedis()
	defer rdb.Close()

	// Conectar a Kafka
	kafkaReader := connectToKafka()
	defer kafkaReader.Close()

	log.Println("Consumer de perdedores iniciado...")

	for {
		// Leer mensaje de Kafka
		message, err := kafkaReader.ReadMessage(ctx)
		if err != nil {
			log.Printf("Error al leer mensaje de Kafka: %v", err)
			continue
		}

		// Procesar mensaje y guardarlo en Redis
		studentID := string(message.Value)
		err = rdb.Set(ctx, fmt.Sprintf("loser:%s", studentID), "true", 0).Err()
		if err != nil {
			log.Printf("Error al guardar en Redis: %v", err)
		} else {
			log.Printf("Perdedor guardado en Redis: %s", studentID)
		}
	}
}
