// BetEngine.h : main header file for the BetEngine DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CBetEngineApp
// See BetEngine.cpp for the implementation of this class
//

class CBetEngineApp : public CWinApp
{
public:
	CBetEngineApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

