#include "Disk.h"
#include <filesystem>

Disk::Disk() {
    this->capacity = 2 * 1024 * 1024; //10MB
    this->block_capacity = 4 * 1024; //4KB
    this->num_blocks = capacity / block_capacity;
    this->path = "../db/disco1";

    create_blocks();
}

Disk::Disk(int capacity, int block_capacity, std::string path = "../db/disco1") {
    this->capacity = capacity;
    this->block_capacity = block_capacity;
    this->num_blocks = capacity / block_capacity;
    this->path = path;

    create_blocks();
}

Disk::~Disk() {}

void Disk::create_blocks() {
    try {
        // Verificar si la carpeta ya existe
        if (std::filesystem::exists(this->path)) {
            // Borrar el contenido de la carpeta existente
            std::filesystem::remove_all(this->path);
        }

        // Crear la carpeta para el disco
        std::filesystem::create_directories(this->path);

        // Crear los bloques como archivos dentro de la carpeta del disco
        for (int i = 1; i <= this->num_blocks; i++) {
            std::string block_filename = this->path + "/bloque" + std::to_string(i) + ".txt";
            std::ofstream block_file(block_filename);

            if (!block_file) {
                throw std::runtime_error("No se pudo crear el archivo: " + block_filename);
            }

            block_file << ""; // Inicializar el archivo vacío
            block_file.close();
        }

        std::cout << "Se crearon " << this->num_blocks << " bloques en la carpeta " << this->path << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error al crear bloques: " << e.what() << std::endl;
    }
}