#ifndef _GUIAUTOCOMPLETECTRL_H_
#define _GUIAUTOCOMPLETECTRL_H_

#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif
#ifndef _GUITEXTLISTCTRL_H_
#include "gui/controls/guiTextListCtrl.h"
#endif
#include "gui/controls/guiTextEditCtrl.h"
#ifndef _GUIBUTTONCTRL_H_
#include "gui/buttons/guiButtonCtrl.h"
#endif
#ifndef _GUIBACKGROUNDCTRL_H_
#include "gui/controls/guiBackgroundCtrl.h"
#endif
#ifndef _GUISCROLLCTRL_H_
#include "gui/containers/guiScrollCtrl.h"
#endif
class GuiAutoCompleteCtrl;
class PopupTextListCtrl;

class PopUpBackgroundCtrl : public GuiControl
{
protected:
   GuiAutoCompleteCtrl *mPopUpCtrl;
   PopupTextListCtrl *mTextList; 
public:
   PopUpBackgroundCtrl(GuiAutoCompleteCtrl *ctrl, PopupTextListCtrl* textList);
   void onMouseDown(const GuiEvent &event);
};

class PopupTextListCtrl : public GuiTextListCtrl
{
   private:
   typedef GuiTextListCtrl Parent;

protected:
   GuiAutoCompleteCtrl *mPopUpCtrl;
   bool mLastPositionInside;

public:
   PopupTextListCtrl(); // for inheritance
   PopupTextListCtrl(GuiAutoCompleteCtrl *ctrl);

   // GuiArrayCtrl overload:
   void onCellSelected(Point2I cell);

   // GuiControl overloads:
   void onMouseMove(const GuiEvent &event);
   bool onKeyDown(const GuiEvent &event);
   void onMouseUp(const GuiEvent &event);
   void onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver);
};

class GuiAutoCompleteCtrl : public GuiTextEditCtrl
{
   typedef GuiTextEditCtrl Parent;

public:
   struct Entry
   {
      char buf[256];
      S32 id;
      U16 ascii;
      U16 scheme;
      bool usesColorBox;	//  Added
      ColorI colorbox;		//  Added
   };

   struct Scheme
   {
      U32      id;
      ColorI   fontColor;
      ColorI   fontColorHL;
      ColorI   fontColorSEL;
   };

   bool mBackgroundCancel; //  Added

protected:
   PopupTextListCtrl *mTl;
   GuiScrollCtrl *mSc;
   PopUpBackgroundCtrl *mBackground;
   Vector<Entry> mEntries;
   Vector<Entry> mFilteredEntries;
   Vector<Scheme> mSchemes;
   S32 mSelIndex;
   S32 mMaxPopupHeight;
   F32 mIncValue;
   F32 mScrollCount;
   S32 mLastYvalue;
   GuiEvent mEventSave;
   S32 mRevNum;
   bool mInAction;
   bool mReplaceText;
   //bool mMouseOver; // Copyright (C) 2013 WinterLeaf Entertainment LLC.
   bool mRenderScrollInNA; //  Added
   bool mReverseTextList;	//  Added - Should we reverse the text list if we display up?
   StringTableEntry mBitmapName; //  Added
   Point2I mBitmapBounds; //  Added
   GFXTexHandle mTextureNormal; //  Added
   GFXTexHandle mTextureDepressed; //  Added
	S32 mIdMax;

   virtual void addChildren();
   virtual void repositionPopup();

public:
	GuiAutoCompleteCtrl(void);
   ~GuiAutoCompleteCtrl();   
   GuiScrollCtrl::Region mScrollDir;
   bool onWake(); //  Added
   bool onAdd();
   void onSleep();
   void setBitmap(const char *name); //  Added
   void sort();
   void sortID(); //  Added
   void addEntry(const char *buf, S32 id = -1, U32 scheme = 0);
   void addScheme(U32 id, ColorI fontColor, ColorI fontColorHL, ColorI fontColorSEL);
   //void onRender(Point2I offset, const RectI &updateRect);
   void rebuildResultEntries();
   void onAction();
   virtual void closePopUp();
   void clear();
	void clearEntry( S32 entry ); //  Added
   void onMouseDown(const GuiEvent &event);
   //void onMouseUp(const GuiEvent &event);
   void onMouseEnter(const GuiEvent &event); //  Added
   void onMouseLeave(const GuiEvent &event); //  Added
   void setupAutoScroll(const GuiEvent &event);
   void autoScroll();
   bool onKeyDown(const GuiEvent &event);
   void reverseTextList();
   bool getFontColor(ColorI &fontColor, S32 id, bool selected, bool mouseOver);
   bool getColoredBox(ColorI &boxColor, S32 id); //  Added
   bool setEntryText( S32 id, const char* buf );

   S32 getSelected();
   void setSelected(S32 id, bool bNotifyScript = true);
   void setFirstSelected(bool bNotifyScript = true); //  Added
   void setNoneSelected();	//  Added
   const char *getScriptValue();
   const char *getTextById(S32 id);
   S32 findText( const char* text );
   S32 getNumEntries()   { return( mEntries.size() ); }
   void replaceText(S32);

   DECLARE_CONOBJECT( GuiAutoCompleteCtrl );
   DECLARE_CATEGORY( "Gui Lists" );
   DECLARE_DESCRIPTION( "A control that allows to select a value from a drop-down list." );
   
   static void initPersistFields(void);

};

#endif //_GUI_POPUPMENU_CTRL_H
