package main

import (
	"encoding/json"
	"log"
	"net/http"

	"google.golang.org/grpc"
)

type ParticipationRequest struct {
	Faculty    string `json:"faculty"`
	Discipline int    `json:"discipline"`
}

func participateHandler(w http.ResponseWriter, r *http.Request) {
	var request ParticipationRequest
	err := json.NewDecoder(r.Body).Decode(&request)
	if err != nil {
		http.Error(w, "Bad request", http.StatusBadRequest)
		return
	}

	// Dependiendo de la disciplina, conecta con el servicio adecuado usando gRPC
	switch request.Discipline {
	case 1:
		// Conectar al servicio de Natación
		connectToDisciplineService("swimming-service:8081")
	case 2:
		// Conectar al servicio de Atletismo
		connectToDisciplineService("track-service:8082")
	case 3:
		// Conectar al servicio de Boxeo
		connectToDisciplineService("boxing-service:8083")
	default:
		http.Error(w, "Invalid discipline", http.StatusBadRequest)
	}
}

func connectToDisciplineService(address string) {
	conn, err := grpc.Dial(address, grpc.WithInsecure())
	if err != nil {
		log.Printf("Error connecting to discipline service: %v", err)
		return
	}
	defer conn.Close()

	// Aquí se envía la solicitud al servicio específico (detalles omitidos)
	log.Printf("Solicitud enviada al servicio en %s", address)
}

func main() {
	http.HandleFunc("/participate", participateHandler)
	log.Println("Faculty Service is running on port 8080...")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
