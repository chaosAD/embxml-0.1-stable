#include <vector>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "libEmbXml/document.h"

/***********************************************************************\
* 					Class Node
\***********************************************************************/

Node::Node(ObjManager<Node> *&effNodeObjManager, ObjManager<Attribute> *&effAttrObjManager, NodeType t, const char *n, const char *v):
                                type(t),
                                effectiveNodeObjManager(effNodeObjManager),
                                attrObjManager(effAttrObjManager),
                                attributes(NULL),
                                num_attributes(0),
                                num_children(0),
                                lkUpN(NULL),
                                first_child(NULL),
                                last_child(NULL),
                                prev(NULL),
                                next(NULL),
                                parent(NULL)
{
    name = (char *) effectiveNodeObjManager->nameBuffer->push(n);
    value = (char *) effectiveNodeObjManager->valueBuffer->push(v);
}



Node::~Node() {
  Attribute *a_p, *a_q;
  Node *p, *q;

  for (p = first_child; (p != NULL); q = p, p = p->next, delete q);

  unlinkNode();

  for (a_p = attributes; a_p != NULL; a_q = a_p, a_p = a_p->next, delete a_q);
}


void Node::unlinkNode()
{
  if (prev != NULL)
    prev->next = next;

  if (next != NULL)
    next->prev = prev;

  if (parent != NULL)
  {
    if (parent->first_child == parent->last_child)
    {
      parent->num_children = num_children;
      parent->first_child = first_child;
      parent->last_child = last_child;
    }
    else
    {
      parent->num_children--;
      if (parent->first_child == this)
        parent->first_child = next;
      if (parent->last_child == this)
        parent->last_child = prev;
    }
  }

  prev = next = parent = first_child = last_child = NULL;
}

void Node::attachNode(Node *p, NodeLinkType where)
{
  Node *ptr;

  switch(where)
  {
    case _first: if (parent != NULL)
                 {
                   p->next = parent->first_child;
                   parent->first_child->prev = p;
                   parent->first_child = p;
                   parent->num_children++;
                   p->parent = parent;
                 }
                 else
                 {
                   for (ptr = this; ptr->prev != NULL; ptr = ptr->prev);
                   ptr->attachNode(p,_prev);
                 }
                 break;
    case _last: if (parent != NULL)
                {
                  p->prev = parent->last_child;
                  parent->last_child->next = p;
                  parent->last_child = p;
                  parent->num_children++;
                  p->parent = parent;
                }
                else
                {
                  for (ptr = this; ptr->next != NULL; ptr = ptr->next);
                  ptr->attachNode(p,_next);
                }
                break;
    case _prev: p->parent = parent;
                p->next = this;
                p->prev = prev;
                if (prev != NULL)
                  prev->next = p;
                prev = p;
                if (parent != NULL)
                {
                  parent->num_children++;
                  if (parent->first_child == this)
                    parent->first_child = p;
                }
                break;
    case _next: p->parent = parent;
                p->prev = this;
                p->next = next;
                if (next != NULL)
                  next->prev = p;
                next = p;
                if (parent != NULL)
                {
                  parent->num_children++;
                  if (parent->last_child == this)
                    parent->last_child = p;
                }
                break;
    case _first_child: p->parent = this;
                       p->next = first_child;
                       if (first_child != NULL)
                         first_child->prev = p;
                       first_child = p;
                       if (last_child == NULL)
                         last_child = p;
                       num_children++;
                       break;
    case _last_child: p->parent = this;
                      p->prev = last_child;
                      if (last_child != NULL)
                        last_child->next = p;
                      last_child = p;
                      if (first_child == NULL)
                        first_child = p;
                      num_children++;
                      break;
    case _parent: if (parent != NULL)
                  {
                    for (ptr = parent->first_child; (ptr != NULL) && (ptr != p); ptr = ptr->next);
                    
                    if (ptr == NULL)
                    {
                      if (prev != NULL)
                        prev->next = p;
                      if (next != NULL)
                        next->prev = p;
                      p->prev = prev;
                      p->next = next;
                      prev = next = NULL;
                      p->parent = parent;
                      if (parent->first_child == this)
                        parent->first_child = p;
                      if (parent->last_child == this)
                        parent->last_child = p;
                      p->num_children = 1;
                      p->first_child = p->last_child = this;
                      parent = p;
                    }
                    else
                    {
                      if (prev != NULL)
                        prev->next = next;
                      if (next != NULL)
                        next->prev = prev;
                      if (this == parent->first_child)
                        parent->first_child = next;
                      if (this == parent->last_child)
                        parent->last_child = prev;
                      parent->num_children--;
                      prev = next = NULL;
                      ptr->attachNode(this,_last_child);
                    }
                  }
                  break;
    default: break;     //USELESS!!!
  }
}

