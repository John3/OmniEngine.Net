//---------------------------------------------------------------------------------------------------------------
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
// 
// The use of the WinterLeaf Entertainment LLC Advanced UI System
// is governed by this license agreement (“Agreement”).
// 
// R E S T R I C T I O N S
// 
// (a) Licensee may not: (i) create any derivative works, including but not
// limited to translations, localizations or technology add-ons of the Advanced UI System;
// (ii) reverse engineer , or otherwise attempt to derive the algorithms
// for Advanced UI System (iii) redistribute, encumber , sell, rent, lease, sublicense, or otherwise
// transfer rights to Advanced UI System; or (iv) remove or alter any trademark, logo, copyright
// or other proprietary notices, legends, symbols or labels in Advanced UI System; or (iiv) use
// the Software to develop or distribute any software that competes with the Software
// without WinterLeaf Entertainment’s prior written consent; or (iiiv) use the Software for
// any illegal purpose.
// (b) Licensee may not distribute the Advanced UI System in any manner.
// 
// 
// Please visit http://www.winterleafentertainment.com for more information about the project and latest updates.
// 
// Last updated: 09/16/2013
//---------------------------------------------------------------------------------------------------------------

function WindowSettings::loadDialog( %this, %ctrl )
{
   %this.ctrl = %ctrl;
   %this.checkAllowedDialogs();
   Canvas.pushDialog(WindowSettings);
  
  if (%this.ctrl.alpha)
  {
      %this.oldAlphaValue = %ctrl.getAlphaValue();
      AlphaLevelSlider.value = %this.oldAlphaValue;
      AlphaLevelText.text = %this.oldAlphaValue;
  }
  if( %this.ctrl.mouseOverAlpha)
  {
      %this.oldMouseOverAlphaValue = %ctrl.getMouseOverAlphaValue();
      MouseOverAlphaLevelSlider.value = %this.oldMouseOverAlphaValue;
      MouseOverAlphaLevelText.text = %this.oldMouseOverAlphaValue;
  }
  if(%this.ctrl.alphaFade)
  {
      %this.oldAlphaFadeTime = %ctrl.getAlphaFadeTime();
      AlphaFadeTimeText.text = %this.oldAlphaFadeTime;
  }
   
}

function WindowSettings::setAlphaValue( %this, %value )
{
   %this.ctrl.setAlphaValue( %value );
}

function WindowSettings::resetValue( %this )
{
   if(%this.ctrl.alpha)
      %this.ctrl.setAlphaValue(%this.oldAlphaValue);
   if(%this.ctrl.mouseOverAlpha)
      %this.ctrl.setMouseOverAlphaValue(%this.oldMouseOverAlphaValue);
   if(%this.ctrl.alphaFade)   
      %this.ctrl.setAlphaFadeTime(%this.oldAlphaFadeTime);
}

function WindowSettings::setMouseOverAlphaValue( %this, %value )
{
   %this.ctrl.setMouseOverAlphaValue( %value );
   
}

function WindowSettings::setAlphaFadeTime ( %this, %value )
{
   %this.ctrl.setAlphaFadeTime( %value );
}

function WindowSettings::checkAllowedDialogs( %this)
{
   if(!%this.ctrl.alpha)
      AlphaLevelControl.visible = 0;
   else
      AlphaLevelControl.visible = 1;
   if(!%this.ctrl.mouseOverAlpha)
      MouseOverAlphaControl.visible = 0;
   else
      MouseOverAlphaControl.visible = 1;
   if(!%this.ctrl.alphaFade)
      AlphaFadeControl.visible = 0;
   else
      AlphaFadeControl.visible = 1;
}
