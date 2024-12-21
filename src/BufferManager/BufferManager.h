#pragma once
#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <cassert>

#include "BufferPool.h"

class BufferManager
{
public:
  BufferManager(int, int);
  ~BufferManager();

  void requestPage(int, char, std::vector<std::string>);
  void releasePage(int);
  void pinPage(int);
  void savePagetoDisk(int);

private:
  BufferPool bufferPool;
  std::unordered_map<int, int> pageTable;
};

#endif