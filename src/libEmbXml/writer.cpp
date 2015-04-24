#include "libEmbXml/writer.h"

/***********************************************************************\
* 					Class Writer
\***********************************************************************/

void Writer::init(void) {
	depth = 0;
	nodeType = _FATHER_NODE;
}


/***********************************************************************\
* 					Class TableWriter
\***********************************************************************/

TableWriter::TableWriter(unsigned int columns) : names(512) {
	for(int i = 0; i < TABS_NUM; i++) tabs[i] = '\t';
	
	if (columns <= 10) 	addresses = cols;
	else 				addresses = new char*[ columns ];

	totalColumns = columns;
}

TableWriter::~TableWriter() {
	/* If addresses allocated in heap freeing memory */
	if (totalColumns > 10) delete addresses;
}

void TableWriter::setTableRowName(const char* _rowName) {
	rowName = const_cast<char*>(names.push(_rowName));
}

void TableWriter::setTableColName(int col, const char* _colName) {
	addresses[col] = const_cast<char*>(names.push(_colName));
}


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
