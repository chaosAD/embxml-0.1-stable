#include <libEmbXml/document.h>
#include <libEmbXml/IOFile.h>


/***********************************************************************\
* testXMLDomTree: this method builds a DOM tree in memory and outputs the
* result to file
\***********************************************************************/
void testXMLDomTree()
{
  int i = 0;
  Node *node1, *node2, *table;
  Document *doc;
  char line[255];

  doc = new Document(4000,0);

  node1 = doc->attach("xml","",NULL,_decl,_last_child);
  node1->addAttribute("version","1.0");

  doc->attachComment("This is a comment");

  node2 = doc->attach("bimbumbam");
  node2->addAttribute("xmlns","http://www.vuvuvu.it");
  node2->addAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
  node2->addAttribute("xsi:schemaLocation","http://www.vuvuvu.com/Description /home/zacmarco/Description.xsd");
  node2->addAttribute("address","10.0.0.1");
  node2->addAttribute("type","inutile");
  node2->addAttribute("version","1.0");

  table = doc->attach("customTable","",node2);
  doc->makeTable(table);
  doc->setTableRowName(table,"customRow");
  doc->setTableColName(table,0,"macAddress");
  doc->setTableColName(table,1,"aNumber");

  for (i = 0; i < 8000; i++)
  {
    doc->addTableRow(table);

    sprintf(line,"%d-%d-%d-%d-%d-%d",(i+1),(i+1),(i+1),(i+1),(i+1),(i+1));
    doc->setTableValue(table, 0, line);

    sprintf(line,"%d.%d.%d.%d",(i+1),(i+1),(i+1),(i+1));
    doc->setTableValue(table, 1, line);
  }

  DocFileWriter writer( "file1.xml" ) ;

  writer << doc ;

  delete doc ;
}


