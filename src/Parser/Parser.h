#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Parser
{
public:
  Parser(std::string path);
  ~Parser();

  void read(std::string path);
  void write(std::string path);

private:
  std::string path;
  std::ifstream file;
  std::ofstream fileOut;
};

#endif