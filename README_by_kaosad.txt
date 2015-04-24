Configuring Build
=================
To configure type:
  ./configure --prefix=/C/Vennie/Projects/C/embxml-0.1-stable/result CC=/C/MinGW/bin/gcc.exe CXX=/C/MinGW/bin/g++.exe

which will use gcc and g++ from the path provided and install into
'/result' folder. Please change the absolute paths if necessary.

Build
=====
Then run:
  make install

To clean run:  
  make clean

  
Running test 
============
To compile and run the test, go to 'test' folder then type:
  ./CompileRunTestEmbXml

The test produce copious XML file in 'artifacts' folder.  
  
The other simple test is for testing CharBuffer. To compile and
run it, type:
  ./DemoCharBuffer


Note
====
I have created a bash file to compile and archive object files into
libraries. It is not the recommended way, but it shows step-by-step
the compilation and archiving. Check the 'CompileArchive' file.