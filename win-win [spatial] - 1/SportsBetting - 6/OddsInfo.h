#pragma once
#include <string>
using namespace std;

enum ODDS_INFO
{
	OI_MAIN = 0,
	OI_HANDICAP = 1,
	OI_GOAL_OU,
	OI_GOAL_OE,
	OI_TEAM1_GOAL_OU,
	OI_TEAM2_GOAL_OU,
};

class COddsInfo
{
public:
	COddsInfo();
	~COddsInfo();

	int m_nOddsInfoKind;

	float m_hVal1;
	float m_oVal1;//ht, over, odd
	float m_oVal2;//at, under, even
	float fDraw;

	UINT64 m_dwOddID1;//home
	UINT64 m_dwOddID2;//away
	UINT64 m_dwOddID3;//draw

	BOOL operator==(COddsInfo& OddsInfo);
	BOOL operator!=(COddsInfo& OddsInfo);
};

