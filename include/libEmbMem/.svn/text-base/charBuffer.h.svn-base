#ifndef __CHARBUFFER_H__
#define __CHARBUFFER_H__

#define DEFAULT_NAME_LEN 16

class CharBufferElem
{
  char *int_buf;
  CharBufferElem *prev;
  CharBufferElem *next;
  CharBufferElem *last;
  int first_free;

public:
  friend class CharBuffer;
  CharBufferElem(int dim);
  ~CharBufferElem();
};

class CharBuffer
{
  char *buf;
  int last_size;
  int int_buf_size;
  CharBufferElem *first_buffer;
  char *int_push(CharBufferElem *&first, const char *str);
  CharBufferElem *int_pop(CharBufferElem *&p, char *&out, bool skip = true);

public:
  CharBuffer(int dim=DEFAULT_NAME_LEN);
  ~CharBuffer();
  const char *push(const char *str);
  const char *pushChar(const char c = 0x0);
  char *pop();
  const char* getValue( const char* startingAddress ) const ;
  int getMaxOutputSize() const;
  void clean();
  bool strEqual(const char *buf_str, const char *str) const ;
};


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
#endif
