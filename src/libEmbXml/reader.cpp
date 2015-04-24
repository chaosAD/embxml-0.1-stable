#include <ctype.h>

#include "libEmbXml/reader.h"
#include "libEmbXml/document.h"

#define DATA_BUF_SIZE			500

/* SM_GLOBAL (global state machine) */

#define __INSIDE_TAG			0x00	/* We are inside an XML tag */
#define __OUTSIDE_TAG			0x01	/* We are outside an XML tag */

/* SM_OUTSIDE (state machine inside __OUTSIDE_TAG state ) */

#define __OUTSIDE_ALL			0x00	/* We are not reading a node value, just meaningless formatting chars between tags (tabs, new lines, spaces) */
#define __NODE_VALUE			0x01	/* We are reading the content of a node value */

/* SM_INSIDE (state machine inside __INSIDE_TAG state ) */

#define __TAG_BEGIN						0x00

#define __AWAITING_NODE_NAME			0x01
#define __NODE_NAME						0x02
#define __AWAITING_ATTRIBUTE_NAME		0x03
#define __ATTRIBUTE_NAME				0x04
#define __END_ATTRIBUTE_NAME			0x05

#define __AWAITING_ATTRIBUTE_VALUE		0x06
#define __ATTRIBUTE_VALUE				0x07
#define __END_ATTRIBUTE_VALUE			0x08

#define __CHECK_CLOSING_TAG				0x09
#define __TAG_END						0x0A

#define __COMMENT_TAG1					0x0B
#define __COMMENT_TAG2					0x0C
#define __COMMENT						0x0D

#define __COMMENT_TAG_END1				0x0E
#define __COMMENT_TAG_END2				0x0F



/***********************************************************************\
* 				Class XMLException
\***********************************************************************/
XMLException::XMLException(char _errorType, unsigned long _line) : errorType ( _errorType ), line(_line) {}


char const* XMLException::getErrorType() {
	switch (errorType) {
	case NO_FILE:
		return "NO FILE";
	case SYNTAX_ERROR:
		return "SYNTAX ERROR";
	case NODE_TAG_MISMATCH:
		return "TAG MISMATCH ERROR";
	case PARSER_STATE_MACHINE_ERROR:
		return "XML PARSER STATE MACHINE ERROR";
	default:
		return "UNKNOWN ERROR";
	}
}


unsigned long XMLException::getErrorLine() { return line; }



/***********************************************************************\
* 				Class DocReader
\***********************************************************************/

DocReader::DocReader(unsigned int data_buf_size) : data_buf(data_buf_size) {

	/* State Machines */
	SM_GLOBAL  = __OUTSIDE_TAG;
	SM_OUTSIDE = __TAG_BEGIN;
	SM_INSIDE  = __NODE_NAME;

	tempPointer = NULL;

	parsedLine = 1;
}

DocReader::~DocReader() {}

