#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

/***********************************************************************************
 ***********************  TD Demarker 1 ******************************************** 
 ***********************************************************************************/

HRESULT CTDDeMarker1::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr pData = (StudyMessagePtr)pBuf;

	pData->DeMarkerPeriod = ntohs(pData->DeMarkerPeriod);
	pData->duration = ntohs(pData->duration);
	ConvertOscillatorParameters( &pData->zones );
	pData->high_lookback = ntohs(pData->high_lookback);
	pData->low_lookback = ntohs(pData->low_lookback);
	pData->MavgPeriod = ntohs(pData->MavgPeriod);
	pData->smoothing = ntohs(pData->smoothing );

	ConvertLineParameters(pData->line_params, 1);

	if (pData->duration != 0)
		m_nDuration = pData->duration;
	m_nHighLookback = pData->high_lookback;
	m_nLowLookback = pData->low_lookback;
	m_nMavgPeriod = pData->MavgPeriod;
	m_nPeriod = pData->DeMarkerPeriod; //jfk : Note the BIG is ALWAYS sending 16 right now.
	m_zones.overbought = pData->zones.overbought;
	m_zones.oversold = pData->zones.oversold;
// These choices are impletmented on the big for TDREI
	m_nPriceHigh = (enum tagEnumPrice)(pData->higher_id[1] - '0');	// Added by Lu Sun
	m_nPriceLow = (enum tagEnumPrice)(pData->lower_id[1] - '0');	// Added by Lu Sun
	m_nSmooth = pData->smoothing;
	m_pLineParameters = pData->line_params;

	// following lines added by Lu Sun
	pData->version = ntohs(pData->version );	// added by Lu Sun
	m_nVersion = pData->version;				// added by Lu Sun

	if (m_nVersion == 1) {
		ConvertLineParameters(&(pData->ma_line_params), 1);	// added by Lu Sun
		m_pMALineParameter = &(pData->ma_line_params);	// added by Lu Sun
		m_nDurationOn = pData->duration_on;			// added by Lu Sun
		pData->ma_length = ntohs(pData->ma_length);	// added by Lu Sun
		m_nShowMALine = pData->show_maline;			// added by Lu Sun
		m_nMALength = pData->ma_length;				// added by Lu Sun
	}
    else {
		m_nDurationOn = 1;
		m_nShowMALine = 0;		
	}

	return NOERROR;
}


HRESULT CTDDeMarker1::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartLineParameters LineParms[] = 
	{
	{1, XRT_LPAT_SOLID, RGB(255, 255, 255)},	// DeMarker line
	{1, XRT_LPAT_SOLID, RGB(255, 255, 255)},	// DeMarker moving average line - Lu Sun
	{1, XRT_TOP_FILL, RGB(255, 0, 255)},		// Upper duration
	{1, XRT_LPAT_SOLID, RGB(255, 0, 255)}};	// Lower duration
	ChartPointParameters PointParms[] = 
	{{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0},						// added by Lu Sun
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0}};
	// Overwrite the first of each group with study parameters.
	if (m_pLineParameters != NULL)
		memcpy( (void*)(LineParms), m_pLineParameters, sizeof( ChartLineParameters ) );
	if (m_nShowMALine != 0 && m_pMALineParameter != NULL) {
		memcpy( (void*)(LineParms+1), m_pMALineParameter, sizeof( ChartLineParameters ) );
	}
	       // These lines are added by Lu Sun
	AddStudy(pGraph,9, 1, LineParms, PointParms);

	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(LineParms[0]);
	pChartLine->SetPointParams(PointParms[0]);
	m_vChartLine.push_back(pChartLine);

	// begin of code added by Lu Sun
	if (m_nShowMALine != 0) {
		CChartLine *pChartMALine = new CChartLine;
		pChartMALine->SetLineParams(LineParms[1]);
		pChartMALine->SetPointParams(PointParms[1]);
		m_vChartLine.push_back(pChartMALine);
	}
	// end of code added by Lu Sun

	m_pDurationTopLine = new CChartLine;
	m_pDurationTopLine->SetLineParams(LineParms[2]);		// modified by Lu Sun
	m_pDurationTopLine->SetPointParams(PointParms[2]);		// modified by Lu Sun

	m_pDurationBottomLine = new CChartLine;
	m_pDurationBottomLine->SetLineParams(LineParms[3]);		// modified by Lu Sun
	m_pDurationBottomLine->SetPointParams(PointParms[3]);	// modified by Lu Sun

	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK)
		AddChart(iChart);
	else
		return S_FALSE;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	iGraphToolbar->AddStudyButton(0, m_strButtonText, this);

	return S_OK;
}


