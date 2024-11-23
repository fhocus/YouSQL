#ifndef DISK_H
#define DISK_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Disk
{
public:
  Disk(std::string path);
  ~Disk();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
};

#include "Disk.cpp"

#endif