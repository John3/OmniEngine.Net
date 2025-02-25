//-----------------------------------------------------------------------------
// Copyright (c) 2014 R.G.S. - Richards Game Studio, the Netherlands
//					  http://www.richardsgamestudio.com/
//
// If you find this code useful or you are feeling particularly generous I
// would ask that you please go to http://www.richardsgamestudio.com/ then
// choose Donations from the menu on the left side and make a donation to
// Richards Game Studio. It will be highly appreciated.
//
// The MIT License:
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

//-----------------------------------------------------------------------------
// Volumetric Fog Rendertarget Manager
//
// Creates and maintains one set of rendertargets to be used by every 
// VolumetricFog object in the scene.
//
// Will be loaded at startup end removed when ending game.
//
//-----------------------------------------------------------------------------

#include "VolumetricFogRTManager.h"
#include "core/module.h"
#include "scene/sceneManager.h"
#include "windowManager/platformWindowMgr.h"
#include "console/engineAPI.h"

MODULE_BEGIN(VolumetricFogRTManager)

MODULE_INIT_AFTER(Scene)
MODULE_SHUTDOWN_BEFORE(Scene)

MODULE_INIT
{
	gVolumetricFogRTManager = new VolumetricFogRTManager;
	gClientSceneGraph->addObjectToScene(gVolumetricFogRTManager);
}

MODULE_SHUTDOWN
{
	gClientSceneGraph->removeObjectFromScene(gVolumetricFogRTManager);
	SAFE_DELETE(gVolumetricFogRTManager);
}

MODULE_END;

ConsoleDocClass( VolumetricFogRTManager,
	"@brief Creates and maintains one set of rendertargets to be used by every\n" 
	 "VolumetricFog object in the scene.\n\n"
	"Will be loaded at startup end removed when ending game.\n\n"
	"Methods:\n"
	"	get()	returns the currently loaded VolumetricFogRTManager, also accessible\n"
	"			through VFRTM define.\n"	
	"	Init()	Initializes the rendertargets, called when a VolumetricFog object is\n"
	"			added to the scene.\n"
	"	isInitialed()	returns true if Rendertargets are present, false if not, then\n"
	"					Init() should be called to create the rendertargets.\n"
	"	setQuality(U32 Quality)	Normally a rendertarget has the same size as the view,\n"
	"							with this method you can scale down the size of it.\n"
	"							Be aware that scaling down will introduce renderartefacts.\n"
	"@ingroup Atmosphere"
	);

VolumetricFogRTManager *gVolumetricFogRTManager = NULL;
S32 VolumetricFogRTManager::mTargetScale = 1;

IMPLEMENT_CONOBJECT(VolumetricFogRTManager);

VolumetricFogRTManager::VolumetricFogRTManager()
{
	setGlobalBounds();

	mTypeMask |= EnvironmentObjectType;
	
	mIsInitialized = false;
	mNumFogObjects = 0;
	mScheduled = false;
}

VolumetricFogRTManager::~VolumetricFogRTManager()
{
	if (isClientObject())
		return;

	if (mFrontTarget.isRegistered())
		mFrontTarget.unregister();

	if (mDepthTarget.isRegistered())
		mDepthTarget.unregister();
	
	if (mDepthBuffer.isValid())
		mDepthBuffer->kill();

	if (mFrontBuffer.isValid())
		mFrontBuffer->kill();
}

void VolumetricFogRTManager::onRemove()
{
	removeFromScene();

	Parent::onRemove();
}

void VolumetricFogRTManager::consoleInit()
{
	Con::addVariable("$pref::VolumetricFog::Quality", TypeS32, &mTargetScale,
		"Controls whether decals are rendered.\n"
		"@ingroup Decals");
}

