#include "Disk.h"

Disk::Disk() {
    this->capacity = 10 * 1024 * 1024; //10MB
    this->block_capacity = 4 * 1024; //4KB
}

Disk::Disk(int capacity, int block_capacity) {
    this->capacity = capacity;
    this->block_capacity = block_capacity;
}

Disk::~Disk() {}