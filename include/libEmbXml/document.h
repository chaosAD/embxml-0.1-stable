#ifndef __DOCUMENT__
#define __DOCUMENT__

#include <vector>
#include "libEmbMem/objManager.h"

using namespace std ;

enum NodeLinkType { _parent = 0, _prev = 1, _next = 2, _first = 3, _last = 4, _first_child = 5, _last_child = 6 };
enum NodeType { _decl = 0, _tag = 1, _root = 2, _comment = 3 };

/*
                            XML Tree Internal Structure

 Level (k-1)-th                  Level kth                          Level (k+1)-th




                                   NULL
     ^                              ^
     |                              |

    Parent                         First
    /----\   First Child           /----\
    |    |  ----->------->-------> |    | ->
 <- |    |  <-----------        <- |    | <-
    |    |  -->-       |           |    | ->
    \----/     |       |           \----/
        | \    |       |            ^    \
        v  \   v       ^            |  |  \
               |       |            |  |
              L|       |               v
              a|       |            ....
              s|       |            ^
              t|       |            |  |
               |       |            |  |
              C|       |               v
              h|       |            Prev
              i|       |           /----\
              l|       |           |    | ->
              d|       |         <-|    | <-
               |       ^           |    | ->
               |       |           \----/
               |       |            ^    \
               v       |            |  |  \
               |       |            |  |
               |       |               v
               |       |        You are here
               |       |           /----\
               |       |  Parent   |XXXX| ----------> First Child
               |       |------<--- |XXXX| <-
               |                   |XXXX| ----------> Last Child
               |                   \----/
               |                    ^    \
               |                    |  |  \
               |                    |  |   \
               |                       v    \
               |                    Next     \ Attributes
               |                   /----\     \-------------> /====\  next  /====\                    /====\
               |                   |    |->                   ~    ~ -----> ~    ~ -----> .... -----> ~    ~ -->-- NULL
               |                 <-|    |<-            --<--- ~    ~ <----  ~    ~ <----  .... <----  ~    ~
               |                   |    |->            |      \====/  prev  \====/                    \====/
               v                   \----/              v
               |                    ^    \             NULL
               |                    |  |  \
               |                    |  |
               |                       v
               |                    ....
               |                    ^
               |                    |  |
               |                    |  |
               |                       v
               |                    Last
               |                   /----\
               |                   |    |->
               |                 <-|    |<-
               |---->------------> |    |->
                                   \----/
                                       | \
                                       |  \
                                       v
                                      NULL

TAG TYPES:
  _decl: create a declaration tag (ex. <? ... >)
  _tag: create a standard tag (ex. <....>...</....>, <..../>)
  _root: reserved for internal use

*/


/***********************************************************************\
* 					Class Attribute
\***********************************************************************/
class Attribute
{
private:
  char *name;
  char *value;
  Attribute *prev;
  Attribute *next;

  ObjManager<Attribute> *effectiveObjManager ;
  
  void *operator new(size_t size, ObjManager<Attribute> *&ptr, ObjManager<Attribute> *&effectiveObjManager);
  void operator delete(void *p);

public:
  friend class Node;
  friend class Document;
  
  Attribute(ObjManager<Attribute> *&effObjManager, const char *n, const char *v = "");
  ~Attribute();
    
  Attribute *getNext()
  {
    return next;
  }
  
  Attribute *getPrev()
  {
    return prev;
  }
  
  const char *getName()
  {
    return effectiveObjManager->nameBuffer->getValue(name);
  }
  
  const char *getValue()
  {
    return effectiveObjManager->valueBuffer->getValue(value);
  }

  void setValue( const char* val ) {
    value = const_cast<char*>(effectiveObjManager->valueBuffer->push( val ) ) ;
  }

};


/***********************************************************************\
* 					Class Node
\***********************************************************************/

class Node
{
private:
  Node *prev;
  Node *next;
  Node *first_child;
  Node *last_child;
  Node *parent;

  Attribute *attributes;
  char *name;
  char *value;
  NodeType type;

  Node *lkUpN;

  int num_attributes;
  unsigned int num_children;

  ObjManager<Node> *effectiveNodeObjManager;
  ObjManager<Attribute> *attrObjManager;
  
  void *operator new(size_t size, ObjManager<Node> *&ptr, ObjManager<Node> *&effectiveObjManager);

protected:
  void operator delete(void *p);
  void unlinkNode();

public:
  friend class Document;
  
  Node(ObjManager<Node> *&effNodeObjManager, ObjManager<Attribute> *&effAttrObjManager, NodeType t = _tag, const char *n = "", const char *v = "");
  virtual ~Node();

  /* Attaches node p to "where" branch */
  virtual void attachNode(Node *p, NodeLinkType where = _last_child);

