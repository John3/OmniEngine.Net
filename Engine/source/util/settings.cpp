//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "util/settings.h"
#include "console/engineAPI.h"
#include "console/consoleTypes.h"
#include "console/SimXMLDocument.h"

IMPLEMENT_CONOBJECT(Settings);

ConsoleDocClass( Settings,
				"@brief Class used for writing out preferences and settings for editors\n\n"
				"Not intended for game development, for editors or internal use only.\n\n "
				"@internal");

Settings::Settings()
{
   mFile = "";
	mSearchPos = 0;
}

Settings::~Settings()
{
   
}

void Settings::initPersistFields()
{
   addField("file", TypeStringFilename, Offset(mFile, Settings), "The file path and name to be saved to and loaded from.");

   Parent::initPersistFields();
}

void Settings::setDefaultValue(const UTF8 *settingName, const UTF8 *settingValue, const UTF8 *settingType)
{
   String baseName;
   buildGroupString(baseName, settingName);
   String name = baseName + "_default";
   StringTableEntry nameEntry = StringTable->insert(name.c_str());
   String type = baseName + "_type";
   StringTableEntry typeEntry = StringTable->insert(type.c_str());

   setModStaticFields(false);
   setDataField(nameEntry, NULL, settingValue);
   setDataField(typeEntry, NULL, settingType);
   setModStaticFields(true);
}

void Settings::setValue(const UTF8 *settingName, const UTF8 *settingValue)
{
   String name;
   buildGroupString(name, settingName);
   StringTableEntry nameEntry = StringTable->insert(name.c_str());

   setModStaticFields(false);
   setDataField(nameEntry, NULL, settingValue);
   setModStaticFields(true);
}

const UTF8 *Settings::value(const UTF8 *settingName, const UTF8 *defaultValue)
{
   String name;
   buildGroupString(name, settingName);

   StringTableEntry nameEntry = StringTable->insert(name.c_str());
   name += "_default";
   StringTableEntry defaultNameEntry = StringTable->insert(name.c_str());

   // we do this setModStaticFields call to make sure our get/set calls
   // don't grab a regular field, don't want to stomp anything
   setModStaticFields(false);  
   const UTF8 *value = getDataField(nameEntry, NULL);
   const UTF8 *storedDefaultValue = getDataField(defaultNameEntry, NULL);
   setModStaticFields(true);

   if(dStrcmp(value, "") != 0)
      return value;
   else if(dStrcmp(storedDefaultValue, "") != 0)
      return storedDefaultValue;
   else
	  return defaultValue;
}

void Settings::remove(const UTF8 *settingName, bool includeDefaults)
{
    // Fetch Dynamic-Field Dictionary.
    SimFieldDictionary* pFieldDictionary = getFieldDictionary();

    // Any Field Dictionary?
    if ( pFieldDictionary == NULL )
    {
        // No, so we're done.
        return;
    }

	 String name;
    buildGroupString(name, settingName);
    StringTableEntry nameEntry = StringTable->insert(name.c_str());
	 StringTableEntry nameEntryDefault = StringTable->insert( String::ToString("%s%s",name.c_str(), "_default") );

    // Iterate fields.
    for ( SimFieldDictionaryIterator itr(pFieldDictionary); *itr; ++itr )
    {
        // Fetch Field Entry.
        SimFieldDictionary::Entry* fieldEntry = *itr;

        // is this a field of our current group
        if ( (dStrcmp(nameEntry, "") == 0) || 
					dStrcmp( nameEntry, fieldEntry->slotName ) == 0 ||
					(includeDefaults && dStrcmp( nameEntryDefault, fieldEntry->slotName ) == 0) )
        {
            // Yes, so remove it.
            pFieldDictionary->setFieldValue( fieldEntry->slotName, "" );
        }
    }
}

void Settings::buildGroupString(String &name, const UTF8 *settingName)
{
   // here we want to loop through the stack and build a "/" seperated string
   // representing the entire current group stack that gets pre-pended to the
   // setting name passed in
   if(mGroupStack.size() > 0)
   {
      for(S32 i=0; i < mGroupStack.size(); i++)
	  {
		 S32 pos = 0;
		 if(name.size() > 0)
			pos = name.size()-1;

         // tack on the "/" in front if this isn't the first
		 if(i == 0)
		 {
	        name.insert(pos, mGroupStack[i]);
		 } else
		 {
			name.insert(pos, "/");
            name.insert(pos+1, mGroupStack[i]);
		 }
	  }

	  // tack on a final "/"
	  name.insert(name.size()-1, "/");
	  if(dStrlen(settingName) > 0)
	     name.insert(name.size()-1, settingName);
   } else
	  name = settingName;
}

