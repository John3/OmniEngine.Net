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

#include "platform/platform.h"
#include "gui/game/guiProgressCtrl.h"

#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/gfxDrawUtil.h"
#include "console/engineAPI.h"

IMPLEMENT_CONOBJECT(GuiProgressCtrl);

ConsoleDocClass( GuiProgressCtrl,
   "@brief GUI Control which displays a horizontal bar which increases as the progress value of 0.0 - 1.0 increases.\n\n"

   "@tsexample\n"
   "     new GuiProgressCtrl(JS_statusBar)\n"
   "	 {\n"
   "		    //Properties not specific to this control have been omitted from this example.\n"
   "     };\n\n"
   "// Define the value to set the progress bar"
   "%value = \"0.5f\"\n\n"
   "// Set the value of the progress bar, from 0.0 - 1.0\n"
   "%thisGuiProgressCtrl.setValue(%value);\n"
   "// Get the value of the progress bar.\n"
   "%progress = %thisGuiProgressCtrl.getValue();\n"
   "@endtsexample\n\n"

   "@see GuiTextCtrl\n"
   "@see GuiControl\n\n"

   "@ingroup GuiValues\n"
);

GuiProgressCtrl::GuiProgressCtrl()
{
   mProgress = 0.0f;
   mHorizontalFill = true;
   mInvertFill = true;
}

const char* GuiProgressCtrl::getScriptValue()
{
   static const U32 bufSize = 64;
   char * ret = Con::getReturnBuffer(bufSize);
   dSprintf(ret, bufSize, "%g", mProgress);
   return ret;
}

void GuiProgressCtrl::setScriptValue(const char *value)
{
   //set the value
   if (! value)
      mProgress = 0.0f;
   else
      mProgress = dAtof(value);

   //validate the value
   mProgress = mClampF(mProgress, 0.f, 1.f);
   setUpdate();
}

void GuiProgressCtrl::onPreRender()
{
   const char * var = getVariable();
   if(var)
   {
      F32 value = mClampF(dAtof(var), 0.f, 1.f);
      if(value != mProgress)
      {
         mProgress = value;
         setUpdate();
      }
   }
}

// updated OnRender function
void GuiProgressCtrl::onRender(Point2I offset, const RectI &updateRect)    
{
   RectI ctrlRect(offset, getExtent());
   
   //draw the progress
   // check the boolean if we want it horizontal or not (set by the user, defaults to true)
   if (mHorizontalFill)
   {
      // the default horizontal code
      // horizontal
      S32 width = (S32)((F32)(getWidth()) * mProgress);
      if (width > 0)
      {
         if (mInvertFill) // default left to right if true
         {
            RectI progressRect = ctrlRect;
            progressRect.extent.x = width;
            GFX->getDrawUtil()->drawRectFill(progressRect, mProfile->mFillColor);
         }
         else // to fill right to left
         {
            RectI progressRect( ( ctrlRect.point.x + ctrlRect.extent.x ), ctrlRect.point.y, -ctrlRect.extent.x, ctrlRect.extent.y);
            progressRect.extent.x = -width;
            GFX->getDrawUtil()->drawRectFill(progressRect, mProfile->mFillColor);
         }
      }
   }
   else
   {
      // new code to check if we want it vertical
      // vertical
      S32 height = (S32)((F32)getHeight() * mProgress);
      
      if (height > 0)
      {
         if (mInvertFill) // default down to up if true
         {
            RectI progressRect(ctrlRect.point.x, ( ctrlRect.point.y + ctrlRect.extent.y ), ctrlRect.extent.x, -ctrlRect.extent.y);
            progressRect.extent.y = -height;
            GFX->getDrawUtil()->drawRectFill(progressRect, mProfile->mFillColor);
         }
         else // to fill down to up
         {
            RectI progressRect = ctrlRect;
            progressRect.extent.y = height;
            GFX->getDrawUtil()->drawRectFill(progressRect, mProfile->mFillColor);
         }
      }
   }
   
   //now draw the border
   if (mProfile->mBorder)
      GFX->getDrawUtil()->drawRect(ctrlRect, mProfile->mBorderColor);

   Parent::onRender( offset, updateRect );

   //render the children
   renderChildControls(offset, updateRect);
}

// add the fields to GuiProgressCtrl  
void GuiProgressCtrl::initPersistFields()    
{    
   // add the horizontal field  
   addField("horizontalFill", TypeBool, Offset(mHorizontalFill, GuiProgressCtrl),
         "True if you want the GuiProgressCtrl to be horizontal. Defaults to true. " );
   
   addField("invertFill", TypeBool, Offset(mInvertFill, GuiProgressCtrl),
         "True if you want the GuiProgressCtrl to fill Left to Right (when horizontalFill = true)"
         "or Down to Up (when horizontalFill = false). Defaults to true. " );
   
   Parent::initPersistFields();
}
