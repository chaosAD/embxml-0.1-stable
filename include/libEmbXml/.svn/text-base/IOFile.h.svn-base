#ifndef __IOFILE__
#define __IOFILE__

#include "libEmbXml/writer.h"
#include "libEmbXml/reader.h"

/***********************************************************************\
* 					Class FileWriter
\***********************************************************************/

class FileWriter: public Writer {
	protected:
		FILE* xmlFile;					/* File descriptor */
	public:
		FileWriter(const char* fileName);
		virtual ~FileWriter();
		void attachNode (const char* name);
		void attachPINode (const char* name);
		void attachComment (const char* comment);
		void addAttribute (const char* name, const char* value);
		void addAttribute (const char* name, int value);
		void setValue (const char* value);
		void setValue (int value);
		void closeNode ();
};


/***********************************************************************\
* 				Class DocFileWriter
\***********************************************************************/

class DocFileWriter: private FileWriter {
  private:
    Document* doc ;
    void writeChildren( const Node* node ) ;
    
    void addAttrName( const char* name ) ;
	  void addAttrValue( const char* value ) ;
    
  public:
    DocFileWriter( const char* fileName ) ;
    ~DocFileWriter() ;
    void write( const Document* doc ) ;
    
    void operator<<( const Document* doc ) ;

};


/***********************************************************************\
* 					Class TableFileWriter
\***********************************************************************/

class TableFileWriter: public FileWriter, public TableWriter {
	private:
		char tabs[128];
	public:
		TableFileWriter(const char* fileName, unsigned int columns);

		void addTable(const char* tableName, int numRows = -1);
		void addTableRow(const char *values, ...);
		void addCell(const char* colName, const char* cellValue);
		void addCell(const char* colName, int cellValue);
};

/***********************************************************************\
* 					Class DocFileReader
\***********************************************************************/

class DocFileReader: public DocReader {
	private:
		char *xmlFileName;	/* Access through buffered file */
	public:
		DocFileReader(const char* fileName, unsigned int data_buf_size = 512);
		~DocFileReader();	
		void parse(Document* _xmldoc);
};


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
#endif
