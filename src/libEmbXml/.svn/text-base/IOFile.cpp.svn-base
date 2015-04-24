#include "libEmbXml/IOFile.h"
#include "libEmbXml/reader.h"
#include <stdarg.h>

FileWriter::FileWriter(const char* fileName) {
	xmlFile = fopen(fileName, "wb");
	init();
}

FileWriter::~FileWriter() {
	while (depth > 0 ) closeNode();
	fclose(xmlFile);
}

void FileWriter::attachNode(const char* name) {
	int i = 0;

	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	while(i++ < depth) fputc( '\t', xmlFile );

	/* Inserting node with name (no closing brackets) */
	fprintf(xmlFile, "<%s", name);

	openNodeList.push(name);
	depth++;
	nodeType = _EMPTY_NODE;
}

void FileWriter::attachPINode (const char* name) {
	int i = 0;
	
	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	while(i++ < depth) fputc( '\t', xmlFile );

	/* Inserting node with name (no closing brackets) */
	fprintf(xmlFile, "<?%s", name);
	
	openNodeList.push(name);
	depth++;
	nodeType = _PI_NODE;
}

void FileWriter::attachComment (const char* comment) {
	int i = 0;

	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	while(i++ < depth) fputc( '\t', xmlFile );

	/* Inserting node with name (no closing brackets) */
	fprintf(xmlFile, "<!-- %s -->\n", comment);
	
	nodeType = _FATHER_NODE;
}

void FileWriter::setValue(const char* value) {
	/* If no valueAdded yet closing node before inserting value */
	if(nodeType == _EMPTY_NODE) {
		fprintf(xmlFile, ">%s", value);
	}

	nodeType = _LEAF_NODE;
}

void FileWriter::setValue(int value) {
	/* If no valueAdded yet closing node before inserting value */
	if(nodeType == _EMPTY_NODE) {
		fprintf(xmlFile, ">%d", value);
	}

	nodeType = _LEAF_NODE;
}

void FileWriter::addAttribute(const char* name, const char* value) {
	/* Inserting attribute (current node will still be open) */
	/* FORMAT: att_name = "att_value" */

	fprintf(xmlFile, " %s=\"%s\"", name, value);
}

void FileWriter::addAttribute(const char* name, int value) {
	/* Inserting attribute (current node will still be open) */
	/* FORMAT: att_name = "att_value" */

	fprintf(xmlFile, " %s=\"%d\"", name, value);
}
	
void FileWriter::closeNode() {
	int i = 0;
	char* poppedName=NULL;
	
	/* Rolling back node n ame in openNodeList */
	poppedName = openNodeList.pop() ;
    if( ! poppedName )
        return ;

	/* If no valueAdded yet closing as empty node */
	switch (nodeType) {
		case _FATHER_NODE:
			/* Inserting  correct indentation (depending on length) */
			while(i++ < depth - 1) fputc( '\t', xmlFile );
		case _LEAF_NODE:
			fprintf(xmlFile, "</%s>\n", poppedName);
			break;
		case _EMPTY_NODE:
			fputs( "/>\n", xmlFile );			
			break;			
		case _PI_NODE:
			fputs( "?>\n", xmlFile );
			break;		
	}

	depth--;
	nodeType = _FATHER_NODE;
}


/***********************************************************************\
* 					Class TableFileWriter
\***********************************************************************/

/* Constructor using JOutPutStream as output system*/
TableFileWriter::TableFileWriter(const char* fileName, unsigned int columns) 
	: FileWriter(fileName), TableWriter(columns) 
{
    for( int i=0; i<128; ++i )
        tabs[i]='\t';
}

void TableFileWriter::addTable(const char* tableName, int numRows) {

	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	fwrite(tabs, 1, depth, xmlFile);

	if(numRows != -1) {
		// Inserting node with name (no closing brackets) 
		fprintf( xmlFile, "<%s numberOfRows=\"%d\"", tableName, numRows );
	}
	else {
		/* Inserting node with name (no closing brackets) */
		fprintf(xmlFile, "<%s", tableName);
	}
	
	openNodeList.push(tableName);
	depth++;

	nodeType = _EMPTY_NODE;
}

void TableFileWriter::addTableRow(const char *values, ...) {
	va_list list;
	unsigned int colNum = 0;
	char* cName ;
	
	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	fwrite(tabs, 1, depth, xmlFile);

	/* Inserting row name */
	fprintf(xmlFile, "<%s>\n", rowName);

	va_start(list,values);
	while (values && colNum < totalColumns) {

		/* Inserting  correct indentation (depending on length) */
		fwrite(tabs, 1, depth + 1, xmlFile);
			
		/* Inserting open node, cell value and closing node */
        cName = const_cast<char*>(names.getValue((char*) addresses[colNum]));
		fprintf( xmlFile, "<%s>%s</%s>\n", cName, values, cName );
        ++colNum;
		values = (char *) va_arg(list, char *);		
	}
	va_end(*((va_list*)(&values)));

	/* Inserting  correct indentation (depending on length) */
	fwrite(tabs, 1, depth, xmlFile);

	/* Inserting cell closing name */
	fprintf(xmlFile, "</%s>\n", rowName);	

	nodeType = _FATHER_NODE;
}

