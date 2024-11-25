#include "DiskManager.h"
#include <algorithm>
#include <sstream>

DiskManager::DiskManager(Disk& disk)
    : disk(disk), block_size(disk.getBlockCapacity()) {
    initializeHeapFile();
}

DiskManager::~DiskManager() {}

// Inicializa la organizacion del archivo de heap y carga los headers de los bloques
void DiskManager::initializeHeapFile() {
    block_headers.resize(disk.getNumBlocks());
    for (int i = 0; i < disk.getNumBlocks(); i++) {
        BlockHeader header = {0, block_size, {}};
        block_headers[i] = header;
        free_space_map[i] = block_size;
    }
}

// Inserta registro de longitud fija
void DiskManager::insertFixedLengthRecord(const std::string& record, int record_size) {
    if (record.size() > record_size) {
        std::cerr << "Error: El tamaño del registro excede el limite de tamaño fijo." << std::endl;
        return;
    }

    int block_index = findFreeBlock(record_size);
    if (block_index == -1) {
        std::cerr << "Error: No se encontro un bloque libre para la insercion del registro." << std::endl;
        return;
    }

    int offset = findSlotInBlock(block_index, record_size);
    if (offset == -1) {
        std::cerr << "Error: No hay espacio disponible en el bloque." << std::endl;
        return;
    }

    // Escribe el registro en el bloque
    std::vector<char> block_data = readBlock(block_index);
    std::copy(record.begin(), record.end(), block_data.begin() + offset);
    writeBlock(block_index, block_data);

    // Actualiza el header y el mapa de espacio libre
    block_headers[block_index].record_count++;
    block_headers[block_index].record_offsets.push_back(offset);
    updateFreeSpaceMap(block_index);
}

// Elimina registro de longitud fija
void DiskManager::deleteFixedLengthRecord(int block_index, int record_index) {
    if (block_index < 0 || block_index >= disk.getNumBlocks() ||
        record_index < 0 || record_index >= block_headers[block_index].record_count) {
        std::cerr << "Error: Bloque o indice de registro invalidos para la eliminacion." << std::endl;
        return;
    }

    int offset = block_headers[block_index].record_offsets[record_index];
    block_headers[block_index].record_offsets[record_index] = -1;

    // Marca el registro como eliminado y actualiza el espacio libre
    block_headers[block_index].record_count--;
    updateFreeSpaceMap(block_index);
}

// Inserta registro de longitud variable
void DiskManager::insertVariableLengthRecord(const std::string& record) {
    int record_size = record.size();
    insertFixedLengthRecord(record, record_size);
}

// Elimina registro de longitud variable
void DiskManager::deleteVariableLengthRecord(int block_index, int record_index) {
    deleteFixedLengthRecord(block_index, record_index);
}

// Encuentra un bloque libre para la insercion
int DiskManager::findFreeBlock(int record_size) {
    for (const auto& [block_index, free_space] : free_space_map) {
        if (free_space >= record_size) {
            return block_index;
        }
    }
    return -1;
}

// Carga los headers de los bloques del disco
void DiskManager::loadHeaders() {
    for (int i = 0; i < disk.getNumBlocks(); i++) {
        std::vector<char> block_data = readBlock(i);
    }
}

// Actualiza el mapa de espacio libre después de la inserción o eliminación
void DiskManager::updateFreeSpaceMap(int block_index) {
    int used_space = block_headers[block_index].record_count * block_size;
    free_space_map[block_index] = block_size - used_space;
}

// Lee un bloque del disco
std::vector<char> DiskManager::readBlock(int block_index) {
    std::string block_path = disk.getBlockPath(block_index);
    std::ifstream file(block_path, std::ios::binary);

    std::vector<char> block_data(block_size);
    file.read(block_data.data(), block_size);
    return block_data;
}

// Escribe un bloque en el disco
void DiskManager::writeBlock(int block_index, const std::vector<char>& block_data) {
    std::string block_path = disk.getBlockPath(block_index);
    std::ofstream file(block_path, std::ios::binary);
    file.write(block_data.data(), block_data.size());
}

// Encuentra slot para el registro en el bloque
int DiskManager::findSlotInBlock(int block_index, int record_size) {
    if (block_headers[block_index].free_space_end < record_size) {
        return -1;
    }
    int offset = block_headers[block_index].free_space_end - record_size;
    block_headers[block_index].free_space_end = offset;
    return offset;
}