  /* Gets node attached to "where" branch, at idx-th position. */
  /* If idx > 0, "where" branch will be walked down */
  /* If idx < 0, "where" branch will be walked up   */
  /* If "where" is _parent, idx will be ignored */
  Node *getNode(NodeLinkType where = _first_child, int idx = 0) const;
  Attribute *getAttribute(int idx = 0) const;

  /* Handles name/value of node */
  const char *getName() const;
  const char *getValue() const;

  virtual void setName(const char *n);
  void setValue(const char *v = "");

  /* Handles attributes */
  virtual Attribute* addAttribute(const char *n, const char *v = "");
  void deleteAttribute(const char *n);
  void deleteAttribute(int idx = 0);
  const char *getAttrName(int idx = 0) const;
  const char *getAttrValue(const char *n) const;
  const char *getAttrValue(int idx = 0) const;
  virtual void setAttrValue(const char *n, const char *v = "");
  virtual void setAttrValue(int idx = 0, const char *v = "");

  NodeType getType() const;

  /* Searches a child node named as "n" */
  /* Search proceeds from the last node found, if called again */
  /* It continues circularly and returns NULL only if no children named as "n" found */
  virtual Node *lookUpNode(const char *n);

  int numAttributes() const;

  /* Returns nodes' number at (k+1)-th level */
  int numChildren() const;
};


/***********************************************************************\
* 					Class Comment
\***********************************************************************/

class Comment : public Node
{
public:
  Comment(ObjManager<Node> *&effNodeObjManager, ObjManager<Attribute> *&effAttrObjManager, const char *v);
  ~Comment();
  
  void attachNode(Node *p, NodeLinkType where = _last_child);
  //void setName(const char *n);
  Attribute* addAttribute(const char *n, const char *v = "");
  void setAttrValue(const char *n, const char *v = "");
  void setAttrValue(int idx = 0, const char *v = "");
  Node *lookUpNode(const char *n);
};


/***********************************************************************\
* 					Class tableListType
\***********************************************************************/
class tableListType
{
  Node *tableRoot;
  int rows;
  int effRows;
  int rowPtr;
  char *rowName;
  vector<char*> colInfo;
  Node *curRow;
  tableListType *next;
  tableListType *findTable(Node *p);
  bool withNumOfRows;
  
public:
  friend class Document;
  friend class DocFileWriter;
  tableListType(tableListType *root, bool withNumOfRows);
  ~tableListType();
};


/***********************************************************************\
* 					Document
\***********************************************************************/
class Document
{
  Node *root;
  
  ObjManager<Attribute> *attrObjManager;
  ObjManager<Node> *nodeObjManager;

  tableListType *tableList;
  
  Node *getCircTableRow(Node *t, int displacement) const;

public:
  friend class DocFileWriter;

  Document(int nodes = 128, int totalNumOfAttr = 32);
  ~Document();

  /* Returns first node of XML document */
  Node *getRootNode() const;
  
  Node *attach(const char *name, const char *value = "", Node *p = (Node *) NULL, NodeType t = _tag, NodeLinkType where = _last_child);
  Node *inject(const char *name, const char *value = "", Node *p = (Node *) NULL, NodeType t = _tag);
  Comment *attachComment(const char *value, Node *p = (Node*) NULL, NodeLinkType where = _last_child);
  
  void remove(Node *p);
  
  void makeTable(Node *t, bool withNumOfRows = true, int rows = 0);
  void addTableRow(Node *t);
  void addTableRow(Node *t, const char *values, ...);
  void setTableRows(Node *t, int rows);
  void setTableRowName(Node *t, const char *n = "tr");
  void setTableColName(Node *t, int col, const char *n = "td");

  int getTableRows(Node *t);
  int getTableCols(Node *t);
  
  Node *getTableNode(Node *t, int rowIndex, int columnIndex);
  Node *getTableNode(Node *t, int columnIndex);
  Node *getCircTableNode(Node *t, int displacement, int columnIndex);
  
  void setTableValue(Node *t, int rowIndex, int columnIndex, const char *value);
  void setTableValue(Node *t, int rowIndex, const char *columnName, const char *value);
  void setTableValue(Node *t, int columnIndex, const char *value);
  void setTableValue(Node *t, const char *columnName, const char *value);

  const char *getTableValue(Node *t, int rowIndex, int columnIndex) const;
  const char *getTableValue(Node *t, int rowIndex, const char *columnName) const;
  const char *getTableValue(Node *t, int columnIndex) const;
  const char *getTableValue(Node *t, const char *columnName) const;
  const char *getCircTableValue(Node *t, int displacement, int columnIndex) const;
  const char *getCircTableValue(Node *t, int displacement, const char *columnName) const;
  
  void clean();
  
  Node *cloneNode(Node *source, Document *dest);
  void clone(Document *doc);
};


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
#endif

