#include "DiskManager.h"

void removeLine(const std::string& inFile, const std::string& outFile, int numline); 
void updateHeader(const std::string& file, const sectorHeader& header);
std::string removeFirstElem(const std::string& line);

DiskManager::DiskManager() {
    this->disk = Disk();
}

DiskManager::DiskManager(bool isVariable, int numPlatters, int tracksPerSurface, int sectorsPerTrack, int bytesPerSector, int bytesPerBlock) {
    this->disk = Disk(numPlatters, tracksPerSurface, sectorsPerTrack, bytesPerSector, bytesPerBlock);
    this->isVariable = isVariable;
    this->dataTypeSize = nullptr;
    this->freeSpaceBegin = nullptr;
    this->freeSpaceEnd = nullptr;
    this->fullSpaceBegin = nullptr;
    this->fullSpaceEnd = nullptr;

    this->currPlatter = 1;
    this->currSurface = 'A';
    this->currTrack = 1;
    this->currSector = 1;
    this->currBlock = 1;
}
DiskManager::~DiskManager() {}

void DiskManager::currentLocationCheck() {
    if (this->currSector > disk.getTotalSectors()) {
            this->currSector = 1;
            this->currTrack++;
            if (this->currTrack > disk.getTracksPerSurface()) {
                this->currTrack = 1;
                this->currSurface = (this->currSurface == 'A') ? 'B' : 'A';
                if (this->currSurface == 'A') {
                    this->currPlatter++;
                    if (this->currPlatter > disk.getNumPlatters())
                        this->currPlatter = 1;
                }
            }
        }
}

void DiskManager::createDiskStructure() {

    std::string carpetaSectors = (PATH / "/Sectors").string();   
    fs::remove_all(carpetaSectors); 
    fs::create_directories(carpetaSectors); // Crea la carpeta directorios

    for (int i = 0; i < disk.getTotalSectors(); i++) {
        std::string fileSector = carpetaSectors + "/" + std::to_string(this->currPlatter) + "-" + this->currSurface + "-" + std::to_string(this->currTrack) + "-" + std::to_string(this->currSector) + ".txt";
        std::ofstream file(fileSector);
        file << this->currSector << "#" << disk.getBytesPerSector() << "#0#" << std::endl;
        file.close();

        this->currSector++;
        
        currentLocationCheck();
    }

    this->currPlatter = 1;
    this->currSurface = 'A';
    this->currTrack = 1;
    this->currSector = 1;

    int sizeBlock;
    sizeBlock = disk.getBytesPerBlock();;

    //Crear los files de bloques
    std::string carpetaBloques = (PATH / "/Blocks").string() ;
    fs::remove_all(carpetaBloques);
    fs::create_directories(carpetaBloques);

    for (int bloque = 1; bloque <= disk.getTotalBlocks(); bloque++)
    {
        std::string blockFile = carpetaBloques + "/block" + std::to_string(bloque) + ".txt";
        std::ofstream file(blockFile);
        if (!file.is_open()) {
            std::cerr << "Error al crear el file " << blockFile << '\n';
        }
        
        std::string contenido = std::to_string(bloque) + "#" + std::to_string(disk.getBytesPerBlock()) + "##";
        file << contenido << std::endl;

        std::unordered_map<int, std::tuple<int, char, int, bool>> sectors;
        for (int sector = 1; sector <= disk.getSectorPerBlock(); sector++) {
            sectors[this->currSector] = std::make_tuple(this->currPlatter, this->currSurface, this->currTrack, false);

            this->currSector++;

            currentLocationCheck();
        }
        insertBlocktoFreeHeapFile(bloque, sizeBlock, sectors);

        file.close();
    }

    this->currPlatter = 1;
    this->currSurface = 'A';
    this->currTrack = 1;
    this->currSector = 1;

    std::cout << "La estructura del disk ha sido creada exitosamente." << std::endl;
}