HRESULT CTDDeMarker1::DoStudy(IBLPGraphData *pGraph)
{
	if (m_vCharts.empty())
		return E_HANDLE;

	pGraph->get_NumPoints(&m_nNumPoints);

// define study lines
	BLPVector<float> vIndicator(m_nNumPoints, HOLEF);
	BLPVector<float> vShadeTop(m_nNumPoints, HOLEF);
	BLPVector<float> vShadeBottom(m_nNumPoints, HOLEF);
	float *pfIndicator = &vIndicator[0];
	float *pfShadeTop = &vShadeTop[0];
	float *pfShadeBottom = &vShadeBottom[0];

// use already allocated space for temporary values	
	float *pfTempHighs = pfShadeTop;
	float *pfTempLows = pfShadeBottom;

// extract parameters we need from Graph data
	float *pfHigh = GetStudyDataLine(pGraph, m_nPriceHigh);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;

	float *pfLow = GetStudyDataLine(pGraph, m_nPriceLow);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;

    int i;
        
// Calculate Temporary values
	for (i = 1; i < m_nNumPoints; i++) {
		CBLPPrice fTemp;
		fTemp = (CBLPPrice)pfHigh[i] - pfHigh[i - m_nHighLookback];

		pfTempHighs[i] = (fTemp > 0.0f) ? (float)fTemp : 0.0f;
		
		fTemp = (CBLPPrice)pfLow[i - m_nLowLookback] - pfLow[i];
		pfTempLows[i] = (fTemp > 0.0f) ? (float)fTemp : 0.0f;
	}

// Calculate indicator line
	for (i = m_nPeriod; i < m_nNumPoints; i++) {
		CBLPPrice fNumerator = 0, fDenominator = 0;
		for (int j = i - m_nPeriod + 1; j <= i; j++) {
			fNumerator += (CBLPPrice)pfTempHighs[j];
			fDenominator += (CBLPPrice)pfTempLows[j];
			}
//AfxDebugBreak();
		pfIndicator[i] = ((CBLPPrice)fNumerator * 100.0f) / (fNumerator + fDenominator);
		}

// HOLEF out all temporary values; create shaded sectors
	int nBarCountOver = 0, nBarCountUnder = 0;
	for (i = 0; i < m_nNumPoints; i++ ) {
		if ((CBLPPrice)pfIndicator[i] >= (CBLPPrice)m_zones.overbought) {
			nBarCountOver++;
			nBarCountUnder = 0;
			}
		else if ((CBLPPrice)pfIndicator[i] < (CBLPPrice)m_zones.overbought) {
			nBarCountOver = 0;

			if ((CBLPPrice)pfIndicator[i] <= (CBLPPrice)m_zones.oversold)
				nBarCountUnder++;
			else
				nBarCountUnder = 0;
			}

		if (m_nDurationOn !=0 && nBarCountOver >= m_nDuration) {		// Modified by Lu Sun
			pfShadeTop[i] = 100.0f;
			pfShadeBottom[i] = m_zones.overbought;
			}
		else if (m_nDurationOn !=0 && nBarCountUnder >= m_nDuration) {	// Modified by Lu Sun
			pfShadeTop[i] = m_zones.oversold;
			pfShadeBottom[i] = 0.0f;
			}
		else {
			pfShadeTop[i] = HOLEF;
			pfShadeBottom[i] = HOLEF;
			}
		}

	CComPtr<IStudyData> iSD;
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	// Copy the study lines onto the Oscillator Chart
	iSD->put_StudyDataLine(0, m_nNumPoints, pfIndicator);
	// begin of code added by Lu Sun
	float *pfTDMAVG1;
	pfTDMAVG1 = new float[m_nNumPoints];
	pGraph->MovingAverage(m_nMALength, m_nNumPoints, pfIndicator, &pfTDMAVG1);
	i = 1;
	if (m_nShowMALine != 0) {
		iSD->put_StudyDataLine(i++, m_nNumPoints, pfTDMAVG1); 
	}
	// end of added by Lu Sun

	iSD->put_StudyDataLine(i++, m_nNumPoints, pfShadeTop);		// modified to i++ by Lu Sun
	iSD->put_StudyDataLine(i++, m_nNumPoints, pfShadeBottom);	// modified to i++ by Lu Sun

	return NOERROR;
}


