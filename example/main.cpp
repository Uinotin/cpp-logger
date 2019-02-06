#include "../logger/Logger.hpp"
#include <iostream>



int main()
{
  for (int i = 0; i < 255; ++i)
    VERBOSE << "Hello World! " << i << std::endl;
  DEBUG << "Hello" << " World" << "!" << std::endl;
  STOPLOGGER
  return 0;
}
