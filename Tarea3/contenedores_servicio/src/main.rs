use std::fs::{File, OpenOptions};
use std::io::{self, Read, Write};
use serde_json::{json, Value};

const UMBRAL_CPU_ALTO: f64 = 50.0;  // Umbral para considerar un alto consumo de CPU (en %)
const UMBRAL_RAM_ALTO: f64 = 50.0;  // Umbral para considerar un alto consumo de RAM (en %)

fn main() -> io::Result<()> {
    let file_path = "/proc/sysinfo_201700633";
    let mut file = File::open(file_path)?;

    let mut contents = String::new();
    file.read_to_string(&mut contents)?;

    let json_value: Value = serde_json::from_str(&contents)?;

    let processes = json_value["processes"].as_array().unwrap();

    let mut contenedores_alto_consumo: Vec<&Value> = Vec::new();
    let mut contenedores_bajo_consumo: Vec<&Value> = Vec::new();

    for process in processes {
        if let (Some(cpu), Some(ram)) = (process.get("Cpu Utilizado"), process.get("Memoria Utilizada")) {
            let cpu_usage: f64 = cpu.as_str().unwrap_or("0").replace(" %", "").parse().unwrap_or(0.0);
            let ram_usage: f64 = ram.as_str().unwrap_or("0").replace(" %", "").parse().unwrap_or(0.0);

            println!("Contenedor encontrado: CPU = {}%, RAM = {}%", cpu_usage, ram_usage);

            if cpu_usage > UMBRAL_CPU_ALTO || ram_usage > UMBRAL_RAM_ALTO {
                contenedores_alto_consumo.push(process);
            } else {
                contenedores_bajo_consumo.push(process);
            }
        } else {
            println!("No se encontraron datos de CPU o RAM para este contenedor: {:?}", process);
        }
    }

    println!("Contenedores de alto consumo:");
    for cont in &contenedores_alto_consumo {
        println!("{}", serde_json::to_string_pretty(cont).unwrap());
    }

    println!("Contenedores de bajo consumo:");
    for cont in &contenedores_bajo_consumo {
        println!("{}", serde_json::to_string_pretty(cont).unwrap());
    }

    let corrected_json = json!(json_value);

    let output_path = "./corrected_sysinfo_201700633.json";
    let mut output_file = OpenOptions::new().write(true).create(true).open(output_path)?;
    write!(output_file, "{}", serde_json::to_string_pretty(&corrected_json)?)?;

    //println!("Archivo JSON corregido guardado en: {}", output_path);

    Ok(())
}