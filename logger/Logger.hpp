#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <memory>
#include <iostream>
#include <sstream>

class Logger
{
public:
  static Logger * Instance();
  Logger & operator << (std::ostream& data(std::ostream&))
  {
    m_stream << data;
    return *this;
  }
  template<typename T> Logger & operator << (const T& data)
  {
    m_stream << data;
    return *this;
  }

private:
  Logger();
  ~Logger();

  class Impl;
  std::unique_ptr<Impl> m_impl;
  std::stringstream m_stream;
  static Logger * m_instance;
};

class NullLogger
{
public:
  NullLogger(void) {}
  inline NullLogger operator<<(std::ostream (*)(std::ostream&)) {
    return *this;
  }
  template <typename T>
  inline NullLogger operator<<(const T&) {
    return *this;
  }
};
static NullLogger s_nullLogger;

#define VERBOSE (*(Logger::Instance()) << "Verbose: ")
#define INFO (*(Logger::Instance()) << "Info: ")
#ifdef _DEBUG
#define DEBUG (*(Logger::Instance()) << "Debug: ")
#else
#define DEBUG s_nullLogger
#endif
#define ERROR (*(Logger::Instance()) << "Error: ")
#define FATAL (*(Logger::Instance()) << "FATAL: ")


#endif /// LOGGER_HPP
