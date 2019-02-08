#include "../logger/Logger.hpp"
#include <iostream>
#include <thread>
#include <chrono>

/// User type
class MyClass
{
public:
  MyClass(int value, std::string string)
    : m_value(value),
      m_string(string)
  {}
  ~MyClass() = default;

  /// Define how output operator works for the user type
  friend std::ostream &operator<<(std::ostream &stream, const MyClass & myClass)
  {
    stream << " my fancy class with value: " << myClass.m_value
	   << " ...and string: \"" << myClass.m_string << "\"";
    return stream;
  }
private:
  int m_value;
  std::string m_string;
};

static void sleeper()
{
  /// INFO streams the local time and the text " Info: " to a Writer object and returns it
  /// The Writer object is released when "info" goes out of scope.
  auto info = INFO;
  info << "This is";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  /// Print user type based on MyClass::operator<<()
  info << MyClass(3, "message") << std::endl;
}

int main()
{
  Logger::Instance()->setLogFile("log.txt");
  std::thread sleeperThread(sleeper);
  {
    auto info = INFO;
    info << "Kurnau";
    /// All messages being printed with a Writer object obtained after "info"
    /// will wait for it to be released before being printed
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    info << " miau" << std::endl;
  }
  for (int i = 0; i < 255; ++i)
    VERBOSE << "Hello World! " << i << std::endl;
  /// DEBUG will only print something if _DEBUG is defined
  DEBUG << "Hello" << " World" << "!" << std::endl;

  sleeperThread.join();

  /// STOPLOGGER waits until the queued log messages have been printed
  /// If there are still UnlockablePtr<Writer> instances in scope,
  /// all messages queued to be printed after those cannot be guaranteed printing
  /// All Writer objects need to be released to guarantee that the programme will exit
  STOPLOGGER
  return 0;
}
