#ifndef ___READER__
#define ___READER__

#include "libEmbXml/document.h"

// Error types for Xml Parser
//
#define NO_FILE                         0x00
#define SYNTAX_ERROR					0x01
#define NODE_TAG_MISMATCH				0x02
#define PARSER_STATE_MACHINE_ERROR		0x03
//

#define XML_READER_BUFFER_SIZE 1024

class XMLException 
{
	private:
		unsigned long line;
        unsigned char errorType;
	public:
		XMLException(char errorType, unsigned long line = 0) ;
		unsigned long getErrorLine();
		char const* getErrorType();
};


class DocReader {
	private:
		CharBuffer data_buf;
		
		/* State Machines */
		char SM_GLOBAL;
		char SM_OUTSIDE;
		char SM_INSIDE;
		
		char* tempPointer;
		int tempCounter;

	protected:
		/* Resulting XML document */
		Node* currentNode;		
		Document* xmldoc;
		char readBuf[XML_READER_BUFFER_SIZE];
		unsigned long parsedLine;
		void process(char c);
		
	public:
		DocReader(unsigned int data_buf_size);
		virtual ~DocReader();

		virtual void parse(Document* _xmldoc) = 0;
};

/*****************************************************************************
 *                               End of file
 *****************************************************************************/ 
#endif