void Settings::clearAllFields()
{
    // Fetch Dynamic-Field Dictionary.
    SimFieldDictionary* pFieldDictionary = getFieldDictionary();

    // Any Field Dictionary?
    if ( pFieldDictionary == NULL )
    {
        // No, so we're done.
        return;
    }

    // Iterate fields.
    for ( SimFieldDictionaryIterator itr(pFieldDictionary); *itr; ++itr )
    {
        // Fetch Field Entry.
        SimFieldDictionary::Entry* fieldEntry = *itr;

        // don't remove default field values
        if (dStrEndsWith(fieldEntry->slotName, "_default"))
           continue;

        // remove it.
        pFieldDictionary->setFieldValue( fieldEntry->slotName, "" );
    }
}

bool Settings::write()
{
   // Fetch Dynamic-Field Dictionary.
   SimFieldDictionary* pFieldDictionary = getFieldDictionary();

   // Any Field Dictionary?
   if ( pFieldDictionary == NULL )
   {
      // No, so we're done.
      return false;
   }

/*
   // Iterate fields.
   for ( SimFieldDictionaryIterator itr(pFieldDictionary); *itr; ++itr )
   {
      // Fetch Field Entry.
      SimFieldDictionary::Entry* fieldEntry = *itr;

	  String check(fieldEntry->slotName);
	  String::SizeType pos = check.find("_default");
	  if(pos != String::NPos)
		 continue;

      // let's build our XML doc
      document->pushNewElement("dynamicField");
	  document->setAttribute("name", fieldEntry->slotName);
	  document->addText(fieldEntry->value);
      document->popElement();
   }
*/
   SimXMLDocument *document = new SimXMLDocument();
   document->registerObject();
   document->addHeader();

   document->pushNewElement(getName());

   SettingSaveNode *node = new SettingSaveNode();
   // Iterate fields.
   for ( SimFieldDictionaryIterator itr(pFieldDictionary); *itr; ++itr )
   {
      // Fetch Field Entry.
      SimFieldDictionary::Entry* fieldEntry = *itr;

      String check(fieldEntry->slotName);
	  if(check.find("_default") != String::NPos || check.find("_type") != String::NPos)
		 continue;

	  node->addValue(fieldEntry->slotName, fieldEntry->value);
   }

   node->buildDocument(document, true);
   node->clear();
   delete node;

   bool saved = document->saveFile(mFile.c_str());
   document->deleteObject();

   if(saved)
      return true;
   else
	   return false;   
}

bool Settings::read()
{
   SimXMLDocument *document = new SimXMLDocument();
   document->registerObject();

   bool success = true;
   if(document->loadFile(mFile.c_str()))
   {
      clearAllFields();

      // set our base element
      if(document->pushFirstChildElement(getName()))
      {
         setModStaticFields(false);
         readLayer(document);
         setModStaticFields(true);
      }
      else
         success = false;
   }
   else
      success = false;

   document->deleteObject();

   return success;
}

void Settings::readLayer(SimXMLDocument *document, String groupStack)
{
   for(S32 i=0; document->pushChildElement(i); i++)
   {
	  const UTF8 *type = document->elementValue();
	  const UTF8 *name = document->attribute("name");
	  const UTF8 *value = document->getText();
	  
	  if(dStrcmp(type, "Group") == 0)
	  {
		 String newStack = groupStack;

		 if(!groupStack.isEmpty())
			newStack += "/";

		 newStack += name;
         readLayer(document, newStack);
	  } else if(dStrcmp(type, "Setting") == 0)
	  {
		 String nameString = groupStack;
         
		 if(!groupStack.isEmpty())
		    nameString += "/";

         nameString += name;
         setDataField(StringTable->insert(nameString.c_str()), NULL, value);
	  }
	  
	  document->popElement();
   }
}

void Settings::beginGroup(const UTF8 *groupName, bool fromStart)
{
   // check if we want to clear the stack
   if(fromStart)
      clearGroups();

   mGroupStack.push_back(String(groupName));
}

void Settings::endGroup()
{
   if(mGroupStack.size() > 0)
      mGroupStack.pop_back();
}

void Settings::clearGroups()
{
   mGroupStack.clear();
}

