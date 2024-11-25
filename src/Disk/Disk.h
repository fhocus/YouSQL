#ifndef DISK_H
#define DISK_H

#include <string>
#include <fstream>
#include <iostream>

class Disk
{
public:
  Disk();
  // Disk(std::string path);
  Disk(int capacity, int block_capacity, std::string path);
  ~Disk();
  int getBlockCapacity();
  int getNumBlocks();
  std::string getBlockPath(int block_index);
  // void read(std::string path);
  // void write(std::string path);


private:
  std::string path;
  // std::ifstream file;
  // std::ofstream fileOut;
  int capacity; //bytes
  int block_capacity; //bytes
  int num_blocks;

  void create_blocks();

//   void updateFreeListHead(int newHead);
//   int allocateRecord();
//   void initializeFile();
};

#include "Disk.cpp"

#endif