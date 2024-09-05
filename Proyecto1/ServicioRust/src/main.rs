use std::fs::File;
use std::io::{self, Read};
use std::path::Path;
use std::process::{Command, Output};
use serde::{Deserialize, Serialize};
use std::os::unix::process::ExitStatusExt; // Importación para ExitStatusExt
use reqwest::blocking::Client; // Importación corregida para usar la API blocking
use serde_json::json;
use std::thread;
use std::time::Duration; // Usado para el tiempo de sleep




// CREACIÓN DE STRUCT
#[derive(Debug, Serialize, Deserialize)]
struct SystemInfo {
    #[serde(rename = "total_memory_kb")]
    total_memory_kb: u64,
    #[serde(rename = "free_memory_kb")]
    free_memory_kb: u64,
    #[serde(rename = "used_memory_kb")]
    used_memory_kb: u64,
    #[serde(rename = "processes")]
    processes: Vec<Process>,
}

#[derive(Debug, Serialize, Deserialize, PartialEq)]
struct Process {
    #[serde(rename = "pid")]
    pid: u32,
    #[serde(rename = "name")]
    name: String,
    #[serde(rename = "Cmdline")]
    cmd_line: String,
    #[serde(rename = "Memoria_Utilizada")]
    memory_usage: f64,
    #[serde(rename = "Cpu_Utilizado")]
    cpu_usage: f64,
}

#[derive(Debug, Serialize, Clone)]
struct LogProcess {
    pid: u32,
    container_id: String,
    name: String,
    memory_usage: f64,
    cpu_usage: f64,
}

// IMPLEMENTACIÓN DE MÉTODOS

impl Process {
    fn get_container_id(&self) -> &str {
        let parts: Vec<&str> = self.cmd_line.split_whitespace().collect();
        for (i, part) in parts.iter().enumerate() {
            if *part == "-id" {
                if let Some(id) = parts.get(i + 1) {
                    return id;
                }
            }
        }
        "N/A"
    }
}

impl Eq for Process {}

impl Ord for Process {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.cpu_usage.partial_cmp(&other.cpu_usage).unwrap_or(std::cmp::Ordering::Equal)
            .then_with(|| self.memory_usage.partial_cmp(&other.memory_usage).unwrap_or(std::cmp::Ordering::Equal))
    }
}

impl PartialOrd for Process {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        Some(self.cmp(other))
    }
}

// FUNCIONES
fn send_log_to_fastapi(log_process: &LogProcess) -> Result<(), reqwest::Error> {
    let client = Client::new();
    let url = "http://localhost:8081/logs"; // Cambia 'localhost' por la IP del contenedor si es necesario

    let response = client
        .post(url)
        .json(&json!({
            "pid": log_process.pid,
            "name": log_process.name,
            "memory_usage": log_process.memory_usage,
            "cpu_usage": log_process.cpu_usage,
            "container_id": log_process.container_id,
        }))
        .send()?;

    if response.status().is_success() {
        println!("Log enviado exitosamente al contenedor de FastAPI");
    } else {
        println!("Error al enviar log: {:?}", response.status());
    }

    Ok(())
}

// Función para crear el contenedor de administración de logs
fn create_log_container() -> Result<(), String> {
    let output = Command::new("docker")
        .arg("run")
        .arg("-d")
        .arg("--name")
        .arg("log_manager")
        .arg("-v")
        .arg("/logs:/logs")  // Montar volumen compartido
        .arg("-p")
        .arg("8081:8081")  // Mapeo de puertos
        .arg("log-manager-fastapi") // Imagen del contenedor
        .output();

    match output {
        Ok(output) => {
            if output.status.success() {
                println!("Contenedor de administración de logs creado exitosamente.");
                Ok(())
            } else {
                Err(format!(
                    "Error al crear el contenedor: {}",
                    String::from_utf8_lossy(&output.stderr)
                ))
            }
        }
        Err(e) => Err(format!("Error al ejecutar el comando Docker: {}", e)),
    }
}
fn kill_container(id: &str) -> Output {
    // Verificar si el contenedor es el de log-manager-fastapi
    if id == "log-manager-fastapi" {
        println!("El contenedor log-manager-fastapi no puede ser eliminado.");
        return Output {
            status: std::process::ExitStatus::from_raw(0),
            stdout: Vec::new(),
            stderr: Vec::new(),
        };
    }

    let output = Command::new("sudo")
        .arg("docker")
        .arg("stop")
        .arg(id)
        .output()
        .expect("failed to execute process");

    println!("Matando contenedor con id: {}", id);

    output
}