std::vector<std::string> DiskManager::blockToVector(int numBlock) {
    std::vector<std::string> registros;
    std::string carpetaBloque = (PATH / "/Blocks/").string();
    std::string blockFile = carpetaBloque + std::string("block") + std::to_string(numBlock) + ".txt";
    std::ifstream file(blockFile);

    if (!file.is_open()) {
        std::cerr << "Error al abrir el file: " << blockFile << std::endl;
        return registros;
    }

    std::string line;
    std::getline(file, line); // Ignorar la cabecera
    while (std::getline(file, line)) {
        registros.push_back(line);
    }

    file.close();
    return registros;
}

void DiskManager::insertBlocktoFreeHeapFile(int numBlock, int freeSpace, const std::unordered_map<int, std::tuple<int, char, int, bool>>& sectors) {
    Node* newNode = new Node(numBlock, freeSpace);
    newNode->sectors = sectors;

    if (!this->freeSpaceBegin) {
        this->freeSpaceBegin = this->freeSpaceEnd = newNode;
        this->currBlock = this->freeSpaceBegin->numBlock;
        auto it = this->freeSpaceBegin->sectors.begin();
        this->currSector = it->first;
        if (it != this->freeSpaceBegin->sectors.end()) {
            this->currPlatter = std::get<0>(it->second);
            this->currSurface = std::get<1>(it->second);
            this->currTrack = std::get<2>(it->second);
        }
    } else {
        this->freeSpaceEnd->next = newNode;
        newNode->prev = this->freeSpaceEnd;
        this->freeSpaceEnd = newNode;
    }
}

void DiskManager::insertBlocktoFullHeapFile(int numBlock, int freeSpace, const std::unordered_map<int, std::tuple<int, char, int, bool>>& sectors) {
    Node* newNode = new Node(numBlock, freeSpace);
    newNode->sectors = sectors;

    if (!this->fullSpaceBegin) {
        this->fullSpaceBegin = this->fullSpaceEnd = newNode;
    } else {
        this->fullSpaceEnd->next = newNode;
        newNode->prev = this->fullSpaceEnd;
        this->freeSpaceEnd = newNode;
    }
}


Node* DiskManager::searchFreeSpace(int numBlock) {
    Node* actual = this->freeSpaceBegin;
    while (actual) {
        std::cout << "--> Visitando bloque " << actual->numBlock << " en heap file (Free)" << std::endl;
        if (actual->numBlock == numBlock) {
            return actual;
        }
        actual = actual->next;
    }
    return nullptr;  // Devuelve nullptr si no encuentra el bloque
}

Node* DiskManager::searchFullSpace(int numBlock) {
    Node* actual = this->fullSpaceBegin;
    while (actual) {
        std::cout << "Visitando bloque " << actual->numBlock << " en heap file (Full)" << std::endl;
        if (actual->numBlock == numBlock) {
            return actual;
        }
        actual = actual->next;
    }
    return nullptr;  // Devuelve nullptr si no encuentra el bloque
}

Node* DiskManager::searchBlockHeapFile(int numBlock) {
    Node* node = searchFreeSpace(numBlock);
    if (node) {
        return node;
    }
    
    node = searchFullSpace(numBlock);
    if (node) {
        return node;
    }

    std::cout << "Bloque con numero " << numBlock << " no encontrado." << std::endl;
    return nullptr;  // Devuelve nullptr si no encuentra el bloque
}

void DiskManager::decreaseSpaceofBlock(int numBlock) {
    Node* block = searchBlockHeapFile(numBlock);

    if (!block) {
        std::cout << "Bloque con numero " << numBlock << " no encontrado." << std::endl;
        return;
    }

    block->freeSpace = block->freeSpace - recordLength;

    std::cout << " -- Bloque " << block->numBlock << " con espacio libre reducido a " << block->freeSpace << std::endl;
    
    if(block->freeSpace < recordLength) {
        moveBlockFreeToFull(block);
        this->currBlock = this->freeSpaceBegin->numBlock;
        auto it = this->freeSpaceBegin->sectors.begin();
        this->currSector = it->first;
        if (it != this->freeSpaceBegin->sectors.end()) {
            this->currPlatter = std::get<0>(it->second);
            this->currSurface = std::get<1>(it->second);
            this->currTrack = std::get<2>(it->second);
        }
    }
}