Node *Node::getNode(NodeLinkType where, int idx) const
{
  Node *ptr = NULL ;

  switch(where)
  {
    case _prev: ptr = prev;
                break;
    case _next: ptr = next;
                break;
    case _first: ptr = ((parent == NULL)? (Node *) this : parent->first_child);
                 break;
    case _last: ptr = ((parent == NULL)? (Node *) this : parent->last_child);
                break;
    case _first_child: ptr = first_child;
                       break;
    case _last_child: ptr = last_child;
                      break;
    case _parent: ptr = parent;
                  break;
    default: break;
  }

  if (where != _parent)
    while ((ptr != NULL) && idx)
    {
      ptr = ((idx > 0)? ptr->next : ptr->prev);
      idx += ((idx > 0)? -1 : +1 );
    }

  return ptr;
}

Attribute *Node::getAttribute(int idx) const
{
  Attribute *retAttr = NULL;
  int i;
  
  if( idx < num_attributes)
    for (i = 0, retAttr = attributes; (retAttr != NULL) && (i < idx); i++, retAttr = retAttr->next);
    
  return retAttr;
}

const char *Node::getName() const
{
  return effectiveNodeObjManager->nameBuffer->getValue(name);
}

const char *Node::getValue() const
{
  return effectiveNodeObjManager->valueBuffer->getValue(value) ;
}

NodeType Node::getType() const
{
  return type;
}

void Node::setName(const char *n)
{
  name = (char *) effectiveNodeObjManager->nameBuffer->push(n);
}

void Node::setValue(const char *v)
{
  value = (char *) effectiveNodeObjManager->valueBuffer->push(v);
}

Attribute* Node::addAttribute(const char *n, const char *v)
{
  ObjManager<Attribute> *buf_ptr;
  Attribute *ptr = new(attrObjManager,buf_ptr) Attribute(buf_ptr,n,v), *q;

  if (attributes == NULL)
    attributes = ptr;
  else
  {
    for (q = attributes; q->next != NULL; q = q->next);

    q->next = ptr;
    ptr->prev = q;
  }
  
  num_attributes++;
  return ptr ;
}

void Node::deleteAttribute(const char *n)
{
  Attribute *ptr = NULL;

  if (strlen(n))
    for (ptr = attributes; (ptr != NULL) && (!ptr->effectiveObjManager->nameBuffer->strEqual(ptr->name,n)); ptr = ptr->next);
  else
  {
    ptr = attributes;
    attributes = attributes->next;
  }

  if (ptr != NULL)
  {
    delete ptr;
    num_attributes--;
  }
}

const char *Node::getAttrName(int idx) const
{
  int i;
  Attribute *ptr;
  
  if (idx < num_attributes)
  {
    for (i = 0, ptr = attributes; (ptr != NULL) && (i < idx); i++, ptr = ptr->next);

    if (ptr != NULL)
      return ptr->effectiveObjManager->nameBuffer->getValue(ptr->name);
  }
	
  return NULL;
}

const char *Node::getAttrValue(int idx) const
{
  int i;
  Attribute *ptr;
  
  if (idx < num_attributes)
  {
    for (i = 0, ptr = attributes; (ptr != NULL) && (i < idx); i++, ptr = ptr->next);

    if (ptr != NULL)
      return ptr->effectiveObjManager->valueBuffer->getValue(ptr->value);
  }
  
  return NULL;
}

void Node::setAttrValue(int idx, const char *v)
{
  int i;
  Attribute *ptr;

  if (idx < num_attributes)
  {
    for (i = 0, ptr = attributes; (ptr != NULL) && (i < idx); i++, ptr = ptr->next);
    
    if (ptr != NULL)
      value = (char *) ptr->effectiveObjManager->valueBuffer->push(v);
  }
}

const char *Node::getAttrValue(const char *n) const
{
  Attribute *ptr;

  for (ptr = attributes; (ptr != NULL) && (!ptr->effectiveObjManager->nameBuffer->strEqual(ptr->name,n)); ptr = ptr->next);

  return ((ptr != NULL)? ptr->effectiveObjManager->valueBuffer->getValue(ptr->value) : NULL);
}

