#include "DiskManager.h"
#include <algorithm>
#include <sstream>
#include <numeric>

DiskManager::DiskManager(Disk& disk)
    : disk(disk), block_size(disk.getBlockCapacity()) {
    initializeHeapFile();
}

DiskManager::~DiskManager() {}

// Inicializa la organizacion del archivo de heap y carga los headers de los bloques
void DiskManager::initializeHeapFile() {
    fixed_block_headers.resize(disk.getNumBlocks());
    variable_block_headers.resize(disk.getNumBlocks());

    for (int i = 0; i < disk.getNumBlocks(); i++) {
        // Inicializa encabezados fijos
        FixedBlockHeader fixed_header = {0, block_size, {}};
        fixed_block_headers[i] = fixed_header;
        fixed_free_space_map[i] = block_size;

        // Inicializa encabezados variables
        VariableBlockHeader variable_header = {0, block_size, {}};
        variable_block_headers[i] = variable_header;
        variable_free_space_map[i] = block_size;
    }
}

// Inserta registro de longitud fija
void DiskManager::insertFixedLengthRecord(const std::string& record, int record_size) {
    if (record.size() > record_size) {
        std::cerr << "Error: El tamaño del registro excede el limite de tamaño fijo." << std::endl;
        return;
    }

    int block_index = findFreeBlock(record_size, true);
    if (block_index == -1) {
        std::cerr << "Error: No se encontro un bloque libre para la insercion del registro." << std::endl;
        return;
    }

   int offset;
    if (!fixed_block_headers[block_index].free_list.empty()) {
        // Usa una ranura libre de la lista
        offset = fixed_block_headers[block_index].free_list.back();
        fixed_block_headers[block_index].free_list.pop_back();
    } else {
        // Usa el espacio libre al final del bloque
        offset = fixed_block_headers[block_index].free_space_end - record_size;
        fixed_block_headers[block_index].free_space_end = offset;
    }

    // Escribe el registro en el bloque
    std::vector<char> block_data = readBlock(block_index);
    std::copy(record.begin(), record.end(), block_data.begin() + offset);
    writeBlock(block_index, block_data);

    // Actualiza el encabezado
    fixed_block_headers[block_index].record_count++;
    updateFreeSpaceMap(block_index, true);
}

// Elimina registro de longitud fija
void DiskManager::deleteFixedLengthRecord(int block_index, int record_index) {
    if (block_index < 0 || block_index >= disk.getNumBlocks()) {
        std::cerr << "Error: Bloque inválido para la eliminación." << std::endl;
        return;
    }

    auto& header = fixed_block_headers[block_index];
    if (record_index < 0 || record_index >= header.record_count) {
        std::cerr << "Error: Índice de registro inválido." << std::endl;
        return;
    }

    // Encuentra el offset del registro a eliminar
    int record_offset = header.free_list[record_index];
    if (record_offset == -1) {
        std::cerr << "Error: El registro ya fue eliminado." << std::endl;
        return;
    }

    // Marca el registro como eliminado en la free list
    header.free_list[record_index] = -1;
    header.record_count--;

    // Actualiza el mapa de espacio libre
    updateFreeSpaceMap(block_index, true);

    std::cout << "Registro fijo eliminado correctamente del bloque " << block_index
              << " en la posición " << record_index << "." << std::endl;
}

// Inserta registro de longitud variable
void DiskManager::insertVariableLengthRecord(const std::string& record) {
    int record_size = record.size();

    int block_index = findFreeBlock(record_size + sizeof(int) * 2, false); // Considera el espacio para la ranura
    if (block_index == -1) {
        std::cerr << "Error: No hay bloques disponibles para registros variables." << std::endl;
        return;
    }

    auto& header = variable_block_headers[block_index];

    int offset = header.free_space_end - record_size;
    if (offset < header.slot_directory_end + sizeof(int) * 2) {
        std::cerr << "Error: No hay espacio suficiente en el bloque." << std::endl;
        return;
    }

    header.free_space_end = offset;

    // Actualiza el directorio de ranuras
    header.slot_directory_end -= sizeof(int) * 2;
    header.slots.push_back({offset, record_size});

    // Escribe el registro en el bloque
    std::vector<char> block_data = readBlock(block_index);
    std::copy(record.begin(), record.end(), block_data.begin() + offset);
    writeBlock(block_index, block_data);

    // Actualiza el encabezado
    header.record_count++;
    updateFreeSpaceMap(block_index, false);
}

