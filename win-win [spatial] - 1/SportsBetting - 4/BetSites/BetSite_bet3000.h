#pragma once
#include "BetSite.h"
class CBetSite_bet3000 :
	public CBetSite
{
public:
	CBetSite_bet3000();
	~CBetSite_bet3000();

public:
	virtual int MainProc();
	void SendRequestEx();
	void AnalyzeResText();
};