void Node::setAttrValue(const char *n, const char *v)
{
  Attribute *ptr;

  for (ptr = attributes; (ptr != NULL) && (!ptr->effectiveObjManager->nameBuffer->strEqual(ptr->name,n)); ptr = ptr->next);

  if (ptr != NULL)
    value = (char *) ptr->effectiveObjManager->valueBuffer->push(v);
}

int Node::numAttributes() const
{
  return num_attributes;
}

int Node::numChildren() const
{
  return num_children;
}

Node *Node::lookUpNode(const char *n)
{
  Node *result = NULL, *p, *ptr;

  if (first_child != NULL)
  {
    if (num_children == 1)
      result = ((!strcmp(n,first_child->name))? first_child : NULL);
    else
    {      
      if (lkUpN == NULL)
        ptr = first_child;
      else
        ptr = ((lkUpN->next == NULL)? first_child : lkUpN->next);
      
      p = ((ptr->prev == NULL)? last_child : ptr->prev);
    
      for (result = ptr; (result != p) && strcmp(n,result->name); result = ((result->next != NULL)? result->next : first_child));
      if( result == p && strcmp(n,result->name) )
        result = NULL ;
    }
  }

  return (lkUpN = result);
}

void *Node::operator new(size_t size, ObjManager<Node>* &ptr, ObjManager<Node>* &effectiveObjManager)
{
  return ((void *) ptr->getFreeObj(effectiveObjManager));
}

void Node::operator delete(void *p)
{
  ((Node *) p)->effectiveNodeObjManager->freeObj((Node *) p);
}


/***********************************************************************\
* 					Class Comment
\***********************************************************************/

Comment::Comment(ObjManager<Node> *&effNodeObjManager, ObjManager<Attribute> *&effAttrObjManager, const char *v) : Node(effNodeObjManager,effAttrObjManager,_comment,"",v)
{
  
}

Comment::~Comment()
{
  unlinkNode();
}  

void Comment::attachNode(Node *p, NodeLinkType where)
{
  if (where != _parent)
    Node::attachNode(p,where);
}

Attribute* Comment::addAttribute(const char *n, const char *v) {
  return NULL ;
}

void Comment::setAttrValue(const char *n, const char *v) { }

void Comment::setAttrValue(int idx, const char *v) { }

Node *Comment::lookUpNode(const char *n) { return NULL; }


/***********************************************************************\
* 					Class Attribute
\***********************************************************************/

Attribute::Attribute(ObjManager<Attribute> *&effObjManager, const char *n, const char *v) :
  effectiveObjManager(effObjManager),next(NULL),prev(NULL)
{
  name = (char *) effectiveObjManager->nameBuffer->push(n);
  value = (char *) effectiveObjManager->valueBuffer->push(v);
}

Attribute::~Attribute()
{
  if (prev != NULL)
    prev->next = this->next;

  if (next != NULL)
    next->prev = this->prev;
}

void *Attribute::operator new(size_t size, ObjManager<Attribute> *&ptr, ObjManager<Attribute> *&effectiveObjManager)
{
  return ((void *) ptr->getFreeObj(effectiveObjManager));
}

void Attribute::operator delete(void *p)
{
  ((Attribute *)p)->effectiveObjManager->freeObj((Attribute *) p);
}


/***********************************************************************\
* 					Document
\***********************************************************************/

Document::Document(int nodes, int numOfAttr)
{
  ObjManager<Node> *buf_ptr;

  nodeObjManager = new ObjManager<Node>(nodes);
  if (numOfAttr)
    attrObjManager = new ObjManager<Attribute>(numOfAttr);
  else
    attrObjManager = new ObjManager<Attribute>(32);
  root = new (nodeObjManager,buf_ptr) Node(buf_ptr,attrObjManager,_root);

  tableList = NULL;
}

Document::~Document()
{
  delete nodeObjManager;
  delete attrObjManager;

  if (tableList != NULL)
    delete tableList;
}

Node *Document::getRootNode() const
{
  return root ;
}

Node *Document::attach(const char *name, const char *value, Node *p, NodeType t, NodeLinkType where)
{
  Node *ptr = ((p == NULL)? root : p);
  Node *new_ptr;

  ObjManager<Node> *buf_ptr;

  new_ptr = new (nodeObjManager,buf_ptr) Node(buf_ptr,attrObjManager,t,name,value);

  ptr->attachNode(new_ptr,where);

  return new_ptr;
}

