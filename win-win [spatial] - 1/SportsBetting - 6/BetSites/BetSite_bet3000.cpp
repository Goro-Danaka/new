#include "..\stdafx.h"
#include "BetSite_bet3000.h"
#include "..\global.h"

CBetSite_bet3000::CBetSite_bet3000()
{
	m_nBookMaker = SITE_bet3000;
}

CBetSite_bet3000::~CBetSite_bet3000()
{
}

int CBetSite_bet3000::MainProc()
{
	SendRequestEx();
	AnalyzeResText();
	return 0;
}

void CBetSite_bet3000::SendRequestEx()
{
}

void CBetSite_bet3000::AnalyzeResText()
{
}
