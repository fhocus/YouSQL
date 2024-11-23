#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "../Disk/Disk.h"

class Database
{
public:
  Database(std::string path);
  ~Database();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
};

#include "Database.cpp"

#endif