bool VolumetricFogRTManager::Init()
{
	if (mIsInitialized)
	{
		Con::warnf("VolumetricFogRTManager allready initialized!!");
		return true;
	}

	PlatformWindowManager *WinMan = PlatformWindowManager::get();
	PlatformWindow *Win = WinMan->getFirstWindow();

	U32 width = mFloor(Win->getClientExtent().x / mTargetScale);
	U32 height = Win->getClientExtent().y;

	if (!Win->isFullscreen())
		height -= 20;//subtract caption bar from rendertarget size.
	height = mFloor(height / mTargetScale);

	mDepthBuffer = GFXTexHandle(width, height, GFXFormatR32F,
		&GFXDefaultRenderTargetProfile, avar("%s() - mDepthBuffer (line %d)", __FUNCTION__, __LINE__));
	if (!mDepthBuffer.isValid())
	{
		Con::errorf("VolumetricFogRTManager Fatal Error: Unable to create Depthbuffer");
		return false;
	}
	if (!mDepthTarget.registerWithName("volfogdepth"))
	{
		Con::errorf("VolumetricFogRTManager Fatal Error : Unable to register Depthbuffer");
			return false;
	}
	mDepthTarget.setTexture(mDepthBuffer);

	mFrontBuffer = GFXTexHandle(width, height, GFXFormatR32F,
		&GFXDefaultRenderTargetProfile, avar("%s() - mFrontBuffer (line %d)", __FUNCTION__, __LINE__));
	if (!mFrontBuffer.isValid())
	{
		Con::errorf("VolumetricFogRTManager Fatal Error: Unable to create front buffer");
		return false;
	}
	if (!mFrontTarget.registerWithName("volfogfront"))
	{
		Con::errorf("VolumetricFogRTManager Fatal Error : Unable to register Frontbuffer");
		return false;
	}

	mFrontTarget.setTexture(mFrontBuffer);

	mIsInitialized = true;

	return true;
}

U32 VolumetricFogRTManager::IncFogObjects()
{
	mNumFogObjects++;
	return mNumFogObjects;
}

U32 VolumetricFogRTManager::DecFogObjects()
{
	if (mNumFogObjects > 0)
		mNumFogObjects--;
	if (mNumFogObjects == 0 && mScheduled != 0)
	{
		setQuality(mScheduled);
		mScheduled = 0;
	}
	return mNumFogObjects;
}

S32 VolumetricFogRTManager::setQuality(U32 Quality)
{
	S32 oldScale = mTargetScale;

	if (!mIsInitialized)
		return (mTargetScale = Quality);

	if (Quality < 1)
		Quality = 1;

	if (Quality == mTargetScale)
		return mTargetScale;

	if (mNumFogObjects > 0)
	{
		mScheduled = Quality;
		Con::warnf("VolumetricFogRTManager::setQuality error: Number of fog objects in scene > 0");
		return 0;
	}

	mTargetScale = Quality;

	PlatformWindowManager *WinMan = PlatformWindowManager::get();
	PlatformWindow *Win = WinMan->getFirstWindow();

	U32 width = mFloor(Win->getClientExtent().x / mTargetScale);
	U32 height = Win->getClientExtent().y;

	if (!Win->isFullscreen())
		height -= 20;//subtract caption bar from rendertarget size.
	height = mFloor(height / mTargetScale);

	if (width < 32 || height < 32)
	{
		mTargetScale = oldScale;
		return mTargetScale;
	}

	if (mFrontBuffer.isValid())
	{
		mFrontTarget.setTexture(NULL);
		mFrontBuffer->kill();
		mFrontBuffer.set(width, height, GFXFormatR32F,
			&GFXDefaultRenderTargetProfile, avar("%s() - mFrontBuffer (line %d)", __FUNCTION__, __LINE__));
		mFrontTarget.setTexture(mFrontBuffer);
	}

	if (mDepthBuffer.isValid())
	{
		mDepthTarget.setTexture(NULL);
		mDepthBuffer->kill();
		mDepthBuffer.set(width, height, GFXFormatR32F,
			&GFXDefaultRenderTargetProfile, avar("%s() - mDepthBuffer (line %d)", __FUNCTION__, __LINE__));
		mDepthTarget.setTexture(mDepthBuffer);
	}
	return mTargetScale;
}

VolumetricFogRTManager* VolumetricFogRTManager::get()
{
	return gVolumetricFogRTManager;
}

DefineConsoleFunction(SetFogVolumeQuality, S32, (U32 new_quality), ,
	"@brief Resizes the rendertargets of the Volumetric Fog object.\n"
	"If there are fogobjects in the scene then the function is postponed until\n"
	"all fogobjects are deleted.\n\n"
	"@params new_quality new quality for the rendertargets 1 = full size, 2 = halfsize, 3 = 1/3, 4 = 1/4 ...")
{
	if (VFRTM == NULL)
		return -1;
	return VFRTM->setQuality(new_quality);
}


















































//---------------DNTC AUTO-GENERATED---------------//
#include <vector>

#include <string>

#include "core/strings/stringFunctions.h"

//---------------DO NOT MODIFY CODE BELOW----------//

extern "C" __declspec(dllexport) S32  __cdecl wle_fn_SetFogVolumeQuality(U32 new_quality)
{
{
	if (VFRTM == NULL)
		return -1;
	return VFRTM->setQuality(new_quality);
};
}
//---------------END DNTC AUTO-GENERATED-----------//