const UTF8 *Settings::getCurrentGroups()
{
   // we want to return a string with our group setup
   String returnString;
   for(S32 i=0; i<mGroupStack.size(); i++)
   {
	  S32 pos = returnString.size() - 1;
	  if(pos < 0)
		 pos = 0;

	  if(i == 0)
	  {
         returnString.insert(pos, mGroupStack[i]);
	  } else
	  {
		 returnString.insert(pos, "/");
         returnString.insert(pos+1, mGroupStack[i]);
	  }
   }

   return StringTable->insert(returnString.c_str());
}
/*
S32 Settings::buildSearchList( const char* pattern, bool deepSearch, bool includeDefaults )
{
	mSearchResults.clear();

	SimFieldDictionary* fieldDictionary = getFieldDictionary();
	// Get the dynamic field count
	if ( !fieldDictionary )
		return -1;

   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
	{
		// Fetch Field Entry.
      SimFieldDictionary::Entry* fieldEntry = *itr;
		
		// Compare strings, store proper results in vector
		String extendedPath = String::ToString(fieldEntry->slotName);
		String::SizeType start(0);
		String::SizeType slashPos = extendedPath.find('/', 0, String::Right);
		String shortPath = extendedPath.substr( start, slashPos );

		if( deepSearch )
		{
			if( shortPath.find( pattern ) != -1 )
			{
				if( !includeDefaults && extendedPath.find("_default") != -1 )
					continue;

				String listMember = String::ToString(fieldEntry->value);
				listMember.insert(start, " " );
				listMember.insert(start, String::ToString(fieldEntry->slotName) );

				mSearchResults.push_back( listMember );
			}
		}
		else
		{
			if( shortPath.compare( pattern ) == 0 )
			{
				if( !includeDefaults && extendedPath.find("_default") != -1 )
					continue;

				String listMember = String::ToString(fieldEntry->value);
				listMember.insert(start, " " );
				listMember.insert(start, String::ToString(fieldEntry->slotName) );

				mSearchResults.push_back( listMember );
			}
		}
	}

	return mSearchResults.size();
}
*/
const char* Settings::findFirstValue( const char* pattern, bool deepSearch, bool includeDefaults )
{
	mSearchResults.clear();

	SimFieldDictionary* fieldDictionary = getFieldDictionary();
	// Get the dynamic field count
	if ( !fieldDictionary )
		return "";

   for (SimFieldDictionaryIterator itr(fieldDictionary); *itr; ++itr)
	{
		// Fetch Field Entry.
      SimFieldDictionary::Entry* fieldEntry = *itr;
		
		// Compare strings, store proper results in vector
		String extendedPath = String::ToString(fieldEntry->slotName);
		String::SizeType start(0);
		String::SizeType slashPos = extendedPath.find('/', 0, String::Right);
		String shortPath = extendedPath.substr( start, slashPos );

		if( deepSearch )
		{
			if( shortPath.find( pattern ) != -1 )
			{
				if( !includeDefaults && extendedPath.find("_default") != -1 )
					continue;

				String listMember = String::ToString(fieldEntry->slotName);
				//listMember.insert(start, " " );
				//listMember.insert(start, String::ToString(fieldEntry->slotName) );

				mSearchResults.push_back( listMember );
			}
		}
		else
		{
			if( shortPath.compare( pattern ) == 0 )
			{
				if( !includeDefaults && extendedPath.find("_default") != -1 )
					continue;

				String listMember = String::ToString(fieldEntry->slotName);
				//listMember.insert(start, " " );
				//listMember.insert(start, String::ToString(fieldEntry->slotName) );

				mSearchResults.push_back( listMember );
			}
		}
	}
	
	if( mSearchResults.size() < 1 )
   {
		Con::errorf("findFirstValue() : Pattern not found");
      return "";
   }

	mSearchPos = 0;
	return mSearchResults[mSearchPos];
}

const char* Settings::findNextValue()
{
	if ( mSearchPos + 1 >= mSearchResults.size() )
      return "";
	mSearchPos++;
	return mSearchResults[mSearchPos];
}

// make sure to replace the strings
DefineConsoleMethod(Settings, findFirstValue, const char*, ( const char* pattern, bool deepSearch, bool includeDefaults ), ("", false, false), "settingObj.findFirstValue();")
{
   return object->findFirstValue( pattern, deepSearch, includeDefaults );
}