void DocReader::process(char c) {
	switch (SM_GLOBAL) {
		case __OUTSIDE_TAG:

			switch (c) {
				case '<':
					switch ( SM_OUTSIDE ) {
						case __NODE_VALUE:
							/* Was reading a node value, completing */
							data_buf.pushChar('\0');
							/* Popping save node value to temporary pointer */
							tempPointer = data_buf.pop();
							tempCounter = strlen(tempPointer);

							while ( ( tempPointer[tempCounter] == '\n' || tempPointer[tempCounter] == '\r' ||
									  tempPointer[tempCounter] == '\t' || tempPointer[tempCounter] == ' ') && tempCounter >= 0 )
									tempCounter--;
							tempPointer[tempCounter] = '\0';

							currentNode->setValue(tempPointer);
							tempPointer = NULL;
							SM_OUTSIDE = __OUTSIDE_ALL;
						case __OUTSIDE_ALL:
							/* Entering in XML Tag */
							SM_GLOBAL = __INSIDE_TAG;
							SM_INSIDE = __TAG_BEGIN;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case ' ': case '\t': case '\n': case '\r':
					switch (SM_OUTSIDE) {
						case __OUTSIDE_ALL:
							/* Ignoring formatting characters */
							break;
						case __NODE_VALUE:
							/* Continuing to save characters in value field */
							data_buf.pushChar(c);
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				default:
					/* Non formatting characters, beginning to save node value */
					data_buf.pushChar(c);
					SM_OUTSIDE = __NODE_VALUE;
			}
			break;

		case __INSIDE_TAG:

			switch (c) {
				case '<':
					throw XMLException( SYNTAX_ERROR, parsedLine );

				case '>':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __NODE_NAME:
							data_buf.pushChar('\0');
							currentNode->setName(data_buf.pop());						
						case __AWAITING_ATTRIBUTE_NAME:
							if( currentNode->getType() == _decl )
								throw XMLException( SYNTAX_ERROR, parsedLine );

							SM_GLOBAL = __OUTSIDE_TAG;
							SM_OUTSIDE = __OUTSIDE_ALL;
							break;
						case __CHECK_CLOSING_TAG:
							data_buf.pushChar('\0');
							/* Checking if closed tag matches last open node */
							if( strcmp(data_buf.pop(), currentNode->getName()) != 0) {
								throw XMLException( NODE_TAG_MISMATCH, parsedLine );
							}
							currentNode = currentNode->getNode(_parent);
						case __END_ATTRIBUTE_VALUE:
						case __TAG_END:
							SM_GLOBAL = __OUTSIDE_TAG;
							SM_OUTSIDE = __OUTSIDE_ALL;
							break;
						case __ATTRIBUTE_NAME:
						case __AWAITING_ATTRIBUTE_VALUE:
						case __ATTRIBUTE_VALUE:
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT_TAG_END2:
							data_buf.pushChar('\0');
							currentNode->setValue(data_buf.pop());
							currentNode = currentNode->getNode(_parent);

							SM_GLOBAL = __OUTSIDE_TAG;
							SM_OUTSIDE = __OUTSIDE_ALL;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '=':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
						case __NODE_NAME:
						case __AWAITING_ATTRIBUTE_NAME:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_NAME:
							data_buf.pushChar('\0');
							SM_INSIDE = __AWAITING_ATTRIBUTE_VALUE;
							break;
						case __AWAITING_ATTRIBUTE_VALUE:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __END_ATTRIBUTE_VALUE:
						case __TAG_END:
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '"':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
						case __NODE_NAME:
						case __AWAITING_ATTRIBUTE_NAME:
						case __ATTRIBUTE_NAME:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __AWAITING_ATTRIBUTE_VALUE:
							SM_INSIDE = __ATTRIBUTE_VALUE;
							break;
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar('\0');
							currentNode->addAttribute( data_buf.getValue(tempPointer) , data_buf.pop() );
							/* Popping attribute name */
							data_buf.pop();
							tempPointer = NULL;
							SM_INSIDE = __END_ATTRIBUTE_VALUE;
							break;
						case __END_ATTRIBUTE_VALUE:
						case __TAG_END:
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '/':
					switch ( SM_INSIDE ) {
						/* Beginning node closing tag */
						case __TAG_BEGIN:
							/* Closing a node with values inside (closing tag) */
							SM_INSIDE = __CHECK_CLOSING_TAG;
							break;
						case __NODE_NAME:
						case __AWAITING_ATTRIBUTE_NAME:
						case __END_ATTRIBUTE_VALUE:
							/* Leaf Node, turning back one node */
							currentNode = currentNode->getNode(_parent);
							SM_INSIDE = __TAG_END;
							break;
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case ' ': case '\t':
					switch ( SM_INSIDE ) {
						case __NODE_NAME:
							data_buf.pushChar('\0');
							currentNode->setName(data_buf.pop());
							SM_INSIDE = __AWAITING_ATTRIBUTE_NAME;
							break;
						case __AWAITING_NODE_NAME:
						case __AWAITING_ATTRIBUTE_NAME:
						case __AWAITING_ATTRIBUTE_VALUE:
							break;
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __END_ATTRIBUTE_VALUE:
							SM_INSIDE = __AWAITING_ATTRIBUTE_NAME;
							break;
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '?':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
							currentNode = xmldoc->attach( "" , "" , currentNode , _decl , _last_child );
							SM_INSIDE = __AWAITING_NODE_NAME;
							break;
						case __NODE_NAME:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __AWAITING_ATTRIBUTE_NAME:
						case __END_ATTRIBUTE_VALUE:
							/* Closing PI node, after '?' cannot insert anything else */
							if(currentNode->getType() == _decl) {
								/* Leaf Node, turning back one node */
								currentNode = currentNode->getNode(_parent);
								SM_INSIDE = __TAG_END;
							}
							else
								throw XMLException( SYNTAX_ERROR, parsedLine );
							break;
						case __ATTRIBUTE_NAME:
						case __AWAITING_ATTRIBUTE_VALUE:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '!':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
							SM_INSIDE = __COMMENT_TAG1;
							break;
						case __AWAITING_NODE_NAME:
						case __NODE_NAME:
						case __AWAITING_ATTRIBUTE_NAME:
						case __ATTRIBUTE_NAME:
						case __END_ATTRIBUTE_NAME:
						case __AWAITING_ATTRIBUTE_VALUE:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __END_ATTRIBUTE_VALUE:
						case __CHECK_CLOSING_TAG:
						case __TAG_END:
						case __COMMENT_TAG1:
						case __COMMENT_TAG2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT:
							data_buf.pushChar(c);
							break;
						case __COMMENT_TAG_END1:
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						case __COMMENT_TAG_END2:
							data_buf.pushChar('-');
							data_buf.pushChar('-');
							data_buf.pushChar(c);
							SM_INSIDE = __COMMENT;
							break;
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				case '-':
					switch ( SM_INSIDE ) {
						case __TAG_BEGIN:
						case __AWAITING_NODE_NAME:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __NODE_NAME:
							data_buf.pushChar(c);
							break;
						case __AWAITING_ATTRIBUTE_NAME:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_NAME:
							data_buf.pushChar(c);
							break;
						case __END_ATTRIBUTE_NAME:
						case __AWAITING_ATTRIBUTE_VALUE:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __ATTRIBUTE_VALUE:
							data_buf.pushChar(c);
							break;
						case __END_ATTRIBUTE_VALUE:
						case __CHECK_CLOSING_TAG:
						case __TAG_END:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						case __COMMENT_TAG1:
							SM_INSIDE = __COMMENT_TAG2;
							break;
						case __COMMENT_TAG2:
							SM_INSIDE = __COMMENT;
							currentNode = xmldoc->attach( "" , "" , currentNode ,  _comment , _last_child );
							break;
						case __COMMENT:
							SM_INSIDE = __COMMENT_TAG_END1;
							break;
						case __COMMENT_TAG_END1:
							SM_INSIDE = __COMMENT_TAG_END2;
							break;
						case __COMMENT_TAG_END2:
							throw XMLException( SYNTAX_ERROR, parsedLine );
						default:
							throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
					}
					break;

				default:
					if( isalnum( c ) ) {
						switch ( SM_INSIDE ) {
							case __TAG_BEGIN:
								currentNode = xmldoc->attach( "" , "" , currentNode , _tag , _last_child );
							case __AWAITING_NODE_NAME:
								SM_INSIDE = __NODE_NAME;
							case __NODE_NAME:
							case __CHECK_CLOSING_TAG:
							case __ATTRIBUTE_NAME:
							case __ATTRIBUTE_VALUE:
								data_buf.pushChar(c);
								break;
							case __AWAITING_ATTRIBUTE_NAME:
								tempPointer = const_cast<char*>(data_buf.pushChar(c));
								SM_INSIDE = __ATTRIBUTE_NAME;
								break;
							case __AWAITING_ATTRIBUTE_VALUE:
							case __END_ATTRIBUTE_VALUE:
								throw XMLException( SYNTAX_ERROR, parsedLine );
							case __TAG_END:
								SM_GLOBAL = __OUTSIDE_TAG;
								break;
							case __COMMENT_TAG1:
							case __COMMENT_TAG2:
								throw XMLException( SYNTAX_ERROR, parsedLine );
							case __COMMENT:
								data_buf.pushChar(c);
								break;
							case __COMMENT_TAG_END1:
								data_buf.pushChar('-');
								data_buf.pushChar(c);
								SM_INSIDE = __COMMENT;
								break;
							case __COMMENT_TAG_END2:
								data_buf.pushChar('-');
								data_buf.pushChar('-');
								data_buf.pushChar(c);
								SM_INSIDE = __COMMENT;
								break;
							default:
								throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
						}
					}
					else {
						switch ( SM_INSIDE ) {
							case __TAG_BEGIN:
							case __NODE_NAME:
							case __AWAITING_ATTRIBUTE_NAME:
							case __ATTRIBUTE_NAME:
								if( c == ':' )	data_buf.pushChar(c);
								break;
							case __AWAITING_ATTRIBUTE_VALUE:
								throw XMLException( SYNTAX_ERROR, parsedLine );
							case __ATTRIBUTE_VALUE:
								data_buf.pushChar(c);
								break;
							case __END_ATTRIBUTE_VALUE:
							case __TAG_END:
								throw XMLException( SYNTAX_ERROR, parsedLine );
							case __COMMENT_TAG1:
							case __COMMENT_TAG2:
								throw XMLException( SYNTAX_ERROR, parsedLine );
							case __COMMENT:
								data_buf.pushChar(c);
								break;
							case __COMMENT_TAG_END1:
								data_buf.pushChar('-');
								data_buf.pushChar(c);
								SM_INSIDE = __COMMENT;
								break;
							case __COMMENT_TAG_END2:
								data_buf.pushChar('-');
								data_buf.pushChar('-');
								data_buf.pushChar(c);
								SM_INSIDE = __COMMENT;
								break;
							default:
								throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
						}
					}
				}
				break;
		default:
			throw XMLException( PARSER_STATE_MACHINE_ERROR, parsedLine );
	}
}


/*****************************************************************************
 *                               End of file
 *****************************************************************************/
