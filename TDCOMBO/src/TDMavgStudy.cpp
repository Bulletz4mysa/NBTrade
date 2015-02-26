#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

HRESULT CTDMavg1::DoStudy(IBLPGraphData *pGraph)
{
	// Calculate both 5-day moving averages on highs and lows.
	// For each day, if the current high (low) is higher (lower) than the 
	// last 12 days' highs (lows), the study value is the high (low) for that 
	// day and the next 3 days.
	float	*pfHigh, *pfLow, *pfTDMAVG1High, *pfTDMAVG1Low;
	BOOL	bUseHigh, bUseLow, *pbUseHigh, *pbUseLow;//, bTDMAVG1Smoothing;
	long	N, i, j;

	pGraph->get_NumPoints(&N); //m_GraphData.number_of_points;
	
	pfHigh = GetStudyCalcRoute(pGraph, m_zHigherId);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	pfLow = GetStudyCalcRoute(pGraph, m_zLowerId);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_INVALIDARG;

	if ( !(pfTDMAVG1High = GetStudyDataPoints(1)) )
		return E_HANDLE;
	
	if ( !(pfTDMAVG1Low = GetStudyDataPoints(0)) )
		return E_HANDLE;

	if ( m_nPeriod < 1 )
		return E_HANDLE;

	switch (m_nType) { // Take the moving average specfied by the BIG
	  case none:
		  for (i = 0; i < N; i++) {
			pfTDMAVG1Low[i] = pfLow[i];
			pfTDMAVG1High[i] = pfHigh[i];
			}
		  break;

	  case exponential:
		pGraph->ExponentialAverage(m_nPeriod, N, pfHigh, &pfTDMAVG1High);
		pGraph->ExponentialAverage(m_nPeriod, N, pfLow, &pfTDMAVG1Low);
		break;

	  case weighted:
		for (i = m_nPeriod - 1; i < N; i++) {
			int nBarsBack;
			pfTDMAVG1Low[i] = 0;
			pfTDMAVG1High[i] = 0;
			long nLowSumWeights = 0, nHighSumWeights = 0;
			for (nBarsBack = 0; nBarsBack < m_nPeriod; nBarsBack++) {
				if (pfLow[i - nBarsBack] != HOLEF) {
					pfTDMAVG1Low[i] += pfLow[i - nBarsBack] * (m_nPeriod - nBarsBack);
					nLowSumWeights += (m_nPeriod - nBarsBack);
					}
				if (pfHigh[nBarsBack] != HOLEF) {
					pfTDMAVG1High[i] += pfHigh[i - nBarsBack] * (m_nPeriod - nBarsBack);
					nHighSumWeights += (m_nPeriod - nBarsBack);
					}
				}

			pfTDMAVG1Low[i] /= nLowSumWeights;
			pfTDMAVG1High[i] /= nHighSumWeights;
			}
		break;

	  case simple:
	  default:
		pGraph->MovingAverage(m_nPeriod, N, pfHigh, &pfTDMAVG1High);
		pGraph->MovingAverage(m_nPeriod, N, pfLow, &pfTDMAVG1Low);
		break;
	}


	pbUseHigh = new BOOL[N];
	pbUseLow = new BOOL[N];

	for (i = 0; i < m_nPeriod + m_nLookback; i++) {
		pfTDMAVG1High[i] = HOLEF;
		pfTDMAVG1Low[i] = HOLEF;
		}
	
	for (i = 0; i < N; i++) {
		pbUseHigh[i] = FALSE;
		pbUseLow[i] = FALSE;
		}

//Reset pfHigh and pfLow for the lookback comparisons
	pfHigh = GetStudyDataLine(pGraph, High);
	pfLow = GetStudyDataLine(pGraph, Low);

	for ( i = m_nPeriod + m_nLookback; i < N; i++ )
	{
		// If the current high is lower than the last Lookback highs,
		// use the mavg of the highs. If the current low is higher than
		// the last Lookback lows, use the mavg of the lows.
		bUseHigh = bUseLow = TRUE;
		for ( j = i - m_nLookback; j < i; j++ ) {
			bUseHigh = bUseHigh && ((CBLPPrice)pfLow[i] > pfLow[j]);
			bUseLow = bUseLow && ((CBLPPrice)pfHigh[i] < pfHigh[j]);
			// Speed up slightly - once both have failed, we can skip.
			if ( !bUseHigh && !bUseLow )
				break;
			}

		if ( bUseHigh ) 
			// set next TDMAVG1Projection study points to MAVGWidth-moving average of pfHigh
			for ( j = 0; j < 4; j++ ) {
				if ((i + j) < N)
					pbUseHigh[i + j] = TRUE;
				}

		if (pbUseHigh[i])
			pfTDMAVG1High[i] *= m_fHighFactor;
		else
			pfTDMAVG1High[i] = HOLEF;

		if ( bUseLow )
			for ( j = 0; j < 4; j++ ) {
				if ((i + j) < N)
					pbUseLow[i + j] = TRUE;
				}

		if (pbUseLow[i])
			pfTDMAVG1Low[i] *= m_fLowFactor;
		else
			pfTDMAVG1Low[i] = HOLEF;

		}
	
	delete[] pbUseHigh;
	delete[] pbUseLow;

	return NOERROR;//CBLPStudy::DoStudy(pGraph);
}


HRESULT CTDMavg1::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartLineParameters LineParms[2] = 
	{
	{1, XRT_LPAT_SOLID, RGB(0, 255, 0)},	
	{1, XRT_LPAT_SOLID, RGB(255, 0, 0)}};

	ChartPointParameters PointParms[2] = 
	{{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0}};
//PointParms[0].point_color
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

	return rc;
}

HRESULT CTDMavg1::ConvertBuf(void * pBigBuf)
{
	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;
	pHead->TDMavg1.period = ntohs(pHead->TDMavg1.period);
	pHead->TDMavg1.smoothing = ntohs(pHead->TDMavg1.smoothing);
	pHead->TDMavg1.high_factor = ntohs(pHead->TDMavg1.high_factor);
	pHead->TDMavg1.low_factor = ntohs(pHead->TDMavg1.low_factor);
	pHead->TDMavg1.lookback = ntohs(pHead->TDMavg1.lookback);

	ConvertLineParameters(pHead->TDMavg1.line_params, 2);
//	ConvertPointParameters(pHead->TDMavg1.point_params, 2);
	
	if (pHead->TDMavg1.period != 0)
		m_nPeriod = pHead->TDMavg1.period;		
	if (pHead->TDMavg1.lookback != 0)
		m_nLookback = pHead->TDMavg1.lookback;		
	m_nType = (enum tagEnumMavg)pHead->TDMavg1.type;
	m_bCenter = pHead->TDMavg1.center;
	m_nSmoothing = pHead->TDMavg1.smoothing;
	m_fHighFactor = pHead->TDMavg1.high_factor / 100.0f; // Multiple of the high
	m_fLowFactor = pHead->TDMavg1.low_factor / 100.0f; // Multiple of the low

	m_pLineParameters = pHead->TDMavg1.line_params;
//	m_pPointParameters = pHead->TDMavg1.point_params;
//AfxDebugBreak();
	memset( m_zHigherId, 0, FIELD_LEN + 1 );
	strncpy(m_zHigherId, pHead->TDMavg1.higher_id, FIELD_LEN );
	memset( m_zLowerId, 0, FIELD_LEN + 1 );
	strncpy(m_zLowerId, pHead->TDMavg1.lower_id, FIELD_LEN );

	return NOERROR;
}