void DiskManager::increaseSpaceofBlock(int numBlock) {
    Node* block = searchBlockHeapFile(numBlock);

    if (!block) {
        std::cout << "Bloque con numero " << numBlock << " no encontrado." << std::endl;
        return;
    }

    block->freeSpace = block->freeSpace + recordLength;
    std::cout << "++ Bloque " << block->numBlock << " con espacio libre incrementado a " << block->freeSpace << std::endl;
    
    if(searchFullSpace(block->numBlock) && block->freeSpace >= recordLength){
        moveBlockFullToFree(block);
        this->currBlock = this->freeSpaceBegin->numBlock;
        auto it = this->freeSpaceBegin->sectors.begin();
        this->currSector = it->first;
        if (it != this->freeSpaceBegin->sectors.end()) {
            this->currPlatter = std::get<0>(it->second);
            this->currSurface = std::get<1>(it->second);
            this->currTrack = std::get<2>(it->second);
        }
    }
}

void DiskManager::deleteBlockHeapFile(Node*& head, Node*& tail, Node* node) {
    if (!node) return;

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        tail = node->prev;
    }

    node->prev = nullptr;
    node->next = nullptr;
}

void DiskManager::moveBlockFreeToFull(Node* node) {
    deleteBlockHeapFile(this->freeSpaceBegin, this->freeSpaceEnd, node);

    // Insertar node al final de FullSpace
    if (!fullSpaceBegin) {
        fullSpaceBegin = fullSpaceEnd = node;
    } else {
        fullSpaceEnd->next = node;
        node->prev = fullSpaceEnd;
        fullSpaceEnd = node;
    }

    std::cout << "El bloque " << node->numBlock << " fue movido de la lista FREE a FULL" << std::endl;

    node->next = nullptr;
}

void DiskManager::moveBlockFullToFree(Node* node) {

    deleteBlockHeapFile(this->fullSpaceBegin, this->fullSpaceEnd, node);

    if (!this->freeSpaceBegin) {
        this->freeSpaceBegin = this->freeSpaceEnd = node;
    } else {
        node->next = this->freeSpaceBegin;
        this->freeSpaceBegin->prev = node;
        this->freeSpaceBegin = node;
    }

    std::cout << "El bloque " << node->numBlock << " fue movido de la lista FULL a FREE" << std::endl;

    node->prev = nullptr;
}

void DiskManager::emptyHeapFile(Node*& head, Node*& tail) {
    Node* actual = head;
    while (actual) {
        Node* siguiente = actual->next;
        delete actual;
        actual = siguiente;
    }
    head = nullptr;
    tail = nullptr;
}

void DiskManager::emptyHeapFile() {
    emptyHeapFile(this->freeSpaceBegin, this->freeSpaceEnd);
    emptyHeapFile(this->fullSpaceBegin, this->fullSpaceEnd);
}

void DiskManager::saveHeapFile() {
    std::ofstream file((PATH / "heapFile.txt").string());

    if (!file.is_open()) {
        std::cerr << "Error al abrir el file." << std::endl;
        return;
    }

    saveInformationInFile(this->freeSpaceBegin, file);
    saveInformationInFile(this->fullSpaceBegin, file);

    file.close();
}

void DiskManager::saveInformationInFile(Node* head, std::ofstream& file) {
    Node* current = head;
    while (current) {
        file << current->numBlock << " " << current->freeSpace << " ";

        for (const auto& [idSector, tupla] : current->sectors) {
            auto [platter, surface, track, state] = tupla;
            file << idSector << ":" << platter << ":" << surface << ":" << track << ",";
        }

        file << std::endl;
        current = current->next;
    }
}

