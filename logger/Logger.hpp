#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Writer.hpp"

#include <memory>
#include <iostream>
#include <mutex>
#include <vector>

/// Class around Writer that unlocks Writer's mutex once WriterExclusivePtr is deleted
template <typename T>
class UnlockablePtr
{
  friend class Logger;
public:
  UnlockablePtr(UnlockablePtr && ptr)
    : m_ptr(ptr.m_ptr),
      m_mutex(ptr.m_mutex)
  {
    ptr.m_ptr = nullptr;
    ptr.m_mutex = nullptr;
  }
  ~UnlockablePtr()
  {
    if (m_mutex)
      m_mutex->unlock();
  }
  T * operator ->()
  {
    return m_ptr;
  }
  T const * operator ->() const
  {
    return m_ptr;
  }
  T & operator << (std::ostream& data(std::ostream&))
  {
    return *m_ptr << data;
  }
  template<typename Y> T & operator << (const Y& data)
  {
    return (*m_ptr << data);
  }

private:
  UnlockablePtr(T * ptr, std::mutex * mutex)
    : m_ptr(ptr),
      m_mutex(mutex)
  {
  }
  UnlockablePtr(UnlockablePtr const &)
  {
  }
  UnlockablePtr & operator = (UnlockablePtr const &)
  {
    return *this;
  }
  T * m_ptr;
  std::mutex * m_mutex;
};

class Logger
{
public:
  static Logger * Instance();

  /// Set the name of the log file
  void setLogFile(const std::string & path);

  /// gets a Writer object for printing
  /// and prints a time stamp on the start of the message
  /// UnlockablePtr should be go out of scope before
  /// stop() is called.
  UnlockablePtr<Writer> getWriter();

  /// Gets a Writer and
  /// @todo do these methods with a single function
  UnlockablePtr<Writer> getWriterVerbose();
#ifdef _DEBUG
  UnlockablePtr<Writer> getWriterDebug();
#else
  /// @todo find a better way of doing this
  /// It can be problematic if the user assumes the method
  /// will return UnlockablePtr<Writer>
  UnlockablePtr<NullLogger> getWriterDebug();
#endif
  UnlockablePtr<Writer> getWriterInfo();
  UnlockablePtr<Writer> getWriterWarning();
  UnlockablePtr<Writer> getWriterError();
  UnlockablePtr<Writer> getWriterFatal();

  /// Stops logger and synchronises with the printing thread
  void stop();

private:
  Logger();
  ~Logger();
  static Logger * m_instance;
  static std::mutex m_instanceMutex;
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

/// Sometimes people just want print a simple message
/// Let's make their life easier
#define VERBOSE Logger::Instance()->getWriterVerbose()
#define INFO Logger::Instance()->getWriterInfo()
#define DEBUG Logger::Instance()->getWriterDebug()
#define WARNING Logger::Instance()->getWriterWarning()
#define ERROR Logger::Instance()->getWriterError()
#define FATAL Logger::Instance()->getWriterFatal()
#define STOPLOGGER Logger::Instance()->stop();


#endif /// LOGGER_HPP