DefineConsoleMethod(Settings, findNextValue, const char*, (), , "settingObj.findNextValue();")
{
	return object->findNextValue();
}
/*
ConsoleMethod(Settings, buildSearchList, void, 2, 2, "settingObj.buildSearchList();")
{
	object->buildSearchList( "foobar" );
}
*/
void SettingSaveNode::addValue(const UTF8 *name, const UTF8 *value)
{
   String nameString(name);
   S32 groupCount = getGroupCount(nameString);
   SettingSaveNode *parentNode = this;

   // let's check to make sure all these groups exist already
   for(S32 i=0; i<groupCount; i++)
   {
      String groupName = getGroup(nameString, i);
	  if(!groupName.isEmpty())
	  {
		 bool found = false;
         // loop through all of our nodes to find if this one exists,
		 // if it does we want to use it
         for(S32 j=0; j<parentNode->mGroupNodes.size(); j++)
		 {
            SettingSaveNode *node = parentNode->mGroupNodes[j];

            if(!node->mIsGroup)
			   continue;

            if(node->mName.compare(groupName) == 0)
			{
			   parentNode = node;
			   found = true;
               break;
			}
		 }
         
		 // not found, so we create it
		 if(!found)
		 {   
			SettingSaveNode *node = new SettingSaveNode(groupName, true);
            parentNode->mGroupNodes.push_back(node);
		    parentNode = node;
		 }
	  }
   }

   // now we can properly set our actual value
   String settingNameString = getSettingName(name);
   String valueString(value);
   SettingSaveNode *node = new SettingSaveNode(settingNameString, valueString);
   parentNode->mSettingNodes.push_back(node);
}

S32 SettingSaveNode::getGroupCount(const String &name)
{
   String::SizeType pos = 0;
   S32 count = 0;

   // loop through and count our exiting groups
   while(pos != String::NPos)
   {
	  pos = name.find("/", pos + 1);
	  if(pos != String::NPos)
	     count++;
   }

   return count;
}

String SettingSaveNode::getGroup(const String &name, S32 num)
{
   String::SizeType pos = 0;
   String::SizeType lastPos = 0;
   S32 count = 0;

   while(pos != String::NPos)
   {
	  lastPos = pos;
	  pos = name.find("/", pos + 1);

      if(count == num)
	  {
		 String::SizeType startPos = lastPos;

         if(count > 0)
			startPos++;

		 if(pos == String::NPos)
		    return name.substr(startPos, name.length() - (startPos));
		 else
            return name.substr(startPos, pos - startPos);
	  }

	  count++;
   }   

   return String("");
}

String SettingSaveNode::getSettingName(const String &name)
{
   String::SizeType pos = name.find("/", 0, String::Right);

   if(pos == String::NPos)
	  return String(name);
   else
	  return name.substr(pos+1, name.length() - (pos+1));
}

void SettingSaveNode::clear()
{
   for( U32 i = 0, num = mGroupNodes.size(); i < num; ++ i )
      delete mGroupNodes[ i ];
   for( U32 i = 0, num = mSettingNodes.size(); i < num; ++ i )
      delete mSettingNodes[ i ];

   mGroupNodes.clear();
   mSettingNodes.clear();
}

void SettingSaveNode::buildDocument(SimXMLDocument *document, bool skipWrite)
{
   // let's build our XML doc
   if(mIsGroup && !skipWrite)
   {
      document->pushNewElement("Group");
      document->setAttribute("name", mName);
   }

   if(!mIsGroup && !skipWrite)
   {
	  document->pushNewElement("Setting");
	  document->setAttribute("name", mName);
      document->addText(mValue);
   } else
   {
	  for(S32 i=0; i<mSettingNodes.size(); i++)
	  {
         SettingSaveNode *node = mSettingNodes[i];
		 node->buildDocument(document);
	  }

      for(S32 i=0; i<mGroupNodes.size(); i++)
	  {
         SettingSaveNode *node = mGroupNodes[i];
		 node->buildDocument(document);
	  }
   }
   
   if(!skipWrite)
      document->popElement();
}

DefineConsoleMethod(Settings, setValue, void, (const char * settingName, const char * value), (""), "settingObj.setValue(settingName, value);")
{
   const char *fieldName = StringTable->insert( settingName );
   
   if( value != "")
      object->setValue( fieldName, value );
   else
      object->setValue( fieldName );
}

DefineConsoleMethod(Settings, setDefaultValue, void, (const char * settingName, const char * value), , "settingObj.setDefaultValue(settingName, value);")
{
   const char *fieldName = StringTable->insert( settingName );
   object->setDefaultValue( fieldName, value );
}

DefineConsoleMethod(Settings, value, const char*, (const char * settingName, const char * defaultValue), (""), "settingObj.value(settingName, defaultValue);")
{
   const char *fieldName = StringTable->insert( settingName );
   
   if(defaultValue != "")
      return object->value( fieldName, defaultValue );
   else if(settingName != "")
      return object->value( fieldName );

   return "";
}

DefineConsoleMethod(Settings, remove, void, (const char * settingName, bool includeDefaults), (false), "settingObj.remove(settingName, includeDefaults = false);")
{
   // there's a problem with some fields not being removed properly, but works if you run it twice,
   // a temporary solution for now is simply to call the remove twice

	object->remove( settingName, includeDefaults );
	object->remove( settingName, includeDefaults );
}