void DiskManager::recoverInformationFromHeapFile() {
    std::ifstream file((PATH / "heapFile.txt").string());

    if (!file.is_open()) {
        std::cerr << "Error al abrir el file." << std::endl;
        return;
    }

    emptyHeapFile(); // Eliminar nodes existentes antes de recuperar la información

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char lista;
        int numBlock, freeSpace;
        char separator;
        std::unordered_map<int, std::tuple<int, char, int, bool>> sectors;

        iss >> lista >> numBlock >> freeSpace;

        while (iss >> numBlock >> separator) {
            int platter, surface, track;
            char separator2;
            bool state;
            iss >> platter >> separator2 >> surface >> separator2 >> track >> separator2 >> state >> separator;
            sectors[numBlock] = std::make_tuple(platter, surface, track, state);
        }

        if (lista == 'F') {
            insertBlocktoFreeHeapFile(numBlock, freeSpace, sectors); // Inserta en FreeSpace
        } else if (lista == 'S') {
            insertBlocktoFullHeapFile(numBlock, freeSpace, sectors); // Inserta en FullSpace
        }
    }

    file.close();
}


void DiskManager::setSchemeSize(const std::string& scheme) {
    std::stringstream ss(scheme);
    std::string campo;
    std::string type;

    int size = 0;
    int i = 0;

    std::stringstream ssCount(scheme);
    std::string temp;
    int count = 0;
    std::getline(ssCount, temp, '#');

    while (std::getline(ssCount, temp, '#')) {
        count++;
    }
    count /= 2; 

    dataTypeSize = new int[count];
    
    std::getline(ss, temp, '#');
    while (std::getline(ss, campo, '#') && std::getline(ss, type, '#')) {
        if (type == "int") {
            dataTypeSize[i] = sizeof(int);
            size += sizeof(int);
        } else if (type == "char") {
            dataTypeSize[i] = sizeof(char);
            size += sizeof(char);
        } else if (type == "float") {
            dataTypeSize[i] = sizeof(float);
            size += sizeof(float);
        } else if (type == "double") {
            dataTypeSize[i] = sizeof(double);
            size += sizeof(double);
        } else if (type == "bool") {
            dataTypeSize[i] = sizeof(bool);
            size += sizeof(bool);
        } else if (type.substr(0, 7) == "varchar") {
            size_t pos1 = type.find('(');
            size_t pos2 = type.find(')');
            if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1 + 1) {
                int length = std::stoi(type.substr(pos1 + 1, pos2 - pos1 - 1));
                dataTypeSize[i] = length * sizeof(char);
                size += length * sizeof(char);
            }
        }
        ++i;
    }

    std::cout << "Tamanho del registro: " << size << std::endl;
    setRecordLength(size);
    
    for (int j = 0; j < i; ++j) {
        std::cout << "Type size at index " << j << ": " << dataTypeSize[j] << " bytes\n";
    }
}

void DiskManager::setRecordLength(int longitud) {
    this->recordLength = longitud;
}

