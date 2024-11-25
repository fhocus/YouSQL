#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "Disk.h"

struct BlockHeader {
    int record_count;       
    int free_space_end;     
    std::vector<int> record_offsets; 
};

class DiskManager {
public:
    DiskManager(Disk& disk);
    ~DiskManager();

    // Fixed-length record 
    void insertFixedLengthRecord(const std::string& record, int record_size);
    void deleteFixedLengthRecord(int block_index, int record_index);

    // Variable-length record 
    void insertVariableLengthRecord(const std::string& record);
    void deleteVariableLengthRecord(int block_index, int record_index);

    // HeapFile 
    void initializeHeapFile();
    int findFreeBlock(int record_size);

private:
    Disk disk;
    std::vector<BlockHeader> block_headers; 
    std::unordered_map<int, int> free_space_map; 
    int block_size;

    void loadHeaders();
    void updateFreeSpaceMap(int block_index);
    void writeBlock(int block_index, const std::vector<char>& block_data);
    std::vector<char> readBlock(int block_index);
    int findSlotInBlock(int block_index, int record_size);
};

#endif 