Node *Document::inject(const char *name, const char *value, Node *p, NodeType t)
{
  Node *ptr = ((p == NULL)? root : p), *q;
  Node *new_ptr = NULL;

  ObjManager<Node> *buf_ptr;
  
  if ((ptr = ptr->getNode(_first_child)) != NULL)
  {
    new_ptr = new (nodeObjManager,buf_ptr) Node(buf_ptr,attrObjManager,t,name,value);

    for (q = ptr; q != NULL;)
    {
      ptr = q;
      q = q->getNode(_next);
      ptr->attachNode(new_ptr,_parent);
    }
  }

  return new_ptr;
}

void Document::remove(Node *p)
{
  delete p;
}

void Document::clean()
{
  root->first_child = root->last_child = NULL;
  root->num_children = 0;
  
  delete tableList;
  tableList = NULL;
  
  attrObjManager->clean();
  nodeObjManager->clean();
}

void Document::makeTable(Node *t, bool withNumOfRows, int rows)
{
  tableListType *ptr = new tableListType(tableList,withNumOfRows);
  tableList = ptr;

  tableList->tableRoot = t;
  tableList->rows = rows;
}

void Document::addTableRow(Node *t)
{
  unsigned int i;
  Node *new_ptr;

  ObjManager<Node> *buf_ptr;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    if ((!ptr->rows) || (ptr->effRows < ptr->rows))
    {
      if (ptr->rows)
      {
        if (ptr->effRows < ptr->rows)
          ptr->effRows++;
        else
          delete getCircTableRow(t,ptr->effRows-1);
      }
          
      else
        ptr->effRows++;
      ptr->rowPtr++;
      new_ptr = new (nodeObjManager,buf_ptr) Node(buf_ptr,attrObjManager,_tag,ptr->rowName);
      ptr->tableRoot->attachNode(new_ptr);
      ptr->curRow = new_ptr;
      for (i = 0; i < ptr->colInfo.size(); ++i)
      {
        new_ptr = new (nodeObjManager,buf_ptr) Node(buf_ptr,attrObjManager,_tag,ptr->colInfo[i]);
        ptr->curRow->attachNode(new_ptr);
      }
    }
    else
      ptr->rowPtr = ((ptr->rowPtr >= (ptr->rows - 1))? (ptr->curRow = ptr->tableRoot->getNode(_first_child), 0) : (ptr->curRow = ptr->tableRoot->getNode(_first_child,ptr->rowPtr + 1), ptr->rowPtr + 1));
  }
}

void Document::addTableRow(Node *t, const char *values, ...)
{
  va_list list;
  unsigned int cols;
  tableListType *ptr = tableList->findTable(t);

  addTableRow(t);
  va_start(list,values);
  cols = 0;
  while (values && (cols < ptr->colInfo.size()))
  {
    setTableValue(t,cols++,values);
    values = (char *) va_arg(list, char *);
  }
  va_end(list);
}

void Document::setTableRows(Node *t, int rows)
{
  int i;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    ptr->rows = rows;

    if (ptr->rows && (ptr->effRows > ptr->rows))
    {
      if (ptr->rowPtr < ptr->rows)
      {
        for (i = 0; i < (ptr->effRows - ptr->rows); i++)
          delete ptr->tableRoot->getNode(_first_child,ptr->rowPtr+1);
      }
      else
      {
        for (i = 0; i < (ptr->effRows - ptr->rowPtr - 1); i++)
          delete ptr->tableRoot->getNode(_first_child,ptr->rowPtr+1);
        for (i = 0; i < (ptr->rowPtr - ptr->rows + 1); i++)
          delete ptr->tableRoot->getNode(_first_child);
      }

      ptr->effRows = ptr->rows;
    }
  }
}

void Document::setTableRowName(Node *t, const char *n)
{
  int i;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    ptr->rowName = (char *) nodeObjManager->nameBuffer->push(n);

    for (i = 0; i < ptr->effRows; i++)
      ptr->tableRoot->getNode(_first_child,i)->setName(ptr->rowName);
  }
}

void Document::setTableColName(Node *t, int col, const char *n)
{
  char *cptr;
  int i, j, len;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    if (col == ptr->colInfo.size())
    {
      cptr = (char *) nodeObjManager->nameBuffer->push(n);
      ptr->colInfo.push_back(cptr);

      /* Has effect on rows already present */
      len = ptr->colInfo.size();
      for (i = 0; i < ptr->effRows; i++)
        for (j = 0; j < len; j++)
          ptr->tableRoot->getNode(_first_child,i)->getNode(_first_child,j)->setName(cptr);
    }
}

