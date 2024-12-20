#include "Disk.h"
#include <filesystem>

Disk::Disk() {

    this->numPlatters = 2;
    this->tracksPerSurface = 8;
    this->sectorsPerTrack = 4;
    this->bytesPerSector = 4096;

    this->bytesPerBlock = 4 * 1024; //4KB
    this->sectorPerBlock = bytesPerBlock / bytesPerSector;
}

Disk::Disk(int numPlatters, int tracksPerSurface, int sectorsPerTrack, int bytesPerSector, int bytesPerBlock) {
    this->numPlatters = numPlatters;
    this->tracksPerSurface = tracksPerSurface;
    this->sectorsPerTrack = sectorsPerTrack;
    this->bytesPerSector = bytesPerSector;

    this->bytesPerBlock = bytesPerBlock;
    this->path = path;

    this->sectorPerBlock = bytesPerBlock / bytesPerSector;
}

Disk::~Disk() {}

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

int Disk::getBytesPerBlock() {
    return this->bytesPerBlock;
}

int Disk::getSectorPerBlock() {
    return sectorPerBlock;
}

int Disk::getTotalBlocks() {
    return getTotalSectors() / getSectorPerBlock();
}

int Disk::getTotalSectors() {
    int total = this->numPlatters * 2 * this->tracksPerSurface * this->sectorsPerTrack;
    return total;
}

std::string Disk::getPath() {
    return this->path;
}
