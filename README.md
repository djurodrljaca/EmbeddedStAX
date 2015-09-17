# Embedded StAX
A light-weight StAX (Streaming API for XML) reader and writer for embedded systems.

The purpose of this project is to create a light-weight StAX reader and writer for embedded systems with minimal dependencies. The only dependency are a few classes from the *C++ standard library*:
* std::string
* std::list
* and related classes

The project contains a small number of C++ source and header files which should be included in the project. The project is licened under the [Unlicense](http://unlicense.org). One of the resons for that is to make it easier to include in embedded projects.

Here is the list of main goals of this project:
* Create a StAX reader and writer with minimal dependencies to make it more useful for embedded software.
* Support as much of the XML standard as possible for this kind of project.
* Reader should be able to read multiple XML documents from a (continous) data stream without explicit framing of the XML documents. For example it should be usable for communication protocols that can read multiple XML messages one after another with data from a TCP socket.
* Make the code MISRA C++ 2008 compliant with minimal deviations
* Have as much of the code covered with unit test as possible
 
There are also some additional goals for when the main goals are achieved: create a code generator for creation of objects that can read and/or write XML documents defined in a XML schema.
