#pragma once
#include "BetSite.h"
class CBetSite_betballer :
	public CBetSite
{
public:
	CBetSite_betballer();
	~CBetSite_betballer();

public:
	virtual int MainProc();
	void SendRequestEx();
	void AnalyzeResText();
};

