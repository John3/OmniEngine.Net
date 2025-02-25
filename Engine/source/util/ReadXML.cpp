//-----------------------------------------------------------------------------------------------------------------
// Copyright (C) 2013 WinterLeaf Entertainment LLC.
// 
// THE SOFTWARE IS PROVIDED ON AN “AS IS” BASIS, WITHOUT WARRANTY OF ANY KIND,
// INCLUDING WITHOUT LIMIT ATION THE WARRANTIES OF MERCHANT ABILITY, FITNESS
// FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT . THE ENTIRE RISK AS TO THE
// QUALITY AND PERFORMANCE OF THE SOFTWARE IS THE RESPONSIBILITY OF LICENSEE.
// SHOULD THE SOFTWARE PROVE DEFECTIVE IN ANY RESPECT , LICENSEE AND NOT LICENSOR 
// OR ITS SUPPLIERS OR RESELLERS ASSUMES THE ENTIRE COST OF ANY SERVICE AND
// REPAIR. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS 
// AGREEMENT. NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.


#include "util/ReadXML.h"

#include "console/consoleTypes.h"
#include "console/SimXMLDocument.h"
#include "console/engineAPI.h"

IMPLEMENT_CONOBJECT(ReadXML);

ConsoleDocClass( ReadXML,
				"@brief Class used for writing out preferences and settings for editors\n\n"
				"Not intended for game development, for editors or internal use only.\n\n "
				"@internal");

ReadXML::ReadXML(void)
{
	mFileName = "";
}


ReadXML::~ReadXML(void)
{
}

void ReadXML::initPersistFields()
{
	addField("fileName", TypeStringFilename, Offset(mFileName, ReadXML), "The file path and name to be saved to and loaded from.");

   Parent::initPersistFields();
}

bool ReadXML::readFile()
{
	SimXMLDocument *document = new SimXMLDocument();
	document->registerObject();
	bool output;
	if(document->loadFile(mFileName))
	{

		// set our base element
		if(document->pushFirstChildElement(getName()))
		{
			setModStaticFields( true );
			readLayer(document);
			output = true;
		}
		else
			output = false;
	}
	else
		output = false;

	document->unregisterObject();
	return output;
}

void ReadXML::readLayer(SimXMLDocument *document)
{
   for(S32 i=0; document->pushChildElement(i); i++)
   {
	  const UTF8 *type = document->elementValue();
	  const UTF8 *name = document->attribute("name");
	  const UTF8 *value = document->getText();
	  
	  static SimObject* object;
	  if(dStrcmp(type, "Group") == 0)
	  {
		  const UTF8 *fileName = document->attribute("fileName");
		  const S32 lineNumber = (dAtoi)(document->attribute("lineNumber"));
		  object = Sim::findObject(fileName, lineNumber);
		  if(object)
		  {
			  readLayer(document);
		  }
	  } 
	  else if(dStrcmp(type, "Setting") == 0)
	  {
		  const UTF8 *id = document->attribute("id");
		  if(object)
		  {
			  if( dStricmp(id, "") )
				  object->setDataField( name, id, value);
			  else
				  object->setDataField( name, NULL, value);
		  }
	  }
	  
	  document->popElement();
   }
}


DefineConsoleMethod(ReadXML, readFile, bool, (), , "readXMLObj.readFile();")
{   
   return object->readFile();
}


















































//---------------DNTC AUTO-GENERATED---------------//
#include <vector>

#include <string>

#include "core/strings/stringFunctions.h"

//---------------DO NOT MODIFY CODE BELOW----------//

extern "C" __declspec(dllexport) S32  __cdecl wle_fn_ReadXML_readFile(char * x__object)
{
ReadXML* object; Sim::findObject(x__object, object ); 
if (!object)
	 return 0;
bool wle_returnObject;
{   
   {wle_returnObject =object->readFile();
return (S32)(wle_returnObject);}
}
}
//---------------END DNTC AUTO-GENERATED-----------//

