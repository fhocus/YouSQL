#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <tuple>
#include <list>
#include <optional>
#include <cstdio>
#include <iomanip>
#include <filesystem>

#include "Disk.h"

namespace fs = std::filesystem;

#define PATH fs::current_path() / "../db"

struct Node {
    int numBlock;      
    int freeSpace;     
    std::unordered_map<int, std::tuple<int, char, int, bool>> sectors; 

    Node* prev;
    Node* next;

    Node(int blockNumber, int freeSpace)
        : numBlock(blockNumber), freeSpace(freeSpace), prev(nullptr), next(nullptr) {}

    void addSector(int idSector, int platter, char surface, int track) {
        sectors[idSector] = std::make_tuple(platter, surface, track, false);
    }
};

struct sectorHeader {
    std::optional<int> id;
    std::optional<int> availableSpace;
    std::optional<std::string> freeSpaces;
    std::optional<int> numRecords; 

    std::string toString() const {
        std::string header;
        if(id.has_value()) {
            header += std::to_string(id.value()) + "#";
        }
        if(availableSpace.has_value()) {
            header += std::to_string(availableSpace.value()) + "#";
        }
        if(freeSpaces.has_value()) {
            header += freeSpaces.value() + "#";
        }
        if(numRecords.has_value()) {
            header += std::to_string(numRecords.value()) + "#";
        }
        return header;
    }
};

class DiskManager {
public:
    DiskManager();
    DiskManager(bool, int, int, int, int, int);
    ~DiskManager();

    void currentLocationCheck();
    void createDiskStructure();
    std::vector<std::string> blockToVector(int blockid);

    //heapfile
    void insertBlocktoFreeHeapFile(int, int, const std::unordered_map<int, std::tuple<int, char, int, bool>>&);
    void insertBlocktoFullHeapFile(int, int, const std::unordered_map<int, std::tuple<int, char, int, bool>>&);
    Node* searchFreeSpace(int);
    Node* searchFullSpace(int);
    Node* searchBlockHeapFile(int);
    void decreaseSpaceofBlock(int);
    void increaseSpaceofBlock(int);        
    void deleteBlockHeapFile(Node*&, Node*&, Node*);       
    void moveBlockFreeToFull(Node*);        
    void moveBlockFullToFree(Node*);        
    void emptyHeapFile(Node*&, Node*&); 
    void emptyHeapFile();
    void saveHeapFile();
    void saveInformationInFile(Node*, std::ofstream&);
    void recoverInformationFromHeapFile();

    //fixed-length
    void setSchemeSize(const std::string&);
    void setRecordLength(int);
    void useFixedLength(const std::string);
    void sectorInsertFL(const std::string&, int, Node*&);
    void updateLineFL(const std::string&, const std::string&, int);
    void updateSectorFL();

private:
    Disk disk;
    bool isVariable;
    int recordLength; //si es de longitud fija

    int currPlatter;
    char currSurface;
    int currTrack;
    int currSector;
    int currBlock;

    //heapfile
    Node* freeSpaceBegin;
    Node* freeSpaceEnd;
    Node* fullSpaceBegin;
    Node* fullSpaceEnd;

    int* dataTypeSize;
};

#endif 
