#include "../logger/Logger.hpp"
#include <iostream>

int main()
{
  VERBOSE << "Hello World!" << std::endl;
  DEBUG << "Hello" << " World" << "!" << std::endl;
  return 0;
}