void DiskManager::useFixedLength(std::string linefile) {
    std::string attribute;
    std::string record;
    bool is_string = false;
    int i = 0;

    for (char c : linefile) {
        if (c == '"') {
            is_string = !is_string;
        } else if (c == ',') {
            if (is_string) {
                attribute.push_back(c);
            } else {
                if (!attribute.empty()) {
                    record += attribute;
                }
                int tamañoSobrante = dataTypeSize[i] - attribute.length();

                for (int j = 0; j < tamañoSobrante; j++) {
                    record += " ";
                }

                attribute.clear();

                i++;
            }
        } else {
            attribute.push_back(c);
        }
    }
    record += attribute;

    bool currSectorualizado = false;
    Node* block = this->freeSpaceBegin;

    do {
        std::string blockFile = (PATH / "/Blocks/block").string() + std::to_string(block->numBlock) + std::string(".txt");
        std::ifstream fileReadBloque(blockFile, std::ios::app);

        if (!fileReadBloque) {
            std::cerr << "Error al abrir el file: " << blockFile << std::endl;
            return;
        }

        std::string line;
        int location = 0;
        std::string espaciosLibres = "";

        std::getline(fileReadBloque, line);
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, '#'); // Ignorar el identificador
        std::getline(ss, token, '#');
        std::cout << "*** Bloque: " << block->numBlock << "; Espacio Disponible: " << block->freeSpace << std::endl;

        if (block->freeSpace >= recordLength) {
            std::getline(ss, token, '#');
            if (!token.empty()) {
                espaciosLibres = removeFirstElem(token);
                std::stringstream ssToken(token);
                std::getline(ssToken, token, ',');
                location = std::stoi(token) + 1;
            } else {
                location = 0;
            }
            std::cout << "\tEspacio libre entre registros: " << location << std::endl;
        } 

        fileReadBloque.close();

        if (location == 0) {
            std::ofstream fileWriteSector(blockFile, std::ios::app);
            if (!fileWriteSector) {
                std::cerr << "Error al abrir el file para escritura: " << blockFile << std::endl;
                return;
            }

            fileWriteSector << record << std::endl;
            fileWriteSector.close();
        } else {
            updateLineFL(blockFile, record, location);           
        }

        decreaseSpaceofBlock(block->numBlock);

        sectorHeader newHeader;
        newHeader.id = block->numBlock;
        newHeader.availableSpace = block->freeSpace;
        newHeader.freeSpaces = espaciosLibres;
        updateHeader(blockFile, newHeader);

        currSectorualizado = false;
        
        sectorInsertFL(record, location, block);

    } while (currSectorualizado);  
}

void DiskManager::sectorInsertFL(const std::string& linefile, int location, Node*& bloque) {
    bool state = true;

    for (const auto& par : bloque->sectors) {
        const auto& [idSector, tupla] = par;
        state = std::get<3>(tupla);
        if (!state) {
            this->currPlatter = std::get<0>(tupla);
            this->currSurface = std::get<1>(tupla);
            this->currTrack = std::get<2>(tupla);
            this->currSector = idSector;
            std::cout << "platter Actual: " << this->currPlatter << "; surface Actual: " << this->currSurface << "; track Actual: " << this->currTrack << "; Sector Actual: " << this->currSector << std::endl;
            break; // Se encontró un sector con state false, se imprime y se sale de la función
        }
        
    }
    if(state) return;
    
    std::string fileSector = (PATH / "/Sectors/").string() + std::to_string(this->currPlatter) + "-" + this->currSurface + "-" + std::to_string(this->currTrack) + "-" + std::to_string(this->currSector) + ".txt";
    std::ifstream fileReadSector(fileSector);

    if (!fileReadSector) {
        std::cerr << "Error al abrir el file: " << fileSector << std::endl;
        return;
    }

    std::string line;
    int espacioDisponible = 0;
    int numRecords = 0;
    std::string espaciosLibres;

    std::getline(fileReadSector, line);
    std::stringstream ss(line);
    std::string token;


    std::getline(ss, token, '#'); // Ignorar el identificador
    std::getline(ss, token, '#');
    espacioDisponible = std::stoi(token);
    std::getline(ss, token, '#');
    numRecords = std::stoi(token);
    std::cout << "Espacio Disponible de Sector: " << espacioDisponible << std::endl;

    fileReadSector.close();

    if (espacioDisponible >= recordLength) {

        if (!location) {
            std::ofstream fileWriteSector(fileSector, std::ios::app);
            if (!fileWriteSector) {
                std::cerr << "Error al abrir el file para escritura: " << fileSector << std::endl;
                return;
            }

            fileWriteSector << linefile << std::endl;
            fileWriteSector.close();
        } else {
            updateLineFL(fileSector, linefile, location);           
        }

        numRecords++;
        espacioDisponible -= recordLength;

        std::cout << "SECTOR -- espacio disponible: " << espacioDisponible << std::endl;

        sectorHeader newHeader;
        newHeader.id = this->currSector;
        newHeader.availableSpace = espacioDisponible;
        newHeader.numRecords = numRecords;
        updateHeader(fileSector, newHeader);
    }

    if(espacioDisponible < recordLength) {
        updateSectorFL();
    }
}

