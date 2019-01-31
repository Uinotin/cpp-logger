#include "Logger.hpp"

#include <iostream>

class Logger::Impl
{
public:
  void foo()
  {
  }
private:
  int asdf;
};

Logger::Logger()
  : m_impl(new Impl)
{
  std::cout << "Initialised logger" << std::endl;
}

Logger::~Logger() = default;

Logger::Logger(Logger&&) = default;
Logger& Logger::operator=(Logger&&) = default;
