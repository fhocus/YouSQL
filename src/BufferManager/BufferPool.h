#pragma once
#ifndef BUFFERPOOL_H
#define BUFFERPOOL_H
#include "frame.h"

#include <vector>

class BufferPool
{
public:
  BufferPool(int, int);
  ~BufferPool();

  int findUnpinnedFrame();
  void fillFrame(std::vector<std::string>, int);
  void pinFrame(int);
  void unpinFrame(int);
  void incrementFrame(int);
  void decrementFrame(int);
  void resetFrame(int);
  void dirtyFrame(int);
  void resetBuffer();
  bool areAllFramesUsed();
  void updateIndex();

  std::vector<Frame> getBuffer();
  Frame* getFrame(int);
  int getBufferSize(); 
  int getcurrIndex(); 

  int CLOCK();
private:
  std::vector<Frame> buffer;
  int currIndex; 
  int id;
};

#endif