// Elimina registro de longitud variable
void DiskManager::deleteVariableLengthRecord(int block_index, int record_index) {
    if (block_index < 0 || block_index >= disk.getNumBlocks()) {
        std::cerr << "Error: Bloque inválido para la eliminación." << std::endl;
        return;
    }

    auto& header = variable_block_headers[block_index];
    if (record_index < 0 || record_index >= header.slots.size()) {
        std::cerr << "Error: Índice de registro inválido." << std::endl;
        return;
    }

    // Marca el registro como eliminado en las ranuras
    auto& slot = header.slots[record_index];
    if (slot.first == -1) {
        std::cerr << "Error: El registro ya fue eliminado." << std::endl;
        return;
    }

    // Elimina el registro
    slot.first = -1;
    slot.second = 0;
    header.record_count--;

    // Actualiza el mapa de espacio libre
    updateFreeSpaceMap(block_index, false);

    std::cout << "Registro variable eliminado correctamente del bloque " << block_index
              << " en la posición " << record_index << "." << std::endl;
}

// Encuentra un bloque libre para la insercion
int DiskManager::findFreeBlock(int record_size, bool is_fixed) {
    const auto& free_space_map = is_fixed ? fixed_free_space_map : variable_free_space_map;

    for (const auto& [block_index, free_space] : free_space_map) {
        if (free_space >= record_size) {
            return block_index;
        }
    }
    return -1;
}

// Carga los headers de los bloques del disco
void DiskManager::loadHeaders(bool is_fixed) {
    for (int i = 0; i < disk.getNumBlocks(); i++) {
        std::vector<char> block_data = readBlock(i);

        if (is_fixed) {
            // Carga el encabezado para registros de longitud fija
            FixedBlockHeader header;
            std::memcpy(&header.record_count, block_data.data(), sizeof(int));
            std::memcpy(&header.free_space_end, block_data.data() + sizeof(int), sizeof(int));

            int free_list_size = (block_size - header.free_space_end) / sizeof(int);
            header.free_list.resize(free_list_size);
            std::memcpy(header.free_list.data(), block_data.data() + header.free_space_end, free_list_size * sizeof(int));

            fixed_block_headers[i] = header;
        } else {
            // Carga el encabezado para registros de longitud variable
            VariableBlockHeader header;
            std::memcpy(&header.record_count, block_data.data(), sizeof(int));
            std::memcpy(&header.free_space_end, block_data.data() + sizeof(int), sizeof(int));
            std::memcpy(&header.slot_directory_end, block_data.data() + 2 * sizeof(int), sizeof(int));

            int slot_count = header.record_count;
            for (int j = 0; j < slot_count; ++j) {
                int offset, size;
                std::memcpy(&offset, block_data.data() + header.slot_directory_end + j * sizeof(int) * 2, sizeof(int));
                std::memcpy(&size, block_data.data() + header.slot_directory_end + j * sizeof(int) * 2 + sizeof(int), sizeof(int));
                header.slots.emplace_back(offset, size);
            }

            variable_block_headers[i] = header;
        }
    }

    std::cout << (is_fixed ? "Encabezados de longitud fija" : "Encabezados de longitud variable")
              << " cargados exitosamente." << std::endl;
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
int DiskManager::findSlotInBlock(int block_index, int record_size, bool is_fixed) {
    if (is_fixed) {
        // Para registros de longitud fija
        auto& header = fixed_block_headers[block_index];
        if (header.free_space_end < record_size) {
            return -1; // No hay espacio suficiente
        }
        int offset = header.free_space_end - record_size;
        header.free_space_end = offset;
        return offset;
    } else {
        // Para registros de longitud variable (slotted page structure)
        auto& header = variable_block_headers[block_index];
        if (header.free_space_end < record_size + sizeof(int) * 2) {
            return -1; // No hay espacio suficiente para el registro y su slot
        }
        // Ubicación del nuevo registro al final del área de datos libres
        int offset = header.free_space_end - record_size;
        header.free_space_end = offset;

        // Agrega un nuevo slot en el directorio de slots
        header.slots.emplace_back(offset, record_size);
        header.slot_directory_end -= sizeof(int) * 2; // Espacio reservado para el slot

        return offset;
    }


// Actualiza el mapa de espacio libre después de la inserción o eliminación
void DiskManager::updateFreeSpaceMap(int block_index, bool is_fixed) {
    auto& free_space_map = is_fixed ? fixed_free_space_map : variable_free_space_map;
    if (is_fixed) {
        auto& header = fixed_block_headers[block_index];
        int header_space = sizeof(int) * 2 + header.free_list.size() * sizeof(int); // Espacio usado por el encabezado
        int used_space = header_space + header.record_count * (block_size / header.record_count); // Tamaño fijo por registro
        free_space_map[block_index] = block_size - used_space;
    } else {
        auto& header = variable_block_headers[block_index];
        int header_space = sizeof(int) * 3 + header.slots.size() * sizeof(std::pair<int, int>); // Espacio usado por el encabezado y ranuras
        int used_space = header_space + std::accumulate(header.slots.begin(), header.slots.end(), 0,
                                                         [](int sum, const std::pair<int, int>& record) {
                                                             return sum + record.second; // Tamaño del registro
                                                         });

    free_space_map[block_index] = block_size - used_space;
}