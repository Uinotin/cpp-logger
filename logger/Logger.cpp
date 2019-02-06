#include "Logger.hpp"

#include <iostream>
#include <sstream>
#include <thread>

Writer::Writer()
{
}

Writer::~Writer() = default;

Writer::Writer(const Writer&)
{
}

Logger * Logger::m_instance = nullptr;

class Logger::Impl
{
public:
  Impl()
    : m_numTasks(0)
  {
    /// Add enough writers to prevent the programme from getting stuck,
    /// if one thread takes longer to unlock the writer.
    m_writers.resize(32);
    m_currentWriter = m_writers.begin();
    m_currentWriter->m_mutex.lock();

    m_printThread = std::thread([this] { printThread(); });
  }
  void printThread();

  std::vector<Writer> m_writers;
  std::vector<Writer>::iterator m_currentWriter;

  std::thread m_printThread;
  int m_numTasks;
  bool m_notClosing;

  std::mutex m_writerGetterMutex;
};

void Logger::Impl::printThread()
{
  auto currentWriter = m_writers.begin();
  currentWriter->m_mutex.lock();
  while (m_notClosing || m_numTasks) {
    std::cout << currentWriter->m_stream.str();
    currentWriter->m_stream.str(std::string());
    --m_numTasks;
    auto nextWriter = currentWriter + 1;
    if (nextWriter == m_writers.end())
      nextWriter = m_writers.begin();
    nextWriter->m_mutex.lock();
    currentWriter->m_mutex.unlock();
    currentWriter = nextWriter;
  }
}

Logger * Logger::Instance()
{
  if (!m_instance)
    m_instance = new Logger;

  return m_instance;
}

WriterExclusivePtr Logger::getWriter()
{
  std::lock_guard<std::mutex> lockGuard(m_impl->m_writerGetterMutex);
  auto returnWriter = m_impl->m_currentWriter++;
  if (m_impl->m_currentWriter == m_impl->m_writers.end())
    m_impl->m_currentWriter = m_impl->m_writers.begin();
  m_impl->m_currentWriter->m_mutex.lock();

  ++m_impl->m_numTasks;
  return WriterExclusivePtr(&(*returnWriter));
}
void Logger::stop()
{
  m_impl->m_writerGetterMutex.lock();
  m_impl->m_notClosing = false;
  m_impl->m_currentWriter->m_mutex.unlock();
  m_impl->m_printThread.join();
}
Logger::Logger()
  : m_impl(new Impl)
{
}

Logger::~Logger() = default;
