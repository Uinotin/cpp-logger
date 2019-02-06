#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <memory>
#include <iostream>
#include <sstream>
#include <mutex>
#include <vector>

class Writer
{
  friend class WriterExclusivePtr;
  friend class Logger;
public:
  Writer();
  ~Writer();
  Writer(const Writer&);

  Writer & operator << (std::ostream& data(std::ostream&))
  {
    m_stream << data;
    return *this;
  }
  template<typename T> Writer & operator << (const T& data)
  {
    m_stream << data;
    return *this;
  }

  std::mutex m_mutex;
private:
  std::stringstream m_stream;
};

/// Class around Writer that unlocks Writer's mutex once WriterExclusivePtr is deleted
class WriterExclusivePtr
{
  friend class Logger;
public:
  WriterExclusivePtr(WriterExclusivePtr && ptr)
    : m_ptr(ptr.m_ptr)
  {
    ptr.m_ptr = nullptr;
  }
  ~WriterExclusivePtr()
  {
    m_ptr->m_mutex.unlock();
  }
  Writer * operator ->()
  {
    return m_ptr;
  }
  Writer const * operator ->() const
  {
    return m_ptr;
  }
  Writer & operator << (std::ostream& data(std::ostream&))
  {
    return *m_ptr << data;
  }
  template<typename T> Writer & operator << (const T& data)
  {
    return (*m_ptr << data);
  }

private:
  WriterExclusivePtr(Writer *ptr)
    : m_ptr(ptr)
  {
  }
  WriterExclusivePtr(WriterExclusivePtr const &)
  {
  }
  WriterExclusivePtr & operator = (WriterExclusivePtr const &)
  {
    return *this;
  }
  Writer * m_ptr;
};

class Logger
{
public:
  static Logger * Instance();
  WriterExclusivePtr getWriter();
  void stop();

private:
  Logger();
  ~Logger();
  static Logger * m_instance;
  class Impl;
  std::unique_ptr<Impl> m_impl;
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

#define VERBOSE (Logger::Instance()->getWriter() << "Verbose: ")
#define INFO (Logger::Instance()->getWriter() << "Info: ")
#ifdef _DEBUG
#define DEBUG (Logger::Instance()->getWriter() << "Debug: ")
#else
#define DEBUG s_nullLogger
#endif
#define ERROR (Logger::Instance()->getWriter() << "Error: ")
#define FATAL (Logger::Instance()->getWriter() << "FATAL: ")
#define STOPLOGGER Logger::Instance()->stop();


#endif /// LOGGER_HPP
