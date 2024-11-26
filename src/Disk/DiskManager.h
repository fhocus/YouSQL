#ifndef DISKMANAGER_H
#define DISKMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>

#include "Disk.h"

struct FixedBlockHeader {
    int record_count;      
    int free_space_end;     
    std::vector<int> free_list; 
};


struct VariableBlockHeader {
    int record_count;                       
    int free_space_end;    
    int slot_directory_end;               
    std::vector<std::pair<int, int>> slots;  // Tabla de ranuras <offset, tamaño>
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
    int findFreeBlock(int record_size, bool is_fixed);

private:
    Disk disk;
    std::vector<FixedBlockHeader> fixed_block_headers;  
    std::vector<VariableBlockHeader> variable_block_headers;   std::unordered_map<int, int> fixed_free_space_map;
    std::unordered_map<int, int> variable_free_space_map;
    int block_size;

    void loadHeaders(bool is_fixed);
    void updateFreeSpaceMap(int block_index, bool is_fixed);
    void writeBlock(int block_index, const std::vector<char>& block_data);
    std::vector<char> readBlock(int block_index);
    int findSlotInBlock(int block_index, int record_size, bool is_fixed);
};

#endif 
