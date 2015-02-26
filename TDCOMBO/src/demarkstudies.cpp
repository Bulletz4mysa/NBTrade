#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif
#include <math.h>

BLPVector<int **> CTDStudyWithCountdown::m_ppnCountdownDisplay(10);

extern void ConvertLineParameters(ChartLineParameters *pParams, int nParamCount);
extern void ConvertPointParameters(ChartPointParameters *pParams, int nParamCount);
extern void ConvertOscillatorParameters(OscillatorParameters *pParams);


BOOL CTDBreakoutQualifier::QualifyBreakout(IBLPGraphData *pGraph, CBLPStudy *pStudy, BOOL bUp, long nBreakoutIndex, float fBreakoutPrice)
{
	long N;
	pGraph->get_NumPoints(&N);

	float *pfOpen = pStudy->GetStudyDataLine(pGraph, CBLPStudy::Open);
	ASSERT(pfOpen != NULL);
	if (pfOpen == NULL)
		return E_HANDLE;

	float *pfClose = pStudy->GetStudyDataLine(pGraph, CBLPStudy::Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

	float *pfTrueHigh = pStudy->GetStudyDataLine(pGraph, CBLPStudy::TrueHigh);
	ASSERT(pfTrueHigh != NULL);
	if (pfTrueHigh == NULL)
		return E_HANDLE;

	float *pfTrueLow = pStudy->GetStudyDataLine(pGraph, CBLPStudy::TrueLow);
	ASSERT(pfTrueHigh != NULL);
	if (pfTrueHigh == NULL)
		return E_HANDLE;

	float *pfLow = pStudy->GetStudyDataLine(pGraph, CBLPStudy::Low);
	ASSERT(pfLow!= NULL);
	if (pfLow == NULL)
		return E_HANDLE;

	float *pfHigh = pStudy->GetStudyDataLine(pGraph, CBLPStudy::High);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;

	BOOL bQualified = !(m_bQualifier1 || m_bQualifier2 || m_bQualifier1);

	if (bUp) {
		if (m_bQualifier1 && ((CBLPStudy::CBLPPrice)pfClose[nBreakoutIndex - 1] < pfClose[nBreakoutIndex - 2]) )
				bQualified = TRUE;

		else if (m_bQualifier2 &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] > pfClose[nBreakoutIndex - 1]) &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] > fBreakoutPrice) &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] < pfHigh[nBreakoutIndex]) )
				bQualified = TRUE;

		else if (m_bQualifier3 &&
			(((CBLPStudy::CBLPPrice)pfClose[nBreakoutIndex - 1] + pfClose[nBreakoutIndex - 1] - pfTrueLow[nBreakoutIndex - 1]) < fBreakoutPrice) )
				bQualified = TRUE;
		}
	else {
		if (m_bQualifier1 && ((CBLPStudy::CBLPPrice)pfClose[nBreakoutIndex - 1] > pfClose[nBreakoutIndex - 2]) )
				bQualified = TRUE;

		else if (m_bQualifier2 &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] < pfClose[nBreakoutIndex - 1]) &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] < fBreakoutPrice) &&
			((CBLPStudy::CBLPPrice)pfOpen[nBreakoutIndex] > pfLow[nBreakoutIndex]) )
				bQualified = TRUE;

		else if (m_bQualifier3 &&
			(((CBLPStudy::CBLPPrice)pfClose[nBreakoutIndex - 1] + pfClose[nBreakoutIndex - 1] - pfTrueHigh[nBreakoutIndex - 1]) > fBreakoutPrice) )
				bQualified = TRUE;
		}

	return bQualified;
}


CBLPStudy2 *CTDStudy::CreateStudyInstance(unsigned long nMsgId)
{
	CBLPStudy2 *pStudy;
#ifdef jfkDEBUG
	static BOOL bDeMarker = FALSE, bRetracement = FALSE;
#endif
	switch (nMsgId) {
		case blgTDREI:
			pStudy = new CTDRei;
			break;

	    case blgTDMAVG1:
			pStudy = new CTDMavg1;
			break;

		case blgTDRangeProjections:
			pStudy = new CTDRangeProjections;
			break;

		case blgTDChannel1 : 
		case blgTDChannel2 : 
			pStudy = new CTDChannel;
			break;

		case blgTDDeMarker1 :
			pStudy = new CTDDeMarker1;
			break;

		case blgTDDeMarker2:
			pStudy = new CTDDeMarker2;
			break;

		case blgTDPressure :
			pStudy = new CTDPressure;
			break;

		case blgTDSequential :
			pStudy = new CTDSequential;
			break;

		case blgTDCombo :
			pStudy = new CTDCombo;
			break;

		case blgTDLines :
		case blgTDLinesEx:
			pStudy = new CTDLines;
			break;

		case blgTDRelativeRetracement:
			pStudy = new CTDRelativeRetracement;
			break;

		case blgTDAbsoluteRetracement:
			pStudy = new CTDAbsoluteRetracement;
			break;

		case blgTDTrendFactor:
			pStudy = new CTDTrendFactor;
			break;

		case blgTDWave:
			pStudy = new CTDDWave;
			break;


		default :
			return NULL; // jfk: In the future, we may want to throw an exception here
		}

	return pStudy;
}
