package main

import (
	"bytes"
	"encoding/json"
	"log"
	"net/http"
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

	// Dependiendo de la disciplina, conecta con el servicio adecuado
	switch request.Discipline {
	case 1:
		// Conectar al servicio de Natación
		sendRequestToDisciplineService("http://swimming-service:80/compete")
	case 2:
		// Conectar al servicio de Atletismo
		sendRequestToDisciplineService("http://track-service:80/compete")
	case 3:
		// Conectar al servicio de Boxeo
		sendRequestToDisciplineService("http://boxing-service:80/compete")
	default:
		http.Error(w, "Invalid discipline", http.StatusBadRequest)
	}
}

func sendRequestToDisciplineService(url string) {
	// Crear un cuerpo de solicitud vacío ya que no se envía información adicional
	requestBody, _ := json.Marshal(map[string]string{})

	resp, err := http.Post(url, "application/json", bytes.NewBuffer(requestBody))
	if err != nil {
		log.Printf("Error connecting to %s: %v", url, err)
		return
	}
	defer resp.Body.Close()

	log.Printf("Solicitud enviada a %s con respuesta: %s", url, resp.Status)
}

func main() {
	http.HandleFunc("/participate", participateHandler)
	log.Println("Faculty Service is running on port 8080...")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