/***********************************************************************\
* testXMLDomAnyWhere: this method builds a DOM tree in memory and outputs the
* result to file, attaching nodes everywhere
\***********************************************************************/
void testXMLDomAnyWhere()
{
  int i = 0;
  Node *node1, *node2, *child;
  Document *doc;
  char line[255];
  DocFileWriter *writer;

  doc = new Document(20,0);

  node1 = doc->attach("xml","",NULL,_decl,_last_child);
  node1->addAttribute("version","1.0");

  node2 = doc->attach("reference");
  writer = new DocFileWriter("01.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("parent","",node2,_tag,_parent);
  writer = new DocFileWriter("02.xml");
  (*writer) << doc;
  delete writer;
  
  doc->attach("previous","",node2,_tag,_prev);
  writer = new DocFileWriter("03.xml");
  (*writer) << doc;
  delete writer;
  
  doc->attach("next","",node2,_tag,_next);
  writer = new DocFileWriter("04.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("first","",node2,_tag,_first);
  writer = new DocFileWriter("05.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("last","",node2,_tag,_last);
  writer = new DocFileWriter("06.xml");
  (*writer) << doc;
  delete writer;
  
  child = doc->attach("child","",node2,_tag,_first_child);
  writer = new DocFileWriter("07.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("nephew","",child,_tag);
  writer = new DocFileWriter("08.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("last_child","",node2,_tag,_last_child);
  writer = new DocFileWriter("09.xml");
  (*writer) << doc;
  delete writer;

  doc->attach("first_child","",node2,_tag,_first_child);
  writer = new DocFileWriter("10.xml");
  (*writer) << doc;
  delete writer;

  doc->inject("injected","",node2);
  writer = new DocFileWriter("11.xml");
  (*writer) << doc;
  delete writer;
  
  delete doc ;
}


/***********************************************************************\
*testXMLNodeCanc: this method builds a DOM structure with two nodes, deletes one
* and writes the rest to file. Testing node removal.
\***********************************************************************/
void testXMLNodeCanc() {

  int i = 0;
  Node *node1, *node2, *node3;
  Document *doc;
  char line[255];
  
  doc = new Document(500,0);

  node1 = doc->attach("xml","",NULL,_decl,_last_child);
  node1->addAttribute("version","1.0");

  node2 = doc->attach("testNode1");
  node2->addAttribute("attrib1","1000");

  node3 = doc->attach("testCanceledNode3","",node2,_tag,_last_child);
  
  doc->remove(node3);
  
  node3 = doc->attach("testCanceledNode4","",node2,_tag,_last_child);
  
  DocFileWriter writer( "testCanc.xml" ) ;

  writer << doc ;

  delete doc ;  
}


/***********************************************************************\
* testXMLTableWriter: this method builds an XML file containing a structured
* table (direct output, no memory consumption involved
\***********************************************************************/
void testXMLTableWriter()
{
	TableFileWriter xmlout("file2.xml", 3);  /* 3, number of table columns */
	
	xmlout.attachPINode("xml");
	xmlout.addAttribute("version", "1.0");
	xmlout.addAttribute("encoding", "UTF-8");
	xmlout.closeNode();
	
	xmlout.attachNode ("taaaable");

		xmlout.attachNode ("header");
			xmlout.addAttribute("attr1", "String1");
			xmlout.addAttribute("attr2", "String2");
		xmlout.closeNode();
		
		xmlout.addTable ("customTable", 4096);
		xmlout.setTableRowName("customRow");
		xmlout.setTableColName(0, "row1");		
		xmlout.setTableColName(1, "row2");
		xmlout.setTableColName(2, "row3");

        char val[32];

		for(unsigned int i = 0; i < 4096; ++i) 
        {
            sprintf( val, "%d-%d-%d-%d-%d-%d", i, i, i, i, i, i );
            xmlout.addTableRow(val, "2", "1");
        }
		
		xmlout.closeNode();
	xmlout.closeNode();		
}

/***********************************************************************\
* testXMLParser: this method parses an XML file and builds the corresponding DOM
* structure to memory, and finally tries to output it again to check correct implementation
\***********************************************************************/
void testXMLParser()
{
	Document* myXmlDoc = new Document();
	
	DocFileReader xmlin("file2.xml");

	try {
		xmlin.parse(myXmlDoc);
	} 
	catch ( XMLException xme) {
		printf("EXCEPTION! %s encountered \n", xme.getErrorType(), xme.getErrorLine() );
        delete myXmlDoc;
        return ;
	}

	DocFileWriter writer( "file3.xml" ) ;

	writer.write(myXmlDoc);
	
	delete myXmlDoc;
}

/***********************************************************************\
* testXMLCircularTable: this example builds an XML file containing
* circular tables
\***********************************************************************/
void testXMLCircularTable()
{
  Document *doc;
  Node *t1, *t2, *node, *row;
  int i;
  char col1[64], col2[64];

  doc = new Document;

  node = doc->attach("xml","",NULL,_decl,_last_child);
  node->addAttribute("version","1.0");

  node = doc->attach("myNode");
  node->addAttribute("address","10.0.0.1");
  node->addAttribute("version","1.0");

  /* This creates the root node of the table */
  t1 = doc->attach("myTable","",node);

  /* Circular table of 20 rows */
  doc->makeTable(t1,true,20);

  doc->setTableRowName(t1,  "row1");
  doc->setTableColName(t1,0,"row2");
  doc->setTableColName(t1,1,"row3");

  /* You should see only last 20 rows, written in circular way */
  for (i = 0; i < 35; i++)
  {
    sprintf(col1,"%d-%d-%d-%d-%d-%d",i+1,i+1,i+1,i+1,i+1,i+1);
    sprintf(col2,"%d.%d.%d.%d",i+1,i+1,i+1,i+1);

    doc->addTableRow(t1,col1,col2);
  }
  
  /* Hang tight, we're looping back fast!!! */
  row = doc->getCircTableNode(t1,10000,1);

  DocFileWriter *writer = new DocFileWriter("20-rows.xml");

  *writer << doc;
  
  delete writer;

  /* Now we shorten it to 10 rows. Last 10 rows will be saved */
  doc->setTableRows(t1,10);

  sprintf(col1,"%d-%d-%d-%d-%d-%d",i+1,i+1,i+1,i+1,i+1,i+1);
  sprintf(col2,"%d.%d.%d.%d",i+1,i+1,i+1,i+1);

  doc->addTableRow(t1,col1,col2);

  writer = new DocFileWriter("10-rows.xml");

  *writer << doc;
  
  delete writer;

  /* New table root node */
  t2 = doc->attach("newTable","",node);

  /* This is will be a standard table */
  doc->makeTable(t2,true);

  doc->setTableRowName(t2,  "newRow1");
  doc->setTableColName(t2,0,"newRow2");
  doc->setTableColName(t2,1,"newRow3");

  for (i = 0; i < 10; i++)
  {
    sprintf(col1,"%d-%d-%d-%d-%d-%d",i+1,i+1,i+1,i+1,i+1,i+1);
    sprintf(col2,"%d.%d.%d.%d",i+1,i+1,i+1,i+1);

    doc->addTableRow(t2,col1,col2);
  }

  writer = new DocFileWriter("full.xml");

  *writer << doc;
  
  delete writer;
  
  delete doc;
}

/***********************************************************************\
* testXMLClone: this example builds an XML-clone file
\***********************************************************************/
void testXMLClone()
{
  Document *doc, *clone;
  Node *t1, *node, *row;
  int i;
  char col1[64], col2[64];

  doc = new Document;

  node = doc->attach("xml","",NULL,_decl,_last_child);
  node->addAttribute("version","1.0");

  node = doc->attach("myNode");
  node->addAttribute("address","10.0.0.1");
  node->addAttribute("version","1.0");

  /* This creates the root node of the table */
  t1 = doc->attach("myTable","",node);

  /* Circular table of 20 rows */
  doc->makeTable(t1,true,20);

  doc->setTableRowName(t1,  "row1");
  doc->setTableColName(t1,0,"row2");
  doc->setTableColName(t1,1,"row3");

  /* You should see only last 20 rows, written in circular way */
  for (i = 0; i < 35; i++)
  {
    sprintf(col1,"%d-%d-%d-%d-%d-%d",i+1,i+1,i+1,i+1,i+1,i+1);
    sprintf(col2,"%d.%d.%d.%d",i+1,i+1,i+1,i+1);

    doc->addTableRow(t1,col1,col2);
  }
  
  DocFileWriter *writer = new DocFileWriter("source.xml");

  *writer << doc;
  
  delete writer;
  
  clone = new Document;
  doc->clone(clone);

  delete doc;
  
  writer = new DocFileWriter("clone.xml");

  *writer << clone;
  
  delete writer;
  
  delete clone;
}

/***********************************************************************\
* testXMLLookup: this example shows how to lookup for a node in a level
\***********************************************************************/
void testXMLLookup()
{
  Document *doc;
  Node *node;
  int i;
  char col1[64], col2[64];

  doc = new Document;

  node = doc->attach("xml","",NULL,_decl,_last_child);
  node->addAttribute("version","1.0");

  node = doc->attach("sampleXML");
  
  printf("No node has been found: %s\n",((node->lookUpNode("findme") == NULL)? "OK" : "ERROR"));
    
  doc->attach("findme","1st value",node);
  
  printf("A node has been found: %s\n",node->lookUpNode("findme")->getValue());
  printf("This should be the same: %s\n",node->lookUpNode("findme")->getValue());
  
  doc->attach("findme","2nd value",node);
  doc->attach("findme","3rd value",node);
  doc->attach("youcanalsofindme","my value",node);
  
  printf("Another node now: %s\n",node->lookUpNode("findme")->getValue());
  printf("One more: %s\n",node->lookUpNode("findme")->getValue());
  printf("Hey, we looped!: %s\n",node->lookUpNode("findme")->getValue());
  
  printf("What about a different node?: %s\n",node->lookUpNode("youcanalsofindme")->getValue());
  
  DocFileWriter *writer = new DocFileWriter("lookup.xml");

  *writer << doc;

  delete writer;

  delete doc;
}

void testXMLComment()
{

  int i = 0;
  Node *node1, *node2, *node3, *node4;
  Document *doc;
  char line[255];
  
  doc = new Document(500,0);

  node1 = doc->attach("xml","",NULL,_decl,_last_child);
  node1->addAttribute("version","1.0");

  node2 = doc->attach("testNode1");
  node2->addAttribute("attrib1","1000");

  doc->attach("testNode3","",node2,_tag,_last_child);
  
  doc->attachComment("Uh cummentu naschcostissimu!!!",node2,_last_child);  

  DocFileWriter writer("testComment.xml") ;

  writer << doc ;

  delete doc ;  
}



void testXMLDelete()
{
  Document *doc;
  Node *node;

  doc = new Document;

  node = doc->attach("xml","",NULL,_decl,_last_child);
  node->addAttribute("version","1.0");
  
  doc->remove(node);

  delete doc;
}



int main() 
{
    testXMLDomTree();
    testXMLDomAnyWhere();
    testXMLNodeCanc();
    testXMLTableWriter();
    testXMLParser();
    testXMLCircularTable();
    testXMLClone();
    testXMLLookup();
    testXMLComment();
    return 0;
}


