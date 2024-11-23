#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class BufferPool
{
public:
  BufferPool(std::string path);
  ~BufferPool();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
};

#endif