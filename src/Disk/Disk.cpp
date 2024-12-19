#include "Disk.h"
#include <filesystem>

Disk::Disk() {
    this->path = "../db";

    this->numPlatters = 2;
    this->tracksPerSurface = 8;
    this->sectorsPerTrack = 4;
    this->bytesPerSector = 4096;

    this->bytesPerBlock = 4 * 1024; //4KB
    this->sectorPerBlock = this->bytesPerBlock / this->bytesPerSector;
    this->blockPerTrack = this->sectorsPerTrack / this->sectorPerBlock;
}

Disk::Disk(int numPlatters, int tracksPerSurface, int sectorsPerTrack, 
            int bytesPerSector, int bytesPerBlocks, std::string path = "../db") {
    this->numPlatters = numPlatters;
    this->tracksPerSurface = tracksPerSurface;
    this->sectorsPerTrack = sectorsPerTrack;
    this->bytesPerSector = bytesPerSector;

    this->bytesPerBlock = bytesPerBlock;
    this->path = path;


    this->sectorPerBlock = this->bytesPerBlock / this->bytesPerSector;
    this->blockPerTrack = this->sectorsPerTrack / this->sectorPerBlock;
}

Disk::~Disk() {}

int Disk::getBytesPerBlock() {
    return this->bytesPerBlock;
}

int Disk::getSectorPerBlock() {
    return this->sectorPerBlock;
}

int Disk::getNumPlatters() {
    return this->numPlatters;
}

int Disk::getTracksPerSurface() {
    return this->tracksPerSurface;
}

int Disk::getSectorsPerTrack() {
    return this->sectorsPerTrack;
}

int Disk::getBytesPerSector() {
    return this->bytesPerSector;
}   

int Disk::getBlockPerTrack() {
    return this->blockPerTrack;
}

std::string Disk::getPath() {
    return this->path;
}