void DiskManager::updateLineFL(const std::string& fileSector, const std::string& nuevaline, int numRegistro) {
    std::ifstream fileLeer(fileSector);
    std::ofstream fileTemporal("temp.txt");

    if (!fileLeer) {
        std::cerr << "Error al abrir el file para lectura: " << fileSector << std::endl;
        return;
    }
    if (!fileTemporal) {
        std::cerr << "Error al crear file temporal" << std::endl;
        fileLeer.close();
        return;
    }

    std::string line;
    int numeroline = 1;

    while (std::getline(fileLeer, line)) {
        std::cout << numeroline << " -- ";
        std::cout << line << std::endl;
        if (numeroline == numRegistro) {
            fileTemporal << nuevaline << std::endl;
        } else {
            fileTemporal << line << std::endl;
        }
        numeroline++;
    }

    fileLeer.close();
    fileTemporal.close();

    if (std::remove(fileSector.c_str()) != 0) {
        std::cerr << "Error al eliminar el file original: " << fileSector << std::endl;
    }
    if (std::rename("temp.txt", fileSector.c_str()) != 0) {
        std::cerr << "Error al renombrar file temporal a original: " << fileSector << std::endl;
    }
}

void DiskManager::updateSectorFL() {
    this->currSector++;
    std::cout << "Actualizando al siguiente sector: " << currSector << std::endl;

    currentLocationCheck();
}


void removeLine(const std::string& fileEntrada, const std::string& fileSalida, int numlineEliminar) {
    std::ifstream fileLectura(fileEntrada);
    if (!fileLectura) {
        std::cerr << "No se pudo abrir el file para leer: " << fileEntrada << std::endl;
        return;
    }

    std::ofstream fileEscritura(fileSalida);
    if (!fileEscritura) {
        std::cerr << "No se pudo abrir el file para escribir: " << fileSalida << std::endl;
        return;
    }

    std::string line;
    int numeroline = 0;
    while (std::getline(fileLectura, line)) {
        numeroline++;
        if (numeroline != numlineEliminar) {
            fileEscritura << line << std::endl;
        }
    }

    fileLectura.close();
    fileEscritura.close();

    // Renombrar el file de salida al nombre del file original
    std::rename(fileSalida.c_str(), fileEntrada.c_str());
}

void updateHeader(const std::string& file, const sectorHeader& newHeader) {
    std::string fileTemporal = "temporal.txt";
    std::ifstream fileRead(file);
    if (!fileRead) {
        std::cerr << "No se pudo abrir el file para leer: " << file << std::endl;
        return;
    }

    std::ofstream fileTempWrite(fileTemporal);
    if (!fileTempWrite) {
        std::cerr << "No se pudo abrir el file temporal para escribir: " << fileTemporal << std::endl;
        return;
    }

    std::string line;
    if (std::getline(fileRead, line)) { } // Cabecera original leída y descartada

    // Leer el contenido original del file y escribirlo al file temporal
    while (std::getline(fileRead, line)) {
        fileTempWrite << line << std::endl;
    }

    fileRead.close();
    fileTempWrite.close();

    // Abrir el file original en modo de escritura para reemplazar la cabecera
    std::ofstream fileWrite(file);
    if (!fileWrite) {
        std::cerr << "No se pudo abrir el file para escribir: " << file << std::endl;
        return;
    }

    // Escribir la nueva cabecera en el file original
    fileWrite << newHeader.toString() << std::endl;

    // Leer el contenido del file temporal y escribirlo al file original
    std::ifstream fileTempRead(fileTemporal);
    if (!fileTempRead) {
        std::cerr << "No se pudo abrir el file temporal para leer: " << fileTemporal << std::endl;
        return;
    }

    while (std::getline(fileTempRead, line)) {
        fileWrite << line << std::endl;
    }

    fileTempRead.close();
    fileWrite.close();

    std::remove(fileTemporal.c_str());
}

std::string removeFirstElem(const std::string& cadena) {
    std::cout << cadena << std::endl;
    size_t pos = cadena.find(',');
    if (pos == std::string::npos) {
        return "";
    }
    return cadena.substr(pos + 1);
}