int Document::getTableRows(Node *t)
{
  int result = -1;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    result = ptr->effRows;

  return result;
}

int Document::getTableCols(Node *t)
{
  int result = -1;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    result = ptr->colInfo.size();

  return result;
}

Node *Document::getTableNode(Node *t, int rowIndex, int columnIndex)
{
  Node *result = NULL, *p, *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    if ((p = ptr->tableRoot->getNode(_first_child,rowIndex)) != NULL)
      if ((q = p->getNode(_first_child,columnIndex)) != NULL)
        result = q;

  return result;
}

Node *Document::getTableNode(Node *t, int columnIndex)
{
  Node *result = NULL, *p;
  tableListType *ptr = tableList->findTable(t);

  if ((ptr != NULL) && (ptr->curRow != NULL))
    if ((p = ptr->curRow->getNode(_first_child,columnIndex)) != NULL)
      result = p;

  return result;
}

Node *Document::getCircTableRow(Node *t, int displacement) const
{
  Node *result = NULL;
  tableListType *ptr = tableList->findTable(t);
  
  if (ptr != NULL)
    result = ptr->tableRoot->getNode(_first_child,(ptr->effRows + ((ptr->rowPtr - displacement) % ptr->effRows)) % ptr->effRows);
    
  return result;
}

Node *Document::getCircTableNode(Node *t, int displacement, int columnIndex)
{
  Node *p = getCircTableRow(t,displacement);
  Node *result = NULL;
  
  if (p != NULL)
    result = p->getNode(_first_child,columnIndex);
  
  return result;
}

void Document::setTableValue(Node *t, int rowIndex, int columnIndex, const char *value)
{
  Node *p, *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    if ((p = ptr->tableRoot->getNode(_first_child,rowIndex)) != NULL)
      if ((q = p->getNode(_first_child,columnIndex)) != NULL)
        q->setValue(value);
}

void Document::setTableValue(Node *t, int rowIndex, const char *columnName, const char *value)
{
  int i, len;
  Node *p, *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    for (i = 0, len = ptr->colInfo.size(); (i < len) && strcmp((char *) ptr->colInfo[i],columnName); i++);

    if (i < len)
      if ((p = ptr->tableRoot->getNode(_first_child,rowIndex)) != NULL)
        if ((q = p->getNode(_first_child,i)) != NULL)
          q->setValue(value);
  }
}

void Document::setTableValue(Node *t, int columnIndex, const char *value)
{
  Node *q;
  tableListType *ptr = tableList->findTable(t);

  if ((ptr != NULL) && (ptr->curRow != NULL))
    if ((q = ptr->curRow->getNode(_first_child,columnIndex)) != NULL)
      q->setValue(value);
}

void Document::setTableValue(Node *t, const char *columnName, const char *value)
{
  int i, len;
  Node *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    for (i = 0, len = ptr->colInfo.size(); (i < len) && strcmp((char *) ptr->colInfo[i],columnName); i++);

    if ((i < len) && (ptr->curRow != NULL))
      if ((q = ptr->curRow->getNode(_first_child,i)) != NULL)
        q->setValue(value);
  }
}

const char *Document::getTableValue(Node *t, int rowIndex, int columnIndex) const
{
  Node *p, *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    if ((p = ptr->tableRoot->getNode(_first_child,rowIndex)) != NULL)
      if ((q = p->getNode(_first_child,columnIndex)) != NULL)
        return q->getValue();

  return NULL;
}

const char *Document::getTableValue(Node *t, int rowIndex, const char *columnName) const
{
  int i, len;
  Node *p, *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    for (i = 0, len = ptr->colInfo.size(); (i < len) && strcmp((char *) ptr->colInfo[i],columnName); i++);

    if ((i < len) && ((p = ptr->tableRoot->getNode(_first_child,rowIndex)) != NULL))
      if ((q = p->getNode(_first_child,i)) != NULL)
        return q->getValue();
  }
  
  return NULL;
}

const char *Document::getTableValue(Node *t, int columnIndex) const
{
  Node *p;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
    if ((p = ptr->curRow->getNode(_first_child,columnIndex)) != NULL)
      return p->getValue();
  
  return NULL;
}

const char *Document::getTableValue(Node *t, const char *columnName) const
{
  int i, len;
  Node *q;
  tableListType *ptr = tableList->findTable(t);

  if (ptr != NULL)
  {
    for (i = 0, len = ptr->colInfo.size(); (i < len) && strcmp((char *) ptr->colInfo[i],columnName); i++);

    if ((i < len) && (ptr->curRow != NULL))
      if ((q = ptr->curRow->getNode(_first_child,i)) != NULL)
        return q->getValue();
  }
  
  return NULL;
}

