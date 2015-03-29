# ITCLib
C++ ipc library (legacy code, pre-c++-11, development towards c++11 and beyond)

ITCLib is a library for multithreaded and asynchronous development. The library is in development since 2003 and
survived almost complete rewrite in 2007. Now is a time to rewrite again or may be even replace all the staff with
new STL features of C++11. This version may be compiled with gcc 4.7.4,  4.8.4 and all in between; you can compile 
this librarary with clang 3.5.1 too. Actually I use clang for static code analysis. 

It is a tamplate library and will be kept this way whenever possible.

The library includes following basic entities: threads, semaphores (mutexes, conditionals and atomics are replaced with those provided by STL),
advanced exception handling template. The threads and semaphores will not be replaced with STL equivalents because:
a) threads are not cancelable in STL, which is vital to proper POSIX semaphores handling (POSIX semaphores are the cancelation points)
b) there are no semaphores provided by STL. Any surrogates like those based on conditionals, are of no use. POSIX semaphores are simply irreplaceable.

There are some high level templates like Scoket which may be transformed at compile time to ServerSocket or ClientSocket
just by typedefs (typedef itc::net::Socket<SRV_TCP_ANY_IF,100> ServerSocket;typedef itc::net::Socket<CLN_TCP_KA_TND> ClientSocket;);
There are two high level thread classes: PThread and CancelableThread. The first one's purpose is to run within ThreadPool, the 
second one's is to run standalone threads. This library also includes a thread safe log output adapter and simple formatter,
those may be used with utils/include/Logger. There is an interface itc::abstract::IRunnable, by inheriting and implementing this
interface, one may simply start a thread with CancelableThread template or push the runnable into ThreadPoolManager (ITCFramework)
for automatic scheduling.

There are still some usless classes and files, those have to be removed. 

The documentation may be generated with doxygen.
