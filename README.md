# cpp-logger
A generic tool class for logging.

This is a simple multi-threaded logger class written in C++. It is designed in a way that a user can from anywhere tell the Logger to log a custom message using output operators <<.

To avoid bottlenecks the calling thread does not do the printing. Instead the printing is delegated to a separate printing thread and the calling thread only streams the message to a buffer. This is done by having a std::vector of std::strings. Each call to the Logger for writing returns a string onto which one can write one's message. Once the message has been written and the pointer to the string released, the message is printed by the printing thread. Printing thread will print the messages in the order which those messages are sent and waits for the inputting threads to complete their operations before doing anything. In other words, Writer objects will wait for Writer objects obtained before them to complete their input before the printing thread can print them.

In the future, different types of log messages should be handled differently. One could, for example, make VERBOSE messages show only if a certain parameter is given. In addition, the memory management will be improved especially in terms of the buffer. With the current implementation, the memory allocation is done by std::strings independently. This should instead have a single block of dynamically allocated memory.

In addition to this, thread synchronisation should be simplified a bit and readability can be improved by code simplification.

Overall this is a very C++11-> implementation in the way that there is a certain lack of data-orientedness. This might be problematic in environments that are performance-intensive or have limited memory.



How to compile:
cmake .
make