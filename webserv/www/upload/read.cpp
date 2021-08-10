#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

int main () {
  std::ifstream ifs;

  ifs.open ("404.html", std::ifstream::in);
  if (ifs.fail())
    std::cout << "ERROR" << std::endl;
    char buf[1024];
    ifs.getline(buf, 1024);

  while (ifs.good()) {
    std::cout << buf << std::endl;
    ifs.getline(buf, 1024);
}

  ifs.close();
  return 0;
