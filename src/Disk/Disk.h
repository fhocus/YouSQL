#ifndef DISK_H
#define DISK_H

#include <string>
#include <fstream>
#include <iostream>

class Disk
{
public:
  Disk();
  Disk(int numPlatters, int tracksPerSurface, int sectorsPerTrack,
       int bytesPerSector, int bytesPerBlocks);
  ~Disk();
  int getNumPlatters();
  int getTracksPerSurface();
  int getSectorsPerTrack();
  int getBytesPerSector();

  int getSectorPerBlock();
  int getBytesPerBlock();
  int getTotalSectors();
  int getTotalBlocks();
  std::string getPath();

private:
  std::string path = "../db";

  int numPlatters;
  int tracksPerSurface;
  int sectorsPerTrack;
  int bytesPerSector;
  int bytesPerBlock;
  int blockPerTrack;
  int sectorPerBlock;
};

#endif