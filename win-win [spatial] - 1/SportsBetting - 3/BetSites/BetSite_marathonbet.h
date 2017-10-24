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
	virtual BOOL AnalyzeResData();
	virtual BOOL AnalyzeResBalance();
	void AnalyzeResLogIn();
	void AnalyzeResLogOut();
};

