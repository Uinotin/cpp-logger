#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <memory>
#include <experimental/propagate_const>

class Logger
{
public:
  Logger();
  ~Logger();
  Logger(Logger&&);
  Logger& operator=(Logger&&);
private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};
#endif /// LOGGER_HPP
