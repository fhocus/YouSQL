#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "BufferPool.h"

class BufferManager
{
public:
  BufferManager(std::string path);
  ~BufferManager();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
  BufferPool *bufferPool;
};

#endif