ConsoleMethod(Settings, write, bool, 2, 2, "%success = settingObj.write();")
{
   TORQUE_UNUSED(argc); TORQUE_UNUSED(argv);
   return object->write();
}

DefineConsoleMethod(Settings, read, bool, (), , "%success = settingObj.read();")
{
   return object->read();
}

DefineConsoleMethod(Settings, beginGroup, void, (const char * groupName, bool includeDefaults), (false), "settingObj.beginGroup(groupName, fromStart = false);")
{
	object->beginGroup( groupName, includeDefaults );
}

DefineConsoleMethod(Settings, endGroup, void, (), , "settingObj.endGroup();")
{
   object->endGroup();
}

DefineConsoleMethod(Settings, clearGroups, void, (), , "settingObj.clearGroups();")
{
   object->clearGroups();
}

DefineConsoleMethod(Settings, getCurrentGroups, const char*, (), , "settingObj.getCurrentGroups();")
{
   return object->getCurrentGroups();
}


















































//---------------DNTC AUTO-GENERATED---------------//
#include <vector>

#include <string>

#include "core/strings/stringFunctions.h"

//---------------DO NOT MODIFY CODE BELOW----------//

extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_beginGroup(char * x__object, char * x__groupName, bool includeDefaults)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* groupName = (const char*)x__groupName;

{
	object->beginGroup( groupName, includeDefaults );
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_clearGroups(char * x__object)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
{
   object->clearGroups();
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_endGroup(char * x__object)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
{
   object->endGroup();
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_findFirstValue(char * x__object, char * x__pattern, bool deepSearch, bool includeDefaults,  char* retval)
{
dSprintf(retval,16384,"");
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* pattern = (const char*)x__pattern;

const char* wle_returnObject;
{
   {wle_returnObject =object->findFirstValue( pattern, deepSearch, includeDefaults );
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_findNextValue(char * x__object,  char* retval)
{
dSprintf(retval,16384,"");
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* wle_returnObject;
{
	{wle_returnObject =object->findNextValue();
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_getCurrentGroups(char * x__object,  char* retval)
{
dSprintf(retval,16384,"");
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* wle_returnObject;
{
   {wle_returnObject =object->getCurrentGroups();
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
}
}
extern "C" __declspec(dllexport) S32  __cdecl wle_fn_Settings_read(char * x__object)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return 0;
bool wle_returnObject;
{
   {wle_returnObject =object->read();
return (S32)(wle_returnObject);}
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_remove(char * x__object, char * x__settingName, bool includeDefaults)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* settingName = (const char*)x__settingName;

{
      
	object->remove( settingName, includeDefaults );
	object->remove( settingName, includeDefaults );
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_setDefaultValue(char * x__object, char * x__settingName, char * x__value)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* settingName = (const char*)x__settingName;
const char* value = (const char*)x__value;
{
   const char *fieldName = StringTable->insert( settingName );
   object->setDefaultValue( fieldName, value );
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_setValue(char * x__object, char * x__settingName, char * x__value)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* settingName = (const char*)x__settingName;
const char* value = (const char*)x__value;
{
   const char *fieldName = StringTable->insert( settingName );
   
   if( value != "")
      object->setValue( fieldName, value );
   else
      object->setValue( fieldName );
}
}
extern "C" __declspec(dllexport) void  __cdecl wle_fn_Settings_value(char * x__object, char * x__settingName, char * x__defaultValue,  char* retval)
{
dSprintf(retval,16384,"");
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return;
const char* settingName = (const char*)x__settingName;
const char* defaultValue = (const char*)x__defaultValue;
const char* wle_returnObject;
{
   const char *fieldName = StringTable->insert( settingName );
   
   if(defaultValue != "")
      {wle_returnObject =object->value( fieldName, defaultValue );
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
   else if(settingName != "")
      {wle_returnObject =object->value( fieldName );
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
   {wle_returnObject ="";
if (!wle_returnObject) 
return;
dSprintf(retval,16384,"%s",wle_returnObject);
return;
}
}
}
extern "C" __declspec(dllexport) S32  __cdecl wle_fnSettings_write(char * x__object)
{
Settings* object; Sim::findObject(x__object, object ); 
if (!object)
	 return 0;
bool wle_returnObject;
{
S32 argc = 2;
{
    
   {wle_returnObject =object->write();
return (S32)(wle_returnObject);}
}
}
}
//---------------END DNTC AUTO-GENERATED-----------//