fn analyzer(system_info: SystemInfo) {
    // Imprimir la información de la memoria
    println!("------------------------------");
    println!("Información de la memoria:");
    println!("Memoria total (kb): {}", system_info.total_memory_kb);
    println!("Memoria libre (kb): {}", system_info.free_memory_kb);
    println!("Memoria utilizada (kb): {}", system_info.used_memory_kb);
    println!("------------------------------");

    let mut log_proc_list: Vec<LogProcess> = Vec::new();
    let mut processes_list: Vec<Process> = system_info.processes;

    processes_list.sort();

    let (lowest_list, highest_list) = processes_list.split_at(processes_list.len() / 2);

    println!("Bajo consumo");
    for process in lowest_list {
        println!(
            "PID: {}, Name: {}, container ID: {}, Memory Usage: {}, CPU Usage: {}",
            process.pid,
            process.name,
            process.get_container_id(),
            process.memory_usage,
            process.cpu_usage
        );
        
    }

    println!("------------------------------");

    println!("Alto consumo");
    for process in highest_list {
        println!(
            "PID: {}, Name: {}, container ID: {}, Memory Usage: {}, CPU Usage: {}",
            process.pid,
            process.name,
            process.get_container_id(),
            process.memory_usage,
            process.cpu_usage
        );
    }

    println!("------------------------------");

    if lowest_list.len() > 3 {
        for process in lowest_list.iter().skip(3) {
            let log_process = LogProcess {
                pid: process.pid,
                container_id: process.get_container_id().to_string(),
                name: process.name.clone(),
                memory_usage: process.memory_usage,
                cpu_usage: process.cpu_usage,
            };

            log_proc_list.push(log_process.clone());
            
            //let _output = kill_container(&process.get_container_id());

            if let Err(e) = send_log_to_fastapi(&log_process) {
                println!("Error al enviar log: {}", e);
            }
        }
    }

    if highest_list.len() > 2 {
        for process in highest_list.iter().take(highest_list.len() - 2) {
            let log_process = LogProcess {
                pid: process.pid,
                container_id: process.get_container_id().to_string(),
                name: process.name.clone(),
                memory_usage: process.memory_usage,
                cpu_usage: process.cpu_usage,
            };

            log_proc_list.push(log_process.clone());
            //let _output = kill_container(&process.get_container_id());
            if let Err(e) = send_log_to_fastapi(&log_process) {
                println!("Error al enviar log: {}", e);
            }
        }
    }

    println!("Contenedores matados");
    for process in log_proc_list {
        println!(
            "PID: {}, Name: {}, Container ID: {}, Memory Usage: {}, CPU Usage: {} ",
            process.pid, process.name, process.container_id, process.memory_usage, process.cpu_usage
        );
    }

    println!("------------------------------");

    
}

fn read_proc_file(file_name: &str) -> io::Result<String> {
    let path = Path::new("/proc").join(file_name);
    let mut file = File::open(path)?;
    let mut content = String::new();
    file.read_to_string(&mut content)?;
    Ok(content)
}

fn parse_proc_to_struct(json_str: &str) -> Result<SystemInfo, serde_json::Error> {
    let system_info: SystemInfo = serde_json::from_str(json_str)?;
    Ok(system_info)
}

fn main() {
    // Crear el contenedor de administración de logs al iniciar el servicio
    match create_log_container() {
        Ok(_) => println!("Contenedor de logs creado correctamente."),
        Err(e) => {
            println!("Error al crear el contenedor de logs: {}", e);
            return;
        }
    }

    // Loop principal del servicio
    loop {
        let json_str = read_proc_file("sysinfo_201700633").unwrap();
        let system_info = parse_proc_to_struct(&json_str);

        match system_info {
            Ok(info) => {
                analyzer(info);
            }
            Err(e) => println!("Failed to parse JSON: {}", e),
        }

        // Aquí podrías añadir la lógica para enviar logs al contenedor de FastAPI
        // usando `reqwest` o alguna otra biblioteca HTTP de Rust.

        std::thread::sleep(std::time::Duration::from_secs(10));
    }
    
    let client = Client::new();
    let url = "http://localhost:8081/generate_graphs";

    // Crear el payload si se requiere (en este caso es un POST vacío)
    let response = client.post(url)
        .send();

    match response {
        Ok(resp) => {
            if resp.status().is_success() {
                println!("Gráficas generadas exitosamente.");
            } else {
                println!("Error al generar gráficas: {}", resp.status());
            }
        }
        Err(err) => {
            println!("Error al realizar la solicitud: {}", err);
        }
    }

}
