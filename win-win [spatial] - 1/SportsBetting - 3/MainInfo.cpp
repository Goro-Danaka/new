#include "stdafx.h"
#include "MainInfo.h"

CMainInfo::CMainInfo()
{
	m_nGameCategory = GC_FOOTBALL;
	m_szLeague = new char[MAX_LEAGUE_NAME];
	m_szHTName = new char[MAX_TEAM_NAME];
	m_szATName = new char[MAX_TEAM_NAME];
	m_szhtName = new char[MAX_TEAM_NAME];
	m_szatName = new char[MAX_TEAM_NAME];
	m_szStage = new char[MAX_STAGE_NAME];
	m_bMaster = FALSE;
	m_nTime = 0;
};

CMainInfo::CMainInfo(CMainInfo& info)
{
	m_nGameCategory = info.m_nGameCategory;
	m_szLeague = new char[MAX_LEAGUE_NAME];
	m_szHTName = new char[MAX_TEAM_NAME];
	m_szATName = new char[MAX_TEAM_NAME];
	m_szhtName = new char[MAX_TEAM_NAME];
	m_szatName = new char[MAX_TEAM_NAME];
	strcpy_s(m_szLeague, MAX_LEAGUE_NAME, info.m_szLeague);
	strcpy_s(m_szHTName, MAX_TEAM_NAME, info.m_szHTName);
	strcpy_s(m_szATName, MAX_TEAM_NAME, info.m_szATName);
	strcpy_s(m_szhtName, MAX_TEAM_NAME, info.m_szhtName);
	strcpy_s(m_szatName, MAX_TEAM_NAME, info.m_szatName);
	m_HTScore = info.m_HTScore;
	m_ATScore = info.m_ATScore;

	m_szStage = new char[MAX_STAGE_NAME];
	strcpy_s(m_szStage, MAX_STAGE_NAME, info.m_szStage);
	m_bMaster = info.m_bMaster;
	m_nTime = info.m_nTime;
};

CMainInfo::CMainInfo(int nGameCategory, char* szHTName, char* szATName, SCORE& HTScore, SCORE& ATScore, char* szStage, int nTime, char* szLeague)
{
	m_nGameCategory = nGameCategory;
	m_szLeague = new char[MAX_LEAGUE_NAME];
	m_szHTName = new char[MAX_TEAM_NAME];
	m_szATName = new char[MAX_TEAM_NAME];
	strcpy_s(m_szLeague, MAX_LEAGUE_NAME, szLeague);
	strcpy_s(m_szHTName, MAX_TEAM_NAME, szHTName);
	strcpy_s(m_szATName, MAX_TEAM_NAME, szATName);
	char* h1 = strstr(m_szHTName, "(Women)");
	char* a1 = strstr(m_szATName, "(Women)");
	if (h1 != 0)
	{
		h1[2] = ')';
		h1[3] = 0;
	}
	if (a1 != 0)
	{
		a1[2] = ')';
		a1[3] = 0;
	}
	//if (strstr(m_szHTName, "PTT"))
	//	_asm int 3
	m_szhtName = new char[MAX_TEAM_NAME];
	m_szatName = new char[MAX_TEAM_NAME];
	strproc1(m_szHTName, m_szhtName);
	strproc1(m_szATName, m_szatName);
	m_HTScore = HTScore;
	m_ATScore = ATScore;

	m_szStage = new char[MAX_STAGE_NAME];
	strcpy_s(m_szStage, MAX_STAGE_NAME, szStage);
	m_nTime = nTime;

	if (strcmp(m_szStage, "HT") == 0 && m_nTime == 0)
		m_nTime = 45 * 60;
	if (m_nGameCategory == GC_FOOTBALL && strcmp(m_szStage, "2H") == 0 && m_nTime < 45 * 60)
		m_nTime += 45 * 60;

};

CMainInfo& CMainInfo::operator=(CMainInfo& info)
{
	m_nGameCategory = info.m_nGameCategory;
	if (!m_szLeague)
		m_szLeague = new char[MAX_LEAGUE_NAME];
	if (!m_szHTName)
		m_szHTName = new char[MAX_TEAM_NAME];
	if (!m_szATName)
		m_szATName = new char[MAX_TEAM_NAME];
	if (!m_szhtName)
		m_szhtName = new char[MAX_TEAM_NAME];
	if (!m_szatName)
		m_szatName = new char[MAX_TEAM_NAME];
	strcpy_s(m_szLeague, MAX_LEAGUE_NAME, info.m_szLeague);
	strcpy_s(m_szHTName, MAX_TEAM_NAME, info.m_szHTName);
	strcpy_s(m_szATName, MAX_TEAM_NAME, info.m_szATName);
	strcpy_s(m_szhtName, MAX_TEAM_NAME, info.m_szhtName);
	strcpy_s(m_szatName, MAX_TEAM_NAME, info.m_szatName);
	m_HTScore = info.m_HTScore;
	m_ATScore = info.m_ATScore;

	if (!m_szStage)
		m_szStage = new char[MAX_STAGE_NAME];
	strcpy_s(m_szStage, MAX_STAGE_NAME, info.m_szStage);
	m_bMaster = info.m_bMaster;
	m_nTime = info.m_nTime;
	return *this;
};

CMainInfo::~CMainInfo()
{
	delete m_szLeague;
	delete m_szHTName;
	delete m_szATName;
	delete m_szhtName;
	delete m_szatName;
	delete m_szStage;
};

void CMainInfo::HAChange()
{
	char szTemp[MAX_TEAM_NAME];
	strcpy_s(szTemp, m_szHTName);
	strcpy_s(m_szHTName, MAX_TEAM_NAME, m_szATName);
	strcpy_s(m_szATName, MAX_TEAM_NAME, szTemp);

	strcpy_s(szTemp, m_szhtName);
	strcpy_s(m_szhtName, MAX_TEAM_NAME, m_szatName);
	strcpy_s(m_szatName, MAX_TEAM_NAME, szTemp);

	SCORE nTemp = m_HTScore;
	m_HTScore = m_ATScore;
	m_ATScore = nTemp;
};