float *CTDDeMarker1::GetStudyDataLine(IBLPGraphData *pGraph, enumPriceType type)
{	
//	float *pfDataLine = CBLPStudy::GetStudyDataLine(pGraph, type);	//commnented out by Lu Sun
	float *pfDataLine = GetExtStudyDataLine(pGraph, type);
	// the above line was changed for Midpoint/HLCAverage which was not covered in
	// CBLPStudy::GetStudyDataLine - by Lu Sun

	if (pfDataLine == NULL)
		return NULL;

	return pfDataLine;
}

// This following method was added by Lu Sun
float *CTDDeMarker1::GetExtStudyDataLine(IBLPGraphData *pGraph, enumPriceType type)
{	
	float *pfDataHigh;
	float *pfDataLow;
	float *pfDataClose;
	int i;
	static float *pfDataLine = NULL;

	switch (type) {
	case Open:
	case High:
	case Low:
	case Close:
	case TrueHigh:
	case TrueLow:
		return CBLPStudy::GetStudyDataLine(pGraph, type);
	case Midpoint:
		pfDataHigh = CBLPStudy::GetStudyDataLine(pGraph, High);
		pfDataLow = CBLPStudy::GetStudyDataLine(pGraph, Low);
		
		if (pfDataLine) delete pfDataLine;
		pfDataLine = new float[m_nNumPoints];
		for (i = 0; i < m_nNumPoints; i++) {
			if (IsInvalid(pfDataHigh[i]) || IsInvalid(pfDataLow[i])) 
				pfDataLine[i] = HOLEF;
			else pfDataLine[i] = (pfDataHigh[i]+pfDataLow[i])/2;
		}

		break;
	case HLCAverage:
		pfDataHigh = CBLPStudy::GetStudyDataLine(pGraph, High);
		pfDataLow = CBLPStudy::GetStudyDataLine(pGraph, Low);
		pfDataClose = CBLPStudy::GetStudyDataLine(pGraph, Close);
		
		if (pfDataLine) delete pfDataLine;
		pfDataLine = new float[m_nNumPoints];
		for (i = 0; i < m_nNumPoints; i++) {
			if (IsInvalid(pfDataHigh[i]) || IsInvalid(pfDataLow[i]) || IsInvalid(pfDataClose[i])) 
				pfDataLine[i] = HOLEF;
			else pfDataLine[i] = (pfDataHigh[i]+pfDataLow[i]+pfDataClose[i])/3;
		}

		break;
	default:	return NULL;
	}

	return pfDataLine;
}


/***********************************************************************************
 ***********************  TD Demarker 2 ******************************************** 
 ***********************************************************************************/

HRESULT CTDDeMarker2::CreateStudy(IBLPGraphStudyList * pGraph)
{

	ChartLineParameters LineParms[] = 
	{
	{1, XRT_LPAT_SOLID, RGB(255, 255, 255)},	// DeMarker line
	{1, XRT_LPAT_SOLID, RGB(255, 255, 255)},	// DeMarker moving average line - Lu Sun
	{1, XRT_TOP_FILL, RGB(255, 0, 255)},		// Upper duration
	{1, XRT_LPAT_SOLID, RGB(255, 0, 255)}};	// Lower duration

	ChartPointParameters PointParms[] = 
	{{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0},						// added by Lu Sun
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0}};

	if (m_pLineParameters != NULL)
		memcpy( (void*)(LineParms), m_pLineParameters, sizeof( ChartLineParameters ) );
	if (m_nShowMALine != 0 && m_pMALineParameter != NULL) {
		memcpy( (void*)(LineParms+1), m_pMALineParameter, sizeof( ChartLineParameters ) );
	}
	       // These lines are added by Lu Sun

	HRESULT rc = AddStudy(pGraph,3, 0, LineParms, PointParms);
	if (FAILED(rc))
		return rc;

	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(LineParms[0]);
	pChartLine->SetPointParams(PointParms[0]);
	m_vChartLine.push_back(pChartLine);

	// begin of code added by Lu Sun
	if (m_nShowMALine != 0) {
		CChartLine *pChartMALine = new CChartLine;
		pChartMALine->SetLineParams(LineParms[1]);	
		pChartMALine->SetPointParams(PointParms[1]);
		m_vChartLine.push_back(pChartMALine);
	}
	// end of code added by Lu Sun

	m_pDurationTopLine = new CChartLine;
	m_pDurationTopLine->SetLineParams(LineParms[2]);		// modified by Lu Sun	
	m_pDurationTopLine->SetPointParams(PointParms[2]);		// modified by Lu Sun	

	m_pDurationBottomLine = new CChartLine;
	m_pDurationBottomLine->SetLineParams(LineParms[3]);		// modified by Lu Sun	
	m_pDurationBottomLine->SetPointParams(PointParms[3]);	// modified by Lu Sun	

	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK)
		AddChart(iChart);
	else
		return S_FALSE;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	iGraphToolbar->AddStudyButton(0, m_strButtonText, this);

	return rc;
} //	return CTDDeMarker2::CreateStudy(pGraph);

