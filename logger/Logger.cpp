#include "Logger.hpp"

#include <iostream>
#include <sstream>
#include <thread>

Logger * Logger::m_instance = nullptr;

class Logger::Impl
{
public:
  Impl()
  {}
private:
};

Logger * Logger::Instance()
{
  if (!m_instance)
    m_instance = new Logger;
  return m_instance;
}

Logger::Logger()
  : m_impl(new Impl)
{
  std::cout << "Initialised logger" << std::endl;
}

Logger::~Logger() = default;
