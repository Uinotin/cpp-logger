#include "Logger.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <ctime>

Logger * Logger::m_instance = nullptr;
std::mutex Logger::m_instanceMutex;

class Logger::Impl
{
public:
  Impl()
    : m_isClosing(false)
  {
    /// Add enough writers to prevent the programme from getting stuck,
    /// if one thread takes longer to unlock the writer.
    m_writers.resize(32);
    m_currentWriter = m_writers.begin();
    m_currentWriter->m_mutex.lock();
    m_writers[2].m_mutex.lock();

    m_printThread = std::thread([this] {
	/// Syncronise with main thread
        m_writers[0].m_mutex.lock();
	m_writers[2].m_mutex.unlock();
	printThread();
      });
  }
  void printThread();

  std::vector<Writer> m_writers;
  std::vector<Writer>::iterator m_currentWriter;

  std::thread m_printThread;
  bool m_isClosing;

  std::mutex m_writerGetterMutex;
  std::mutex m_mutex;

  std::string m_logFile;
};

void Logger::Impl::printThread()
{
  auto currentWriter = m_writers.begin();
  std::ofstream file;
  while (true) {
    std::cout << currentWriter->m_stream.str();
    if (!m_logFile.empty()) {
      if (!file.is_open()){
	file.open(m_logFile, std::fstream::app);
	if (file.fail()) {
	  std::cout << "Unable to open log file" << std::endl;
	  m_logFile = "";
	  if (file.is_open())
	    file.close();
	}
      }
      if (!m_logFile.empty())
	file << currentWriter->m_stream.str();
    }
    currentWriter->m_stream.str(std::string());
    auto nextWriter = currentWriter + 1;
    if (nextWriter == m_writers.end())
      nextWriter = m_writers.begin();

    /// Hack: If the programme is about to close,
    ///       make sure to break from while at the correct time.
    m_mutex.lock();
    if (m_isClosing) {
      if (!nextWriter->m_mutex.try_lock())
	break;
      else
	nextWriter->m_mutex.unlock();
    }
    m_mutex.unlock();

    if (!nextWriter->m_mutex.try_lock()) {
      if (file.is_open())
        file.close();
      nextWriter->m_mutex.lock();
    }
    currentWriter->m_mutex.unlock();
    currentWriter = nextWriter;
  }
}

Logger * Logger::Instance()
{
  if (!m_instance) {
    /// Make sure to not make multiple instances of Logger
    std::lock_guard<std::mutex> lockGuard(m_instanceMutex);
    if (!m_instance)
      m_instance = new Logger;
  }

  return m_instance;
}

UnlockablePtr<Writer> Logger::getWriter()
{
  std::lock_guard<std::mutex> lockGuard(m_impl->m_writerGetterMutex);
  auto returnWriter = m_impl->m_currentWriter++;
  if (m_impl->m_currentWriter == m_impl->m_writers.end())
    m_impl->m_currentWriter = m_impl->m_writers.begin();
  m_impl->m_currentWriter->m_mutex.lock();

  auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char timeString[100];
  if (std::strftime(timeString, sizeof(timeString), "%c", std::localtime(&timeNow)))
    *returnWriter << "[" << timeString << "] ";
  return UnlockablePtr<Writer>(&(*returnWriter), &(returnWriter->m_mutex));
}


UnlockablePtr<Writer> Logger::getWriterVerbose()
{
  auto writer = getWriter();
  writer << " Verbose: ";
  return writer;
}

#ifdef _DEBUG
UnlockablePtr<Writer> Logger::getWriterDebug()
#else
UnlockablePtr<NullLogger> Logger::getWriterDebug()
#endif
{
#ifdef _DEBUG
  auto writer = getWriter();
  writer << " Debug: ";
#else
  auto writer = UnlockablePtr<NullLogger>(s_nullLogger, nullptr);
#endif

  return writer;
}

UnlockablePtr<Writer> Logger::getWriterInfo()
{
  auto writer = getWriter();
  writer << " Info: ";
  return writer;
}

UnlockablePtr<Writer> Logger::getWriterWarning()
{
  auto writer = getWriter();
  writer << " Warning: ";
  return writer;
}

UnlockablePtr<Writer> Logger::getWriterError()
{
  auto writer = getWriter();
  writer << " Error: ";
  return writer;
}

UnlockablePtr<Writer> Logger::getWriterFatal()
{
  auto writer = getWriter();
  writer << " Fatal: ";
  return writer;
}

void Logger::stop()
{
  std::lock_guard<std::mutex> lockGuard(m_impl->m_writerGetterMutex);
  /// Make sure that printThread is not currently reading at:
  /// if(m_isClosing) {...}
  /// to avoid a race conditions.
  m_impl->m_mutex.lock();
  auto writer = m_impl->m_currentWriter++;
  if (m_impl->m_currentWriter == m_impl->m_writers.end())
    m_impl->m_currentWriter = m_impl->m_writers.begin();
  m_impl->m_currentWriter->m_mutex.lock();
  m_impl->m_isClosing = true;

  *writer << "Logger exited correctly." << std::endl;
  writer->m_mutex.unlock();
  m_impl->m_mutex.unlock();

  m_impl->m_printThread.join();
}
Logger::Logger()
  : m_impl(new Impl)
{
}

Logger::~Logger() = default;


void Logger::setLogFile(const std::string & path)
{
  m_impl->m_logFile = path;
}
