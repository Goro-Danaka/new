#include "..\stdafx.h"
#include "BetSite_betballer.h"
#include "..\global.h"

CBetSite_betballer::CBetSite_betballer()
{
	m_nBookMaker = SITE_betballer;
}

CBetSite_betballer::~CBetSite_betballer()
{
}

int CBetSite_betballer::MainProc()
{
	SendRequestEx();
	AnalyzeResText();
	return 0;
}

void CBetSite_betballer::SendRequestEx()
{
}

void CBetSite_betballer::AnalyzeResText()
{
}
