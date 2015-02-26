#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif


// IBLPStudy overrides
HRESULT CTDRangeProjections::ConvertBuf(void *pBigBuf)
{
	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pBuf = (StudyMessagePtr)pBigBuf;

// jfk: Note this parameter does not seem to be aligned properly when it comes
//	down from the big, so I kludged it back one byte.
	m_bShowAll = pBuf->TDRangeProjections.use_every_bar;
	m_bUseTrueHighLow = pBuf->TDRangeProjections.use_true_prices;

	m_pLineParameters = (ChartLineParameters *)((char *)(pBuf->TDRangeProjections.line_params) );
	ConvertLineParameters( m_pLineParameters, 2 );
	return NOERROR;
}

HRESULT CTDRangeProjections::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartPointParameters PointParms[2] = 
		{{XRT_POINT_NONE,0,0},
		{XRT_POINT_NONE,0,0}};
			
	ChartLineParameters LineParms[2] = 
	{
	{1, XRT_LPAT_SOLID, RGB(0, 255, 0)},	
	{1, XRT_LPAT_SOLID, RGB(255, 0, 0)}};

	LineParms[0].line_color = m_pLineParameters[0].line_color;
	LineParms[1].line_color = m_pLineParameters[1].line_color;

	HRESULT rc = AddStudy(pGraph, 2, 2, LineParms, PointParms);
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, m_strButtonText, this);
		}

	pSD->put_StudyStyle(0, STUDY_STYLE_OPEN_STEP);
	pSD->put_StudyStyle(1, STUDY_STYLE_OPEN_STEP);

	return rc;//CBLPStudy::CreateStudy(pGraph);
}


HRESULT CTDRangeProjections::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints);

	float	*pfHigh, *pfLow, *pfOpen, *pfClose;
	CBLPPrice trueLow, trueHigh;

	BLPVector<float> vTDRangeProjectionsHigh(m_nNumPoints + 1, HOLEF);
	BLPVector<float> vTDRangeProjectionsLow(m_nNumPoints + 1, HOLEF);
//	float	*pfTDRangeProjectionsHigh = (vector<float>(m_nNumPoints + 1, HOLEF)).begin(),
//			*pfTDRangeProjectionsLow  = (vector<float>(m_nNumPoints + 1, HOLEF)).begin();
	CBLPPrice	x;
	long	i;

	// Extend study into the future by one day.
	HRESULT hr = pSD->put_NumStudyPoints(m_nNumPoints + 1);
	pGraph->put_NumFuturePoints(m_nNumPoints + 1);

	if (m_bUseTrueHighLow) {
		pGraph->get_TrueHighData(m_nNumPoints, &pfHigh);
		pGraph->get_TrueLowData(m_nNumPoints, &pfLow);
		}
	else {
		pGraph->get_HighData(m_nNumPoints, &pfHigh);
		pGraph->get_LowData(m_nNumPoints, &pfLow);
		}

	pGraph->get_CloseData(m_nNumPoints, &pfClose);
	pGraph->get_OpenData(m_nNumPoints, &pfOpen);

	for ( i = 1; i < m_nNumPoints; i++ ) {
		if (!m_bShowAll && (i < m_nNumPoints - 2))
			continue;
		if ( ( pfOpen[i] == HOLEF ) || ( pfOpen[i] == INTERPOLATEF ) )
			continue;

		trueHigh = pfHigh[i];
		trueLow = pfLow[i];

		if ( (CBLPPrice)pfClose[i] < pfOpen[i] ) 
			x = ( trueHigh + (trueLow * 2.0f) + (CBLPPrice)pfClose[i]) * 0.5f; 
		else if ( (CBLPPrice)pfClose[i] > pfOpen[i] )
			x = ((trueHigh * 2.0f) + trueLow + (CBLPPrice)pfClose[i]) * 0.5f;
		else
			x = ( trueHigh + trueLow + ((CBLPPrice)pfClose[i] * 2.0f)) * 0.5f;

		vTDRangeProjectionsHigh[i+1] = x - trueLow;
		vTDRangeProjectionsLow[i+1] = x - trueHigh;
		}
	
	m_nNumPoints++;
    SetStudyDataLine(0, &vTDRangeProjectionsHigh[0]);
    SetStudyDataLine(1, &vTDRangeProjectionsLow[0]);
	return NOERROR;
}