HRESULT CTDDeMarker2::DoStudy(IBLPGraphData *pGraph)
{
	if (m_vCharts.empty())
		return E_HANDLE;

// define five study lines in the lower chart
	float *pfIndicator = GetStudyDataPoints(0); // draw indicator after zones
	float *pfShadeTop = GetStudyDataPoints(1);
	float *pfShadeBottom = GetStudyDataPoints(2);

// extract parameters we need from Graph data
	pGraph->get_NumPoints(&m_nNumPoints);

	float *pfHigh = GetStudyDataLine(pGraph, m_nPriceHigh);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;

	float *pfLow = GetStudyDataLine(pGraph, m_nPriceLow);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

	float *pfHighDiffs = new float[m_nNumPoints]; 
	float *pfLowDiffs = new float[m_nNumPoints];

// Setup indicator bands
    int i;

// Calculate Diff values
	short nLookback = (m_nHighLookback > m_nLowLookback) ? m_nHighLookback : m_nLowLookback;
	for (i = nLookback; i < m_nNumPoints; i++) {
		pfHighDiffs[i] = (CBLPPrice)pfHigh[i] - pfClose[i - m_nHighLookback];
		if ((CBLPPrice)pfHighDiffs[i] < 0.0f)
			pfHighDiffs[i] = 0;
		pfHighDiffs[i] += (pfClose[i] - pfLow[i]);

		pfLowDiffs[i] = (CBLPPrice)pfClose[i - m_nLowLookback] - pfLow[i];
		if ((CBLPPrice)pfLowDiffs[i] < 0.0f)
			pfLowDiffs[i] = 0;
		pfLowDiffs[i] += (pfHigh[i] - pfClose[i]);
		}

// Calculate indicator line
	for (i = m_nPeriod; i < m_nNumPoints; i++) {
		CBLPPrice fNumerator = 0, fDenominator = 0;
		for (int j = i - m_nPeriod + 1; j <= i; j++) {
			fNumerator += (CBLPPrice)pfHighDiffs[j];
			fDenominator += (CBLPPrice)pfLowDiffs[j];
			}
//AfxDebugBreak();
		pfIndicator[i] = fNumerator * 100.0f / (fNumerator + fDenominator);
		}

// HOLEF out all temporary values; create shaded sectors
	int nBarCountOver = 0, nBarCountUnder = 0;
	for (i = 0; i < m_nNumPoints; i++ ) {
		if ((CBLPPrice)pfIndicator[i] > (CBLPPrice)m_zones.overbought) {
			nBarCountOver++;
			nBarCountUnder = 0;
			}
		else {
			nBarCountOver = 0;
			if ((CBLPPrice)pfIndicator[i] < (CBLPPrice)m_zones.oversold)
				nBarCountUnder++;
			else
				nBarCountUnder = 0;
			}
		
// Create areas where appropriate for arrow algorithm, but no shading yet
		if (m_nDurationOn !=0 && nBarCountOver >= m_nDuration) {		// modified by Lu Sun
			pfShadeTop[i] = 100.0;
			pfShadeBottom[i] = HOLEF;
			}
		else if (m_nDurationOn !=0 && nBarCountUnder >= m_nDuration) {	// modified by Lu Sun
			pfShadeBottom[i] = 0.0;
			pfShadeTop[i] = HOLEF;
			}
		else {
			pfShadeTop[i] = HOLEF;
			pfShadeBottom[i] = HOLEF;
			}
		}


// Create shaded indicators where necessary
	for (i = 0; i < m_nNumPoints; i++ ) {
		if ((pfIndicator[i] == HOLEF) && (i !=0)) {
                    int j;
                    
			for (j = i + 1; j < m_nNumPoints; j++)
				if (pfIndicator[j] != HOLEF)
					break;
			if (j != m_nNumPoints)
				pfIndicator[i] = ((CBLPPrice)pfIndicator[i - 1] + pfIndicator[j]) * 0.5f;
			}

		if (pfShadeTop[i] == 100.0)
			pfShadeBottom[i] = m_zones.overbought;
		else if (pfShadeBottom[i] == 0.0)
			pfShadeTop[i] = m_zones.oversold;
		}

	// Copy the study lines onto the Oscillator Chart
	CComPtr<IStudyData> iSD;
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	iSD->put_StudyDataLine(0, m_nNumPoints, pfIndicator);
	// begin of code added by Lu Sun 
	float *pfTDMAVG1;
	pfTDMAVG1 = new float[m_nNumPoints];
	pGraph->MovingAverage(m_nMALength, m_nNumPoints, pfIndicator, &pfTDMAVG1);
	i = 1;
	if (m_nShowMALine != 0) {
		iSD->put_StudyDataLine(i++, m_nNumPoints, pfTDMAVG1); 
	}
	// end of added by Lu Sun

	iSD->put_StudyDataLine(i++, m_nNumPoints, pfShadeTop);		// modified to i++ by Lu Sun
	iSD->put_StudyDataLine(i++, m_nNumPoints, pfShadeBottom);	// modified to i++ by Lu Sun

	delete[] pfHighDiffs; 
	delete[] pfLowDiffs;
	return NOERROR;	
}//return CTDDeMarker2::DoStudy(pGraph);


