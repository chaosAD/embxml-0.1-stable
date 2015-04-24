#ifndef __WRITER__
#define __WRITER__

#include <stdio.h>

#include "libEmbMem/charBuffer.h"
#include "libEmbXml/document.h"

#define TABS_NUM        128

#define _EMPTY_NODE      0x00
#define _FATHER_NODE 	0x01
#define _LEAF_NODE       0x02
#define _PI_NODE         0x03

class Writer {
	protected:
		long depth;
		CharBuffer openNodeList;
		char nodeType;
		
		/* Common intialization code */
		void init(void);
		
	public:
    Writer() : openNodeList(512) {}
		virtual ~Writer() {}
		virtual void attachNode (const char* name) = 0;
		virtual void attachPINode (const char* name) = 0;
		virtual void attachComment (const char* comment) = 0;
		virtual void addAttribute (const char* name, const char* value) = 0;
		virtual void addAttribute (const char* name, int value) = 0;
		virtual void setValue (const char* value) = 0;
		virtual void setValue (int value) = 0;
		virtual void closeNode () = 0;
};


/***********************************************************************\
* 					Class TableWriter
\***********************************************************************/

class TableWriter {
	protected:
		char tabs[TABS_NUM];
		CharBuffer names;
		char * rowName;		
		char* cols[10];		/* First 10 column name pointers are held directly inside object (no malloc necessary) */
		char** addresses;	/* Other column name pointers are held outside (malloc necessary) */
		unsigned int totalColumns;

	public:
		TableWriter(unsigned int columns);
		virtual ~TableWriter();

		void setTableRowName(const char *rowName = "TableRow");
		void setTableColName(int col, const char* _colName = "TableColumn");

		virtual void addTable(const char* tableName, int numRows) = 0;  /* if numRows == -1 (default value) --> attribute "numberOfRows" is not added */
		virtual void addTableRow(const char *values, ...) = 0;
		virtual void addCell(const char* colName, const char* cellValue) = 0;
		virtual void addCell(const char* colName, int cellValue) = 0;
};


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
#endif
