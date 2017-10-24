#pragma once
#include "BetSite.h"

class CBetSite_marathonbet : public CBetSite
{
public:
	CBetSite_marathonbet();
	~CBetSite_marathonbet();

public:
	virtual int MainProc();
	virtual BOOL SendRequestEx(int nReqKind);
	void AnalyzeResData();
	void AnalyzeResLogIn();
	void AnalyzeResLogOut();
	void AnalyzeResBalance();
};

