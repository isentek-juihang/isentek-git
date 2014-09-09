#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "ISenI2cMaster.h"

CISenI2cMaster* __stdcall CreateIicObj();
void __stdcall ReleaseIicObj(CISenI2cMaster* pIic);