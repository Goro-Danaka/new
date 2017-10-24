#include "stdafx.h"
#include "OddsInfo.h"

COddsInfo::COddsInfo()
{
	m_nOddsInfoKind = 0;

	m_hVal1 = 0;
	m_oVal1 = 0;
	m_oVal2 = 0;
}

COddsInfo::~COddsInfo()
{
}

BOOL COddsInfo::operator==(COddsInfo& OddsInfo)
{
	if (m_nOddsInfoKind != OddsInfo.m_nOddsInfoKind)
		return FALSE;
	if (m_nOddsInfoKind == OI_HANDICAP || m_nOddsInfoKind == OI_GOAL_OU || m_nOddsInfoKind == OI_TEAM1_GOAL_OU || m_nOddsInfoKind == OI_TEAM2_GOAL_OU)
	{
		if (m_hVal1 != OddsInfo.m_hVal1)
			return FALSE;
	}
	return TRUE;
}
BOOL COddsInfo::operator!=(COddsInfo& OddsInfo)
{
	if (m_nOddsInfoKind != OddsInfo.m_nOddsInfoKind)
		return TRUE;
	if (m_nOddsInfoKind == OI_HANDICAP || m_nOddsInfoKind == OI_GOAL_OU || m_nOddsInfoKind == OI_TEAM1_GOAL_OU || m_nOddsInfoKind == OI_TEAM2_GOAL_OU)
	{
		if (m_hVal1 != OddsInfo.m_hVal1)
			return TRUE;
	}

	return FALSE;
}
