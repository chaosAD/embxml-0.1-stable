#include <stdlib.h>
#include <string.h>
#include "libEmbMem/charBuffer.h"

#define OUT_BUF_SIZE 512

CharBufferElem::CharBufferElem(int dim)
{
  last = this;
  next = prev = NULL;
  int_buf = new char[dim];
  first_free = 0;
}

CharBufferElem::~CharBufferElem()
{
  delete [] int_buf;
}

CharBuffer::CharBuffer(int dim)
{ 
  buf = new char[OUT_BUF_SIZE+1];
  last_size = OUT_BUF_SIZE+1;
  int_buf_size = dim;  
  first_buffer = new CharBufferElem(int_buf_size);
}

CharBuffer::~CharBuffer()
{
  CharBufferElem *q;

  delete [] buf;
  
  while (first_buffer != NULL)
  {
    q = first_buffer;
    first_buffer = first_buffer->next;
    delete q;
  }
}

const char *CharBuffer::pushChar(const char c)
{
  CharBufferElem *ptr, *q;
  int i, count;
  bool found;

  ptr = first_buffer->last;

  if (ptr->first_free == int_buf_size)
  {
    ptr->next = new CharBufferElem(int_buf_size);
    ptr->next->prev = ptr;
    for (q = ptr; q != NULL; q->last = ptr->next->last, q = q->prev);
    ptr = ptr->next;
  }

  ptr->int_buf[ptr->first_free++] = c;
  if (!c)
  {
    for (i = ptr->first_free-2, q = ptr, count = 0, found = false;
         (q != NULL) && (!found);
         i = ((i > 0)? i-1 : (q = q->prev, int_buf_size-1)))
       if (i >= 0)
       {
         found = (!q->int_buf[i]);
         if (!found)
           count++;
       }
    if (count > last_size)
    {
      delete [] buf;
      buf = new char[last_size = (count+1)];
    }
  }
  // Result value
  return &(ptr->int_buf[ptr->first_free-1]);
}

char *CharBuffer::int_push(CharBufferElem *&first, const char *str)
{
  int len;
  CharBufferElem *ptr, *q;
  char *result = NULL;

  len = strlen(str);

  if (first == NULL)
    ptr = first = new CharBufferElem(int_buf_size);

  ptr = first->last;

  if ((ptr->first_free + len) >= int_buf_size)
  {
    strncpy(&(ptr->int_buf[ptr->first_free]),str,int_buf_size - ptr->first_free);
    if (int_buf_size - ptr->first_free)
    {
      result = &(ptr->int_buf[ptr->first_free]);
      int_push(ptr->next,&(str[int_buf_size - ptr->first_free]));
    }
    else
      result = int_push(ptr->next,&(str[int_buf_size - ptr->first_free]));
    ptr->first_free = int_buf_size;
    ptr->next->prev = ptr;
    for (q = ptr; q != NULL; q->last = ptr->next->last, q = q->prev);
  }
  else
  {
    strcpy(&(ptr->int_buf[ptr->first_free]),str);
    result = &(ptr->int_buf[ptr->first_free]);
    ptr->first_free += len+1;
  }

  return result;
}

const char *CharBuffer::push(const char *str)
{
  bool found = false;
  CharBufferElem *ptr;

  if ((int) strlen(str) > last_size)
  {
    delete [] buf;
    buf = new char[(last_size = (strlen(str)+1))];
  }

  ptr = first_buffer;
  while ((ptr != NULL) && (!found))
  {
    found = ((str >= ptr->int_buf) && (str < ptr->int_buf + int_buf_size));
    ptr = ptr->next;
  }

  return ((found)? (char *) str : int_push(first_buffer,str));
}

int CharBuffer::getMaxOutputSize() const
{
  return last_size;
}

const char *CharBuffer::getValue(const char *startingAddress) const {
  int i, j = 0;
  CharBufferElem *ptr;

  ptr = first_buffer;
  while ((ptr != NULL) && (!((startingAddress >= ptr->int_buf) && (startingAddress < ptr->int_buf + int_buf_size))))
    ptr = ptr->next;

  if (ptr != NULL)
    for (i = (startingAddress - ptr->int_buf); ptr->int_buf[i]; j++)
    {
      buf[j] = ptr->int_buf[i];
      if (++i == int_buf_size)
      {
        ptr = ptr->next;
        i = 0;
      }
    }

  buf[j] = '\0';
  return buf;
}

CharBufferElem *CharBuffer::int_pop(CharBufferElem *&p, char *&out, bool skip)
{
  int i;
  CharBufferElem *ptr = p;

  if (p != NULL)
  {
    for (i = p->first_free-1; (i >= 0) && (skip || p->int_buf[i]);  i--)
    {
      skip = skip && (!p->int_buf[i]);
      if (!skip)
        *(out--) = p->int_buf[i];
    }
    if ((i < 0) && (p->prev != NULL))
    {
      ptr = int_pop(p->prev,out,false);
      p->prev->next = NULL;
      delete p;
    }
    else
      p->first_free = ++i;
  }

  return ptr;
}

char *CharBuffer::pop()
{
  char *p = (buf+last_size-1);

  *(p--) = '\0';
  
  first_buffer->last = int_pop(first_buffer->last,p);
  first_buffer->last->next = NULL;
  return (++p);
}

void CharBuffer::clean()
{
  CharBufferElem *q;
  CharBufferElem *ptr = first_buffer->next;

  first_buffer->first_free = 0;
  first_buffer->last = first_buffer;
  first_buffer->next = first_buffer->prev = NULL;
  while (ptr != NULL)
  {
    q = ptr;
    ptr = ptr->next;
    delete q;
  }
}

bool CharBuffer::strEqual(const char *buf_str, const char *str) const
{
  return (!strcmp(getValue(buf_str),str));
}
