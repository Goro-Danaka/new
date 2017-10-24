#pragma once
#include "BetSite.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>

typedef CArray<string, string&> CStrArray;

class CBetSite_1bet :
	public CBetSite
{
public:
	CBetSite_1bet();
	~CBetSite_1bet();

public:
	virtual int MainProc();
	void SendRequestEx();
	void AnalyzeResText();

	void Proc(char* s);
	void MakeArray(char* s, CStrArray& arr);
	void Parse(CStrArray& arrVal);
};

