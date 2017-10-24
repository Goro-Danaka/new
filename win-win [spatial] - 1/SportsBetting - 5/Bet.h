#pragma once

#include "stdafx.h"
#include "..\SportsBetting\global.h"

__declspec(dllexport) BOOL SetGameInfoArray(CArrayGameInfo& aryGameInfo, BetInfo* pBetInfo, int nBetStrategy, float* fBalance, BOOL bExitBetThreadFlg, BOOL& bBettingCompleted);