HRESULT CTDDeMarker2::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr pData = (StudyMessagePtr)pBuf;

	pData->rei_period = ntohs(pData->rei_period);
	pData->duration = ntohs(pData->duration);
	ConvertOscillatorParameters( &pData->zones );
	pData->high_lookback = ntohs(pData->high_lookback);
	pData->low_lookback = ntohs(pData->low_lookback);
	pData->period = ntohs(pData->period);
	pData->smoothing = ntohs(pData->smoothing );
	pData->Qualifier = ntohs(pData->Qualifier);
	pData->UseAltOscillator = ntohs(pData->UseAltOscillator);
	pData->DisplayGaps = ntohs(pData->DisplayGaps);
	pData->BarsBack = ntohs(pData->BarsBack);

    long *ptemp = (long *)&(pData->UpperNeutral);
	*ptemp = ntohl(*ptemp);

    ptemp = (long *)&(pData->LowerNeutral);
	*ptemp = ntohl(*ptemp);

	ConvertLineParameters(pData->line_params, 1);
	ConvertPointParameters(pData->point_params, 3);

	m_nDuration = pData->duration;
	m_nHighLookback = pData->high_lookback;
	m_nLowLookback = pData->low_lookback;
	m_nMavgPeriod = pData->period;
	m_nPeriod = pData->rei_period;
	m_zones.overbought = pData->zones.overbought;
	m_zones.oversold = pData->zones.oversold;
	m_nPriceHigh = (enum tagEnumPrice)pData->higher_id[0]; 
	m_nPriceLow = (enum tagEnumPrice)pData->lower_id[0];
	m_bCenterMavg = pData->center;
	m_nSmooth = pData->smoothing;

//	m_nQualifier = (enum tagEnumTDPOQ) pData->Qualifier;
	m_bUseAltOscillator = pData->UseAltOscillator;
	m_bDisplayGaps = pData->DisplayGaps;
	m_nBarsBack = pData->BarsBack;
	m_fUpperNeutral = pData->UpperNeutral;
	m_fLowerNeutral = pData->LowerNeutral;

	m_pLineParameters = pData->line_params;
	m_pPointParameters = pData->point_params;

	// following lines added by Lu Sun
	pData->version = ntohs(pData->version );	// added by Lu Sun
	m_nVersion = pData->version;				// added by Lu Sun

	if (m_nVersion == 1) {
		ConvertLineParameters(&(pData->ma_line_params), 1);	// added by Lu Sun
		m_pMALineParameter = &(pData->ma_line_params);	// added by Lu Sun
		m_nDurationOn = pData->duration_on;			// added by Lu Sun 
		pData->ma_length = ntohs(pData->ma_length);	// added by Lu Sun 
		m_nShowMALine = pData->show_maline;			// added by Lu Sun 
		m_nMALength = pData->ma_length;				// added by Lu Sun
	}
	else {
		m_nDurationOn = 1;
		m_nShowMALine = 0;		
	}

	return NOERROR;

}	//return CTDDeMarker2::ConvertBuf(pBuf);}
