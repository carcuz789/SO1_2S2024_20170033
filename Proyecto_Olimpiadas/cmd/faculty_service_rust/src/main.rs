use actix_web::{web, App, HttpServer, Responder, HttpResponse};
use serde::Deserialize;
use rand::Rng;

// Estructura de la solicitud
#[derive(Deserialize)]
struct ParticipationRequest {
    faculty: String,
    discipline: u8,
}

// Manejador para la solicitud de participación
async fn participate(request: web::Json<ParticipationRequest>) -> impl Responder {
    let discipline_server = match request.discipline {
        1 => "swimming-service:8081",
        2 => "track-service:8082",
        3 => "boxing-service:8083",
        _ => return HttpResponse::BadRequest().body("Invalid discipline"),
    };
    HttpResponse::Ok().body(format!("Redirigiendo a: {}", discipline_server))
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    HttpServer::new(|| {
        App::new()
            .route("/participate", web::post().to(participate))
    })
    .bind("0.0.0.0:8084")?
    .run()
    .await
}
