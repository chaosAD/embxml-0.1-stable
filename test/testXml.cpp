#include <iostream>
#include <libEmbXml/document.h>
#include <libEmbXml/IOFile.h>

using namespace std;

void testCharBuffer() {
  CharBuffer charBuf = CharBuffer();
  charBuf.push("my");
  charBuf.push("name");
  charBuf.push("chrizz");
  
  cout << charBuf.pop() << endl;
  cout << charBuf.pop() << endl;
  cout << charBuf.pop() << endl;
}



int main() {
  std::cout << "hello" << std::endl;
  testCharBuffer();
  return 0;
}