void TableFileWriter::addCell(const char* colName, const char* cellValue) {
	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	fwrite(tabs, 1, depth + 1, xmlFile);

	fprintf(xmlFile, "<%s>%s</%s>", colName, cellValue, colName);

	nodeType = _FATHER_NODE;
}

void TableFileWriter::addCell(const char* colName, int cellValue) {
	if(nodeType == _EMPTY_NODE) {
		fputs( ">\n", xmlFile );
	}

	/* Inserting  correct indentation (depending on length) */
	fwrite(tabs, 1, depth + 1, xmlFile);

	fprintf(xmlFile, "<%s>%d</%s>", colName, cellValue, colName);

	nodeType = _FATHER_NODE;
}


/***********************************************************************\
* 				Class DocFileWriter
\***********************************************************************/

DocFileWriter::DocFileWriter( const char* fileName ) : FileWriter( fileName ) {
  doc = NULL ;
  }

DocFileWriter::~DocFileWriter() {}

void DocFileWriter::addAttrName( const char* name ) {
	/* Inserting attribute (current node will still be open) */
	/* FORMAT: att_name = "att_value" */
	fprintf(xmlFile, " %s=", name);
}


void DocFileWriter::addAttrValue( const char* value ) {
	/* Inserting attribute (current node will still be open) */
	/* FORMAT: att_name = "att_value" */  
  fprintf(xmlFile, "\"%s\"", value);
}

void DocFileWriter::writeChildren( const Node* node ) {
  Node* child = node->getNode(_first_child) ;
  NodeType type ;
  tableListType* table = NULL ;
  while( child ) {
    type = child->getType() ;
    switch( type ) {
      case _tag :
        attachNode( child->getName() ) ;
        if( doc->tableList )
          table = doc -> tableList -> findTable( (Node*) child ) ;
        if( table && table->withNumOfRows ) {
          addAttribute( "numberOfRows" , table -> effRows ) ;
          table = NULL ;
          }
        break ;	  
	  case _decl :
        attachPINode( child->getName() ) ;
        break ;
      case _comment :
        attachComment( child->getValue() ) ;
        break ;
      default :
        attachNode( child->getName() ) ;
        break ;
      } 
/*
    if( child->getType() == _decl )
      attachPINode( child->getName() ) ;
    else
      attachNode( child->getName() ) ;
*/
    Attribute* actualAttr = child->getAttribute() ;
    while( actualAttr ) {
      addAttrName( actualAttr->getName() ) ;
      addAttrValue( actualAttr->getValue() ) ;
      actualAttr = actualAttr->getNext() ;
      }
    if( type != _decl && type != _comment ) {
      const char *stringBuffer = child->getValue() ;
      if( strlen( stringBuffer ) )
        setValue( stringBuffer ) ;
      writeChildren( child ) ;
      }
    
	if (type != _comment) closeNode() ;
	
    child=child->getNode(_next) ;
    }
  }


void DocFileWriter::write( const Document* doc ) {
  this -> doc = const_cast<Document*>(doc) ;
  writeChildren( doc->getRootNode() ) ;
  }

void DocFileWriter::operator<<( const Document* doc ) {
  this -> doc = const_cast<Document*>(doc) ;
  writeChildren( doc->getRootNode() ) ;
  }
  

/***********************************************************************\
* 				Class DocFileReader
\***********************************************************************/

DocFileReader::DocFileReader(const char* fileName, unsigned int data_buf_size)
	: DocReader(data_buf_size) {
	xmlFileName = new char[ strlen(fileName) + 1 ];
    strcpy( xmlFileName, fileName );
}

DocFileReader::~DocFileReader() {
	delete [] xmlFileName;
}

void DocFileReader::parse(Document* _xmldoc) {
	int bytesRead, i;
    FILE *file;

	xmldoc = _xmldoc;
	currentNode = xmldoc->getRootNode();

    file=fopen(xmlFileName, "rb");

    if( ! file )
        throw XMLException( NO_FILE );

	do 
    {
		/* Reading up to bufferSize new chars */
		bytesRead = fread(readBuf, 1, XML_READER_BUFFER_SIZE, file);

		/* Processing byte by byte */
		for(i = 0; i < bytesRead; ++i) 
            process(readBuf[i]);

	}
    while (bytesRead);
    
    fclose( file );
}


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
