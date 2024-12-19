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
       int bytesPerSector, int bytesPerBlocks, std::string disk_name);
  ~Disk();
  int getNumPlatters();
  int getTracksPerSurface();
  int getSectorsPerTrack();
  int getBytesPerSector();

  int getSectorPerBlock();
  int getBytesPerBlock();
  int getBlockPerTrack();
  std::string getPath();

private:
  std::string path;

  int numPlatters;
  int tracksPerSurface;
  int sectorsPerTrack;
  int bytesPerSector;

  int sectorPerBlock;
  int bytesPerBlock;
  int blockPerTrack;
};

#endif