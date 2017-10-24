#pragma once
#include "global.h"

class CMainInfo
{
public:
	int m_nGameCategory;
	char* m_szLeague;
	char* m_szHTName;
	char* m_szATName;
	char* m_szhtName;
	char* m_szatName;
	SCORE m_HTScore;
	SCORE m_ATScore;
	char* m_szStage;
	int m_nTime;
	BOOL m_bMaster;

	CMainInfo();
	CMainInfo(CMainInfo& info);
	CMainInfo(int nGameCategory, char* szHTName, char* szATName, SCORE& HTScore, SCORE& ATScore, char* szStage, int nTime, char* szLeague = "");
	CMainInfo& operator=(CMainInfo& info);
	~CMainInfo();
	void HAChange();
};
