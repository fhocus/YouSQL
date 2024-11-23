#ifndef DISK_H
#define DISK_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Disk
{
public:
  Disk();
  // Disk(std::string path);
  Disk(int capacity, int block_capacity);
  ~Disk();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
  int capacity; //bytes
  int block_capacity; //bytes
};

#include "Disk.cpp"

#endif