const char *Document::getCircTableValue(Node *t, int displacement, int columnIndex) const
{
  Node *q;
  
  if ((q = getCircTableRow(t,displacement)) != NULL)
    if ((q = q->getNode(_first_child,columnIndex)) != NULL)
      return q->getValue();
  
  return NULL;
}

const char *Document::getCircTableValue(Node *t, int displacement, const char *columnName) const
{
  int i, len;
  Node *q;
  tableListType *ptr = tableList->findTable(t); 

  if ((q = getCircTableRow(t,displacement)) != NULL)
  {
    for (i = 0, len = ptr->colInfo.size(); (i < len) && strcmp((char *) ptr->colInfo[i],columnName); i++);

    if ((i < len) && ((q = q->getNode(_first_child,i)) != NULL))
      return q->getValue();
  }

  return NULL;
}

Comment *Document::attachComment(const char *value, Node *p, NodeLinkType where)
{
  Node *ptr = ((p == NULL)? root : p);
  Comment *new_ptr = NULL;

  ObjManager<Node> *buf_ptr;

  if (where != _parent)
  {
    new_ptr = new (nodeObjManager,buf_ptr) Comment(buf_ptr,attrObjManager,value);

    ptr->attachNode((Node *) new_ptr,where);
  }

  return new_ptr;
}  

Node *Document::cloneNode(Node *source, Document *dest)
{
  unsigned int i;
  ObjManager<Node> *bufNode;
  ObjManager<Attribute> *bufAttr;
  Node *newNode, *nodePtr;
  Attribute *attrPtr, *newAttr, *lastAttr;
  tableListType *tablePtr, *newTablePtr;
  
  newNode = new (dest->nodeObjManager,bufNode) Node(bufNode,dest->attrObjManager,source->getType(),source->getName(),source->getValue());
  if ((tablePtr = tableList->findTable(source)) != NULL)
  {
    dest->makeTable(newNode,tablePtr->withNumOfRows,tablePtr->rows);
    newTablePtr = dest->tableList->findTable(newNode);
    newTablePtr->effRows = tablePtr->effRows;
    newTablePtr->rowPtr = tablePtr->rowPtr;

    for (i = 0; i < tablePtr->colInfo.size(); i++)
      newTablePtr->colInfo.push_back(tablePtr->colInfo[i]);
  }  

  attrPtr = source->getAttribute();
  lastAttr = NULL;
  while (attrPtr != NULL)
  {
    newAttr = new (dest->attrObjManager,bufAttr) Attribute(bufAttr,attrPtr->getName(),attrPtr->getValue());
    if (lastAttr == NULL)
      newNode->attributes = newAttr;
    else
    {
      newAttr->prev = lastAttr;
      lastAttr->next = newAttr;
    }
    lastAttr = newAttr;
    attrPtr = attrPtr->next;
  }

  newNode->num_attributes = source->num_attributes;
  newNode->num_children = source->num_children;

  for (i = 0; i < source->num_children; i++)
  {
    nodePtr = cloneNode(source->getNode(_first_child,i),dest);
    newNode->attachNode(nodePtr);
  }

  return newNode;
}


/***********************************************************************\
* 					Class tableListType
\***********************************************************************/

tableListType::tableListType(tableListType *root, bool withNumOfRows)
{
  rowName = NULL;
  next = root;
  effRows = rowPtr = 0;
  curRow = NULL;
  this->withNumOfRows = withNumOfRows;
}

tableListType::~tableListType()
{
  colInfo.clear();
  
  if (next != NULL)
    delete next;
}

tableListType *tableListType::findTable(Node *p)
{
  tableListType *ptr = this;

  while ((ptr != NULL) && (ptr->tableRoot != p))
    ptr = ptr->next;

  return ptr;
}

void Document::clone(Document *doc)
{
  unsigned int i;
  Node *clonedTree;
  tableListType *tablePtr;
  
  for (i = 0; i < root->num_children; i++)
  {
    clonedTree = cloneNode(root->getNode(_first_child,i),doc);
    doc->root->attachNode(clonedTree,_last_child);
  }
  
  tablePtr = doc->tableList;
  while (tablePtr != NULL)
  {
    tablePtr->curRow = tablePtr->tableRoot->getNode(_first_child,tablePtr->rowPtr);
    tablePtr = tablePtr->next;
  }
}

