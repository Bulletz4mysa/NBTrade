// legacystudies.cpp: implementation of the CBollingerBands class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "legacystudies.h"
#include "demarkstudies.h"
#include <fstring.h>
#include "compileropts.h"
#include "blpvector.h"
#include <math.h>

void ConvertChartColors(Chart_Color_Scheme *pParams)
{
  pParams->chart_background = htonl(pParams->chart_background);
  pParams->chart_foreground = htonl(pParams->chart_foreground);
  pParams->yaxis_background = htonl(pParams->yaxis_background);
  pParams->yaxis_foreground = htonl(pParams->yaxis_foreground);
  pParams->legend_background = htonl(pParams->legend_background);
  pParams->legend_foreground = htonl(pParams->legend_foreground);
  pParams->legend_border = htonl(pParams->legend_border);
#ifdef _DEBUG
  TRACE( "ConvertChartColors chart_background = 0x%X chart_foreground = 0x%X yaxis_background = 0x%X yaxis_foreground = 0x%X legend_background = 0x%X legend_foreground = 0x%X legend_border = 0x%X\n",
	  pParams->chart_background,
	  pParams->chart_foreground,
	  pParams->yaxis_background,
	  pParams->yaxis_foreground,
	  pParams->legend_background,
	  pParams->legend_foreground,
	  pParams->legend_border );
#endif
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBollingerBands::CBollingerBands()
{
	m_strCalcRtId = "#0";
}

CBollingerBands::~CBollingerBands()
{

}

HRESULT CBollingerBands::ConvertBuf(void * pBuf)
{
  CBLPStudy::ConvertBuf(pBuf);
  StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	
  m_nPeriod = ntohs(pHead->period);
	
  int *ptemp = (int *)&(pHead->percent_band1);
  *ptemp = ntohl(*ptemp);
  m_fUpperBandFactor = pHead->percent_band1;

  ptemp = (int *)&(pHead->percent_band2);
  *ptemp = ntohl(*ptemp);
  m_fLowerBandFactor = pHead->percent_band2;

  m_bShowBollBandwidth = pHead->show_boll_bandwidth;
  m_bShowBollPercentb = pHead->show_boll_percentb;

  pHead->boll_lead_lag = ntohs(pHead->boll_lead_lag);

  if (pHead->version >= 2)
    pHead->window_owner_id = ntohs(pHead->window_owner_id);

  ConvertLineParameters(pHead->line_params, 5);
  memcpy(m_LineParams, pHead->line_params, sizeof(m_LineParams));
  if (pHead->version >= 1) {
	ConvertPointParameters(pHead->point_params, 5);
	memcpy(m_PointParams, pHead->point_params, sizeof(m_PointParams));
	}

  m_strCalcRtId = pHead->field_id;
  if (pHead->version >= 1)
    ConvertChartColors(&pHead->bandwidth_chart_colors);
  if (pHead->version >= 1)
	ConvertChartColors(&pHead->percent_chart_colors);
	
	m_nBandwidthBackground = pHead->bandwidth_chart_colors.chart_background;
	m_nPercentBBackground = pHead->percent_chart_colors.chart_background;
  return S_OK;
}

HRESULT CBollingerBands::CreateStudy(IBLPGraphStudyList * pGraph)
{
	HRESULT rc;

	rc = AddStudy(pGraph, 5, 3, m_LineParams, m_PointParams);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "BOLL", this);
		}

	CComPtr<IStudyData> iStudyExtra;
	short nChartCount = 1;
	if (m_bShowBollBandwidth) {
		rc = AddStudyExtra(pGraph, nChartCount++, 3, 1, &(m_LineParams[3]), &(m_PointParams[3]), &iStudyExtra);
		ASSERT(SUCCEEDED(rc));

		iChart = NULL;
		if (SUCCEEDED(rc) && (iStudyExtra->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) ){
			AddChart(iChart);
			iChart->put_BackgroundColor(m_nBandwidthBackground);
			}
		}

	if (m_bShowBollPercentb) {
		iStudyExtra = NULL;
		rc = AddStudyExtra(pGraph, nChartCount, 4, 1, &(m_LineParams[4]), &(m_PointParams[4]), &iStudyExtra);
		ASSERT(SUCCEEDED(rc));

		iChart = NULL;
		if (SUCCEEDED(rc) && (iStudyExtra->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) ){
			AddChart(iChart);
			iChart->put_BackgroundColor(m_nPercentBBackground);
			}
		}

	return S_OK;
}

HRESULT CBollingerBands::DoStudy(IBLPGraphData *pGraph)
{
	long nBar;
	pGraph->get_NumPoints(&m_nNumPoints);
//	ASSERT(m_nPeriod <= m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfBollMavg(m_nNumPoints, HOLEF);
	BLPVector<float> vfUpperBand(m_nNumPoints, HOLEF);
	BLPVector<float> vfLowerBand(m_nNumPoints, HOLEF);
	BLPVector<float> vfBandwidth(m_nNumPoints, HOLEF);
	BLPVector<float> vfPercentB(m_nNumPoints, HOLEF);

	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	for (nBar = 0; nBar < m_nNumPoints; nBar++) {
		if (pfData[nBar] == HOLEF) {
			long nLastHole = nBar;
			for (nBar++; (nBar <= (nLastHole + m_nPeriod)) && (nBar < m_nNumPoints); nBar++) {
				if (pfData[nBar] == HOLEF)
					nLastHole = nBar;
				}
			if (nBar == m_nNumPoints)
				break;
			}

		if (nBar < m_nPeriod)
			continue;

		long nSumBar;
		float fSum = 0.0;
// compute Simple MAVG = main Boll line
		for (nSumBar = (nBar - m_nPeriod) + 1; nSumBar <= nBar; nSumBar++)
			fSum += pfData[nSumBar];
		
		vfBollMavg[nBar] = fSum / m_nPeriod;

// compute std dev:
		float fDiff;
		float fStdDev;
		fSum = 0.0;
		for (nSumBar = (nBar - m_nPeriod) + 1; nSumBar <= nBar; nSumBar++) {
			fDiff = pfData[nSumBar] - vfBollMavg[nBar];
			fSum += (fDiff * fDiff);
			}
		
		fStdDev = sqrt(fSum / m_nPeriod);
		vfUpperBand[nBar] = vfBollMavg[nBar] + m_fUpperBandFactor * fStdDev;
		vfLowerBand[nBar] = vfBollMavg[nBar] - m_fLowerBandFactor * fStdDev;

		float fWidth = vfUpperBand[nBar] - vfLowerBand[nBar];
		if (vfBollMavg[nBar] != 0.0)
			vfBandwidth[nBar] = 100.0 * fWidth / vfBollMavg[nBar];

		if (fWidth != 0)
			vfPercentB[nBar] = (pfData[nBar] - vfLowerBand[nBar]) / fWidth;
		}

	SetStudyDataLine(0, &vfBollMavg[0]);
	SetStudyDataLine(1, &vfUpperBand[0]);
	SetStudyDataLine(2, &vfLowerBand[0]);
	SetStudyDataLine(3, &vfBandwidth[0]);
	SetStudyDataLine(4, &vfPercentB[0]);
	return S_OK;
}

HRESULT CTE::CreateStudy(IBLPGraphStudyList * pGraph)
{
	m_PointParams[0].point_style = XRT_POINT_NONE;
	m_PointParams[1].point_style = XRT_POINT_HATCH_FILL;
	m_PointParams[1].point_color = m_LineParams[1].line_color;
	m_PointParams[2].point_style = XRT_POINT_NONE;

	if (m_PointParams[1].point_color == blgBlack)
		m_PointParams[1].point_color = 0x01;

	HRESULT rc;

	rc = AddStudy(pGraph, 3, 3, m_LineParams, m_PointParams);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "TE", this);
		}

	return S_OK;
}

HRESULT CTE::DoStudy(IBLPGraphData *pGraph)
{
	long nBar;
	pGraph->get_NumPoints(&m_nNumPoints);
	ASSERT(m_nPeriod <= m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfBollMavg(m_nNumPoints, HOLEF);
	BLPVector<float> vfUpperBand(m_nNumPoints, HOLEF);
	BLPVector<float> vfLowerBand(m_nNumPoints, HOLEF);

	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	for (nBar = 0; nBar < m_nNumPoints; nBar++) {
		if (pfData[nBar] == HOLEF) {
			long nLastHole = nBar;
			for (nBar++; (nBar <= (nLastHole + m_nPeriod)) && (nBar < m_nNumPoints); nBar++) {
				if (pfData[nBar] == HOLEF)
					nLastHole = nBar;
				}
			if (nBar == m_nNumPoints)
				break;
			}

		if (nBar < m_nPeriod)
			continue;

		long nSumBar;
		float fSum = 0.0;
// compute Simple MAVG = main Boll line
		for (nSumBar = (nBar - m_nPeriod) + 1; nSumBar <= nBar; nSumBar++)
			fSum += pfData[nSumBar];
		
		vfBollMavg[nBar] = fSum / m_nPeriod;

// compute std dev:
		float fDiff;
		float fStdDev;
		fSum = 0.0;
		for (nSumBar = (nBar - m_nPeriod) + 1; nSumBar <= nBar; nSumBar++) {
			fDiff = pfData[nSumBar] - vfBollMavg[nBar];
			fSum += (fDiff * fDiff);
			}
		
		fStdDev = sqrt(fSum / m_nPeriod);
		vfUpperBand[nBar] = vfBollMavg[nBar] + m_fUpperBandFactor * fStdDev;
		vfLowerBand[nBar] = vfBollMavg[nBar] - m_fLowerBandFactor * fStdDev;
		}

	SetStudyDataLine(0, &vfBollMavg[0]);
	SetStudyDataLine(1, &vfUpperBand[0]);
	SetStudyDataLine(2, &vfLowerBand[0]);
	return S_OK;
}

HRESULT CDirectionalMovementIndicator::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod = ntohs(pHead->period);

	m_bShowADX = pHead->show_adx;
	m_bShowADXR = pHead->show_adxr;

	if (pHead->version >= 2)
		pHead->window_owner_id = ntohs(pHead->window_owner_id);

	ConvertLineParameters(pHead->line_params, 4);
	memcpy(m_LineParams, pHead->line_params, 4 * sizeof(ChartLineParameters));

	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 4);
		memcpy(m_PointParams, pHead->point_params, 4 * sizeof(ChartPointParameters));

		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}
	return S_OK;
}

HRESULT CDirectionalMovementIndicator::CreateStudy(IBLPGraphStudyList * pGraph)
{
	HRESULT rc;
	m_nBaseId = 0;
	rc = AddStudy(pGraph, 4, 0, m_LineParams, m_PointParams);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	m_nBaseId = INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW;

	CChartLine *pDMIPlus = new CChartLine;
	pDMIPlus->SetLineParams(m_LineParams[0]);
	pDMIPlus->SetPointParams(m_PointParams[0]);
	m_vChartLine.push_back(pDMIPlus);

	CChartLine *pDMIMinus = new CChartLine;
	pDMIMinus->SetLineParams(m_LineParams[1]);
	pDMIMinus->SetPointParams(m_PointParams[1]);
	m_vChartLine.push_back(pDMIMinus);

	if (m_bShowADX) {
		CChartLine *pADX= new CChartLine;
		pADX->SetLineParams(m_LineParams[2]);
		pADX->SetPointParams(m_PointParams[2]);
		m_vChartLine.push_back(pADX);
		}
	else if (m_bShowADXR) {
		CChartLine *pADXR= new CChartLine;
		pADXR->SetLineParams(m_LineParams[3]);
		pADXR->SetPointParams(m_PointParams[3]);
		m_vChartLine.push_back(pADXR);
		}

	if (m_bShowADXR) {
		CChartLine *pADXR= new CChartLine;
		pADXR->SetLineParams(m_LineParams[3]);
		pADXR->SetPointParams(m_PointParams[3]);
		m_vChartLine.push_back(pADXR);
		}

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iGraphToolbar->AddStudyButton(0, "DMI", this);
		}

	return rc;
}

HRESULT CDirectionalMovementIndicator::DoStudy(IBLPGraphData *pGraph)
{
	if (m_nPeriod < 3)
		m_nPeriod = 3;

	pGraph->get_NumPoints(&m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return E_INVALIDARG;

	float *pfCloseOld = GetStudyDataLine(pGraph, Close);
	ASSERT(pfCloseOld != NULL);
	if (pfCloseOld == NULL)
		return E_HANDLE;

	float *pfHighOld = GetStudyDataLine(pGraph, High);
	if (pfHighOld == NULL)
		pfHighOld = pfCloseOld;

	float *pfLowOld = GetStudyDataLine(pGraph, Low);
	if (pfLowOld == NULL)
		pfLowOld = pfCloseOld;

	float *pfClose = new float[m_nNumPoints];
	float *pfHigh = new float[m_nNumPoints];
	float *pfLow = new float[m_nNumPoints];

	memcpy(pfClose, pfCloseOld, (sizeof (float)) * m_nNumPoints);
	memcpy(pfHigh, pfHighOld, (sizeof (float)) * m_nNumPoints);
	memcpy(pfLow, pfLowOld, (sizeof (float)) * m_nNumPoints);

	float *pfData[3] = {pfClose, pfHigh, pfLow};
	long* pnBadIndex = new long[m_nNumPoints];
	long nNumBadPoints;
	ScreenForMissing(pfData, 3, m_nNumPoints, pnBadIndex, &nNumBadPoints);

	BLPVector<float> pfPlusDMI(m_nNumPoints, HOLEF);
	BLPVector<float> pfMinusDMI(m_nNumPoints, HOLEF);
	BLPVector<float> pfADXLine(m_nNumPoints, HOLEF);
	BLPVector<float> pfADXRLine(m_nNumPoints, HOLEF);

// declare a few work arrays
	BLPVector<float> pfHighDMI(m_nNumPoints, HOLEF);
	BLPVector<float> pfLowDMI(m_nNumPoints, HOLEF);
	BLPVector<float> pfTrueSpread(m_nNumPoints, HOLEF);

	float fInversePeriod = 1.0f / m_nPeriod; // save few cpu cycles by multiplying by the inverse instead of dividing
	float fMultiplicationFactor = ((float)m_nPeriod - 1) * fInversePeriod;
	float fPlusSum = 0, fMinusSum = 0, fSpreadSum = 0, fAccumADX = 0;

	long nBar;
	for (nBar = 1; nBar < m_nNumPoints - nNumBadPoints; nBar++) {

		pfHighDMI[nBar] = pfHigh[nBar] - pfHigh[nBar - 1];
		if (pfHighDMI[nBar] < 0)
			pfHighDMI[nBar] = 0;
		pfLowDMI[nBar] = pfLow[nBar - 1] - pfLow[nBar];
		if (pfLowDMI[nBar] >= pfHighDMI[nBar]) {
			pfHighDMI[nBar] = 0;
			if (pfLowDMI[nBar] == pfHighDMI[nBar])
				pfLowDMI[nBar] = 0;
			}
		else 
			pfLowDMI[nBar] = 0;

		pfTrueSpread[nBar] = pfHigh[nBar] - pfLow[nBar];
		float fTestSpread = fabs(pfHigh[nBar] - pfClose[nBar - 1]);
		if (fTestSpread > pfTrueSpread[nBar])
			pfTrueSpread[nBar] = fTestSpread;

		fTestSpread = fabs(pfLow[nBar] - pfClose[nBar - 1]);
		if (fTestSpread > pfTrueSpread[nBar])
			pfTrueSpread[nBar] = fTestSpread;

		if (nBar <= m_nPeriod) {
			fPlusSum += pfHighDMI[nBar];
			fMinusSum += pfLowDMI[nBar];
			fSpreadSum += pfTrueSpread[nBar];

			if (nBar < m_nPeriod)
				continue;
		}
		else
		{
			fSpreadSum = fSpreadSum * fMultiplicationFactor + pfTrueSpread[nBar];	
			fPlusSum = fPlusSum * fMultiplicationFactor + pfHighDMI[nBar];	
			fMinusSum = fMinusSum * fMultiplicationFactor + pfLowDMI[nBar];	
		}

		pfPlusDMI[nBar] = (fPlusSum / fSpreadSum) * 100;
		pfMinusDMI[nBar] = (fMinusSum / fSpreadSum) * 100;
	
	    float fDelta = (100.0f * fabs(pfPlusDMI[nBar] - pfMinusDMI[nBar])  / (pfPlusDMI[nBar] + pfMinusDMI[nBar]));

		if (nBar < m_nPeriod * 2 - 1) {
			fAccumADX += fDelta;
			continue;
		}
		else if (nBar == m_nPeriod * 2 - 1) {
			fAccumADX += fDelta;
			fAccumADX *= fInversePeriod;
		}
		else
		{		
			fAccumADX = (fAccumADX * (m_nPeriod - 1) + fDelta) * fInversePeriod;
		}

		pfADXLine[nBar] = fAccumADX;

		if (nBar > m_nPeriod * 3)
			pfADXRLine[nBar] = (pfADXLine[nBar] + pfADXLine[nBar - m_nPeriod]) * 0.5f;
		}
	
	MergeGoodValuesWithMissing(&pfPlusDMI[0], m_nNumPoints, pnBadIndex, nNumBadPoints);
	MergeGoodValuesWithMissing(&pfMinusDMI[0], m_nNumPoints, pnBadIndex, nNumBadPoints);
	MergeGoodValuesWithMissing(&pfADXRLine[0], m_nNumPoints, pnBadIndex, nNumBadPoints);
	MergeGoodValuesWithMissing(&pfADXLine[0], m_nNumPoints, pnBadIndex, nNumBadPoints);

	CComPtr<IStudyData> iSD;
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;
	iSD->put_StudyDataLine(0, m_nNumPoints, &pfPlusDMI[0]);
	iSD->put_StudyDataLine(1, m_nNumPoints, &pfMinusDMI[0]);
	short nLineNum = 2;
	if (m_bShowADX)
		iSD->put_StudyDataLine(nLineNum++, m_nNumPoints, &pfADXLine[0]);
	else if (m_bShowADXR)
		iSD->put_StudyDataLine(nLineNum++, m_nNumPoints, &pfADXRLine[0]);

	if (m_bShowADXR)
		iSD->put_StudyDataLine(nLineNum++, m_nNumPoints, &pfADXRLine[0]);

	delete[] pfClose;
	delete[] pfHigh;
	delete[] pfLow;

	return S_OK;
}

HRESULT CParabolicTimePrice::ConvertBuf(void *pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;

	long *ptemp = (long *)&(pHead->accel_factor);
	*ptemp = ntohl(*ptemp);
	m_fAccelerationFactor = pHead->accel_factor;

	ptemp = (long *)&(pHead->start);
	*ptemp = ntohl(*ptemp);
	m_fStart = pHead->start;

	ptemp = (long *)&(pHead->max);
	*ptemp = ntohl(*ptemp);
	m_fMax = pHead->max;

	ConvertLineParameters(pHead->line_params, 1);
	m_pLineParameters = pHead->line_params;
	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 1);
		pHead->line_params[0].line_color = pHead->point_params[0].point_color;
		m_pPointParameters = pHead->point_params;
		}
	else
		m_pPointParameters = NULL;

	return S_OK;
}
HRESULT CParabolicTimePrice::DoStudy(IBLPGraphData *pGraph)
{
	BOOL weAreLong = TRUE;
	BOOL newEPtoday;
	pGraph->get_NumPoints(&m_nNumPoints);

	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		return E_INVALIDARG;

	BLPVector<float> pfPTPSLine(m_nNumPoints, HOLEF);
	float AF = m_fStart;

	float EP;
	float todaysSAR;
	float tomorrowsSAR;
	float dailyHigh;
	float dailyLow;
	float yesterdaysHigh;
	float yesterdaysLow;
	float twoDayHigh;
	float twoDayLow;

	BOOL bFirstBar = TRUE;
	long nBar;
	for (nBar = 0; (pfHigh[nBar] == HOLEF) || (pfLow[nBar] == HOLEF); nBar++) {
		if (nBar == m_nNumPoints)
			return S_OK;
		}

	bFirstBar = FALSE;
	dailyLow  = pfLow[nBar];
	dailyHigh = pfHigh[nBar];
	tomorrowsSAR = dailyLow;
	EP = tomorrowsSAR;

	for (nBar++; nBar < m_nNumPoints; nBar++) {
		if ((pfHigh[nBar] == HOLEF) || (pfLow[nBar] == HOLEF))
			continue;

		todaysSAR = tomorrowsSAR;
		yesterdaysHigh = dailyHigh;
		yesterdaysLow = dailyLow;
		dailyLow  = pfLow[nBar];
		dailyHigh = pfHigh[nBar];
		newEPtoday = FALSE;

    if (yesterdaysLow < dailyLow) {
      twoDayLow = yesterdaysLow;
    } else {
      twoDayLow = dailyLow;
    }
    if (yesterdaysHigh > dailyHigh) {
      twoDayHigh = yesterdaysHigh;
    } else {
      twoDayHigh = dailyHigh;
    }
    
    
    if (weAreLong) 
    {
      if (dailyHigh > EP) 
      {
        EP = dailyHigh;
        newEPtoday = TRUE;
      } 
      
      if ((newEPtoday)&&((AF + m_fAccelerationFactor) <= m_fMax)) 
      {
        AF = AF + m_fAccelerationFactor;
      }
      
      if (todaysSAR >= dailyLow) 
      {
        // Reverse position now
        weAreLong = FALSE;
        AF = m_fStart;
        todaysSAR = EP;
        EP = dailyLow;
        tomorrowsSAR = todaysSAR - AF * (todaysSAR - EP) ;
        
        // special case : adjust SAR if it is about to
        // penetrate the two day high
        if (tomorrowsSAR < twoDayHigh) 
        {
          tomorrowsSAR = twoDayHigh;
        }
      } 
      else 
      {
        // continue current position
        tomorrowsSAR = todaysSAR + AF * (EP - todaysSAR) ;
        
        if (tomorrowsSAR > twoDayLow) 
        {
          tomorrowsSAR = twoDayLow;
        }
      } 
    } 
    else 
    {
      // we are short
      if (dailyLow < EP) 
      {
        EP = dailyLow;
        newEPtoday = TRUE;
      } 
      
      if ((newEPtoday)&&((AF + m_fAccelerationFactor) <= m_fMax)) 
      {
        AF = AF + m_fAccelerationFactor;
      }
      
      if (todaysSAR <= dailyHigh)
      {
        // reverse position now
        weAreLong = TRUE;
        AF = m_fStart;
        todaysSAR = EP;
        EP = dailyHigh;
        tomorrowsSAR = todaysSAR + AF * (EP - todaysSAR) ;
        
        // special case : adjust SAR if it is about to
        // penetrate the two day low
        if (tomorrowsSAR > twoDayLow) 
        {
          tomorrowsSAR = twoDayLow;
        }
      } 
      else 
      {
        // continue with current position
        tomorrowsSAR = todaysSAR - AF * (todaysSAR - EP) ;
        
        // special case : adjust SAR if it is about to
        // penetrae the two day high
        if (tomorrowsSAR < twoDayHigh) 
        {
          tomorrowsSAR = twoDayHigh;
        }
      } 
    }

    pfPTPSLine[nBar] = todaysSAR;
	}

	SetStudyDataLine(0, &pfPTPSLine[0]);
	return S_OK;
}

HRESULT CParabolicTimePrice::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartPointParameters pp = {0};
	if (m_pPointParameters == NULL)
		m_pPointParameters = &pp;

	HRESULT rc;
	rc = AddStudy(pGraph, 1, 1, m_pLineParameters, m_pPointParameters);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "PTPS", this);
		}

	return rc;
}

HRESULT CMAvgConvDiv::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod1 = ntohs(pHead->period1);
	m_nPeriod2 = ntohs(pHead->period2);
	m_nSignalPeriod = ntohs(pHead->period3);
	m_bShowHistogram = pHead->show_macd_histogram;
	m_bShowMACD2 = pHead->show_macd2;

	ConvertLineParameters(pHead->line_params, 3);
	memcpy(m_LineParams, pHead->line_params, 3 * sizeof(ChartLineParameters));

	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 3);
		memcpy(m_PointParams, pHead->point_params, 3 * sizeof(ChartPointParameters));

		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	strncpy(m_zCalcRouteId, pHead->field_id, FIELD_LEN);
	return S_OK;
}

HRESULT CMAvgConvDiv::CreateStudy(IBLPGraphStudyList * pGraph)
{
	HRESULT rc;
	m_nBaseId = INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW;

	CComQIPtr<IBLPGraphData, &IID_IBLPGraphData> iGraphData(pGraph);
	iGraphData->get_NumPoints(&m_nNumPoints);
	rc = iGraphData->get_CalcRouteData(m_zCalcRouteId, m_nNumPoints, &m_pfDataIn);
	if (FAILED(rc))
		return rc;

	if (m_bShowHistogram) {
		strcpy(m_zCalcRouteId, "STUDY2"); // allows MACD histogram style, using the 2nd study data line
		m_LineParams[2].line_style = XRT_LPAT_NONE;
		}

	rc = AddStudy(pGraph, 3, m_bShowMACD2 ? 3 : 2, m_LineParams, m_PointParams);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
        if (m_bShowHistogram) {
			iChart->put_ChartStyle(CHART_STYLE_BAR);
			iChart->put_BarColor(m_LineParams[2].line_color);
			}
		iGraphToolbar->AddStudyButton(0, "MACD", this);
		}
	return rc;
}

HRESULT CMAvgConvDiv::DoStudy(IBLPGraphData *pGraph)
{
	if (m_nPeriod1 < 1)
		m_nPeriod1 = 1;

	if (m_nPeriod2 < 1)
		m_nPeriod2 = 1;

	if (m_nSignalPeriod < 1)
		m_nSignalPeriod = 1;

	// Copy of MACD constant setup from core graph
	float exp1 = 2.0f / ( (float)m_nPeriod1 + 1.0f );	// exp percentage constant for current point, period1
	float exp2 = 2.0f / ( (float)m_nPeriod2 + 1.0f );	// exp percentage constant for current point, period2
	float exp3 = 2.0f / ( (float)m_nSignalPeriod + 1.0f );	// exp percentage constant for current point, period3

	float exq1 = 1.0f - exp1;						// exp percentage constant for previous point, period1 
	float exq2 = 1.0f - exp2;						// exp percentage constant for previous point, period2
	float exq3 = 1.0f - exp3;						// exp percentage constant for previous point, period3
  
	pGraph->get_NumPoints(&m_nNumPoints);
	BLPVector<float> pfMacd(m_nNumPoints, HOLEF);
	BLPVector<float> pfMacdSignal(m_nNumPoints, HOLEF);
	BLPVector<float> pfMacd2(m_nNumPoints, HOLEF);

	float fRunningAvg1 = m_pfDataIn[0],
		fRunningAvg2 = m_pfDataIn[0];

	pfMacd[0] = 0;
	pfMacdSignal[0] = 0;
	pfMacd2[0] = 0;

	long nBar;
	for (nBar = 1; nBar < m_nNumPoints; nBar++) {
		fRunningAvg1 = (m_pfDataIn[nBar] * exp1) + (fRunningAvg1 * exq1);
		fRunningAvg2 = (m_pfDataIn[nBar] * exp2) + (fRunningAvg2 * exq2);
		pfMacd[nBar] = fRunningAvg1 - fRunningAvg2;
		pfMacdSignal[nBar] = (pfMacd[nBar] * exp3) + (pfMacdSignal[nBar - 1] * exq3);
		pfMacd2[nBar] = pfMacd[nBar] - pfMacdSignal[nBar];
		}

	SetStudyDataLine(0, &pfMacd[0]);
	SetStudyDataLine(1, &pfMacdSignal[0]);
	SetStudyDataLine(2, &pfMacd2[0]);

	return S_OK;
}

#if 0
HRESULT CTAStochastics::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;

	m_nKPeriod = ntohs(pHead->period1);
	m_nDPeriod = ntohs(pHead->period2);
	m_nDSPeriod = ntohs(pHead->period3);	
	m_nDSSPeriod = ntohs(pHead->period4);
	m_bShowFast = pHead->show_tas_fast;
	m_bShowSlow = pHead->show_tas_slow;

	m_pLineParameters = pHead->line_params;
	ConvertLineParameters(m_pLineParameters, 4);

	m_pPointParameters = pHead->point_params;
	if (pHead->version >= 1) {
		ConvertPointParameters(m_pPointParameters, 4);
		ConvertChartColors(&pHead->chart_colors);
		m_crBackgroundFastchart = pHead->chart_colors.chart_background;
		ConvertChartColors(&pHead->second_chart_colors);
		m_crBackgroundSlowchart = pHead->second_chart_colors.chart_background;
		}

	ConvertOscillatorParameters(&pHead->zone);
	m_zones.overbought = pHead->zone.overbought;
	m_zones.oversold = pHead->zone.oversold;
	return S_OK;
}

HRESULT CTAStochastics::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pPercentK = new CChartLine;
	pPercentK->SetLineParams(m_pLineParameters[0]);
	pPercentK->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pPercentK);

	CChartLine *pPercentD = new CChartLine;
	pPercentD->SetLineParams(m_pLineParameters[1]);
	pPercentD->SetPointParams(m_pPointParameters[1]);
	m_vChartLine.push_back(pPercentD);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK)
		AddChart(iChart);

	iGraphToolbar->AddStudyButton(0, "TAS", this);
	return S_OK;
}

HRESULT CTAStochastics::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints);
	BLPVector<float> vfPercentK(m_nNumPoints, HOLEF);
	BLPVector<float> vfPercentD(m_nNumPoints, HOLEF);

	CComPtr<IStudyData> iSD;
	CComPtr<IChart> iChart = m_vCharts[0];
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	// Copy the study lines onto the Oscillator Chart
	iSD->put_StudyDataLine(0, m_nNumPoints, &vfPercentK[0]);
	iSD->put_StudyDataLine(1, m_nNumPoints, &vfPercentD[0]);

	return S_OK;
}
#endif

HRESULT CGOCChart::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nConversionPeriod = ntohs(pHead->period1);
	m_nBasePeriod = ntohs(pHead->period2);
	m_nLeadLagPeriod = ntohs(pHead->period3);

	ConvertLineParameters(pHead->line_params, 5);
	if (pHead->version >= 1)
		ConvertPointParameters(pHead->point_params, 5);

	m_pLineParameters = pHead->line_params;
	m_pPointParameters = pHead->point_params;
	return S_OK;
}

HRESULT CGOCChart::CreateStudy(IBLPGraphStudyList * pGraph)
{
	m_pPointParameters[3].point_style = XRT_POINT_HATCH_FILL;
	m_pPointParameters[3].point_color = blgBlue;
	HRESULT hr = AddStudy(pGraph, 5, 5, m_pLineParameters, m_pPointParameters);

	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "GOC", this);
		}

	return S_OK;
}

HRESULT CGOCChart::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumGraphDataPoints);

	// Extend study into the future by one day.
	m_nNumPoints = m_nNumGraphDataPoints + m_nLeadLagPeriod;
	long nLeadLagPeriod = m_nLeadLagPeriod * 2;


	HRESULT hr = pSD->put_NumStudyPoints(m_nNumPoints);
	pGraph->put_NumFuturePoints(m_nNumPoints);

	BLPVector<float> vfBase(m_nNumPoints, HOLEF);
	BLPVector<float> vfConversion(m_nNumPoints, HOLEF);
	BLPVector<float> vfLagging(m_nNumPoints, HOLEF);
	BLPVector<float> vfLeading1(m_nNumPoints, HOLEF);
	BLPVector<float> vfLeading2(m_nNumPoints, HOLEF);

	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		return E_INVALIDARG;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	if (pfClose == NULL)
		return E_INVALIDARG;

	// Changhai Xu DRQS 3336212
	// This function has been re-engineered to fix the problem that what is shown on the chart and 
	// what is shown on the table do not agree on conversion, base, leading1 and leading2 lines.
	// The change in this function is on taking care of situations where High, Low, and Close may 
	// be missing so that leaving holes in the input arrays. The change 
	// makes sure that in the caculation for conversion, base, lagging, leading1 and leading2 lines
	// it goes back/ahead specified number of valid periods. An alternative fix is to call 
	// ScreenForMissing on input High, Low, Close arrays to filter out the missings, then call 
	// MergeGoodWithMissing on Conversion, Base, Lagging, Leading1 and Leading2 after the calculation. 
	// This way is more conformal to the ways for handling other studies in this DLL. However, the 
	// DoStudy is called twice in GOC,which causes ScreenForMissing is called twice and results in the 
	// alterative fix does not work. 

	long nCurrentBar, count;
	for (nCurrentBar = 0; nCurrentBar < m_nNumGraphDataPoints; nCurrentBar++) {
		long nBar;
		
		/* Conversion Line */
		CBLPPrice fMaxHighConversion, fMinLowConversion;
	
		// go back "CoversionPeriod" number of valid periods
		for (count = 0, nBar = nCurrentBar; count < m_nConversionPeriod; nBar--) {
			if (nBar < 0)
				break;

			if( IsInvalid(pfHigh[nBar], pfHigh[nBar], pfLow[nBar], pfClose[nBar]) )
				continue;
			count++;
			if (!(fMaxHighConversion > pfHigh[nBar]))
				fMaxHighConversion = pfHigh[nBar];

			if (!(fMinLowConversion < pfLow[nBar]))
				fMinLowConversion = pfLow[nBar];
			}
		
		if ( (count < m_nConversionPeriod) || 
			 IsInvalid(pfHigh[nCurrentBar], pfHigh[nCurrentBar], pfLow[nCurrentBar], pfClose[nCurrentBar]) 
			 || (fMaxHighConversion == HOLEF) || (fMinLowConversion == HOLEF) ) 
			vfConversion[nCurrentBar] = HOLEF;
		else 
			vfConversion[nCurrentBar] = 0.5 * (fMaxHighConversion + fMinLowConversion);

		
		/* Base Line */
		CBLPPrice fMaxHighBase, fMinLowBase;
		// go back "BasePeriod" number of valid periods
		for (count = 0, nBar = nCurrentBar; count < m_nBasePeriod; nBar--) {
			if (nBar < 0)
				break;

			if( IsInvalid(pfHigh[nBar], pfHigh[nBar], pfLow[nBar], pfClose[nBar]) )
				continue;
			count++;

			if (!(fMaxHighBase > pfHigh[nBar]))
				fMaxHighBase = pfHigh[nBar];

			if (!(fMinLowBase < pfLow[nBar]))
				fMinLowBase = pfLow[nBar];
		}
		
		if ( (count < m_nBasePeriod) || 
			IsInvalid(pfHigh[nCurrentBar], pfHigh[nCurrentBar], pfLow[nCurrentBar], pfClose[nCurrentBar]) 
			|| (fMaxHighBase == HOLEF) || (fMinLowBase == HOLEF) ) 
			vfBase[nCurrentBar] = HOLEF;
		else 
			vfBase[nCurrentBar] = 0.5 * (fMaxHighBase + fMinLowBase);


		/* Leading and Lagging Span */
		CBLPPrice fMaxHighLeadLag, fMinLowLeadLag;
		// go back "2*LeadLagPeriod" number of valid periods
		for (count = 0, nBar = nCurrentBar; count < nLeadLagPeriod; nBar--) {
			if (nBar < 0)
				break;
			if( IsInvalid(pfHigh[nBar], pfHigh[nBar], pfLow[nBar], pfClose[nBar]) )
				continue;
			count++;
			if (!(fMaxHighLeadLag > pfHigh[nBar]))
				fMaxHighLeadLag = pfHigh[nBar];

			if (!(fMinLowLeadLag < pfLow[nBar]))
				fMinLowLeadLag = pfLow[nBar];
		}

		long countLeadLag=0;
		// look ahead "LeadLagPeriod" number of valid periods
		for(countLeadLag = 0, nBar = nCurrentBar; nBar < m_nNumGraphDataPoints && countLeadLag < m_nLeadLagPeriod; nBar++) {
			if( IsInvalid(pfHigh[nBar], pfHigh[nBar], pfLow[nBar], pfClose[nBar]) )
				continue;
			countLeadLag++;
		}

		// set today's Lagging Span as the close of"LeadLagPeriod" number of valid periods ahead
		if ( countLeadLag < m_nLeadLagPeriod || 
			IsInvalid(pfHigh[nCurrentBar], pfHigh[nCurrentBar], pfLow[nCurrentBar], pfClose[nCurrentBar]) )
		     vfLagging[nCurrentBar] = HOLEF;
		else 
			vfLagging[nCurrentBar] = pfClose[nBar-1];

		// if it projects the future that is out of user's input date range, 
		// we don't need to consider the future day is valid or not.
		if(nCurrentBar+m_nLeadLagPeriod > m_nNumGraphDataPoints)
			nBar = nCurrentBar+m_nLeadLagPeriod;
 
		// set the Leading Span1 of"LeadLagPeriod" number of valid periods ahead
		if ( (vfBase[nCurrentBar] == HOLEF) || (vfConversion[nCurrentBar] == HOLEF) ) 
			vfLeading1[nBar - 1] = HOLEF; 
		else 
			vfLeading1[nBar - 1] = 0.5F * (vfBase[nCurrentBar] + vfConversion[nCurrentBar]);

		// set the Leading Span2 of"LeadLagPeriod" number of valid periods ahead
		if ( (count < nLeadLagPeriod) || IsInvalid(pfHigh[nCurrentBar], pfHigh[nCurrentBar], pfLow[nCurrentBar], pfClose[nCurrentBar])|| 
			 (fMaxHighLeadLag == HOLEF) || (fMinLowLeadLag == HOLEF) ) 
			vfLeading2[nBar - 1] = HOLEF;
		else 
			vfLeading2[nBar - 1] = 0.5 * (fMaxHighLeadLag + fMinLowLeadLag);
	}

	SetStudyDataLine(0, &vfConversion[0]);
	SetStudyDataLine(1, &vfBase[0]);
	SetStudyDataLine(2, &vfLagging[0]);
	SetStudyDataLine(3, &vfLeading1[0]);
	SetStudyDataLine(4, &vfLeading2[0]);

	return S_OK;
}

HRESULT CRateOfChange::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;

	m_nPeriod = ntohs(pHead->period);
	m_bModeIsPercent = pHead->mode;

	m_pLineParameters = pHead->line_params;
	ConvertLineParameters(m_pLineParameters, 1);

	m_pPointParameters = pHead->point_params;
	ConvertPointParameters(m_pPointParameters, 1);

	ConvertChartColors(&pHead->chart_colors);
	m_crChartBackground = pHead->chart_colors.chart_background;

	strncpy(m_zCalcRouteId, pHead->field_id, 8);
	return S_OK;
}

HRESULT CRateOfChange::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints);

	BLPVector<float> vfRateOfChange(m_nNumPoints, HOLEF);
	float *pfDataValues = GetStudyCalcRoute(pGraph, m_zCalcRouteId);
	float fAnnualiseFactor = 1; //360.0 / m_nPeriod;
	long nBar;
	for (nBar = m_nPeriod; nBar < m_nNumPoints; nBar++) {
		if ( (pfDataValues[nBar] == HOLEF) ||
		  (pfDataValues[nBar - m_nPeriod] == HOLEF) )
			continue;

		vfRateOfChange[nBar] = fAnnualiseFactor * (pfDataValues[nBar] - pfDataValues[nBar - m_nPeriod]);
		if (m_bModeIsPercent)
			if ( fabs(pfDataValues[nBar - m_nPeriod]) < DELTA)
				vfRateOfChange[nBar] = HOLEF;
			else
				vfRateOfChange[nBar] *= 100.0 / pfDataValues[nBar - m_nPeriod];
		}

	CComPtr<IStudyData> iSD;
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	// Copy the study lines onto the Oscillator Chart
	iSD->put_StudyDataLine(0, m_nNumPoints, &vfRateOfChange[0]);
	return S_OK;
}

HRESULT CRateOfChange::CreateStudy(IBLPGraphStudyList * pGraph)
{
	HRESULT rc = pGraph->AddStudy(m_nInstanceId, 0, m_zCalcRouteId, 1, 0, m_pLineParameters, 
						m_pPointParameters, &pSD, this);
    //If there is no data for this security, it will fail at this point
    if (FAILED(rc))
        return rc;

	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(m_pLineParameters[0]);
	pChartLine->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pChartLine);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "ROC", this);
		}

	return S_OK;
}

HRESULT CVolumeChart::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_crHistogramBar = ntohl(pHead->color);
	if (pHead->version >= 1) {
		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	m_zCalcRouteId[0] = '\0';
	if (pHead->version > 2) {
		pHead->VolumeId[5] = '\0';
		strcpy(m_zCalcRouteId, pHead->VolumeId);
		}

	if (m_zCalcRouteId[0] == '\0')
		strcpy(m_zCalcRouteId, "PR013");
	return S_OK;
}

HRESULT CVolumeChart::DoStudy(IBLPGraphData *pGraph)
{
	return S_OK;
}

HRESULT CVolumeChart::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartLineParameters LineParms[1] = {0, 0, 0};
	ChartPointParameters PointParms[1] = {0, 0, 0};
	m_nBaseId = 0;
	m_nBaseId = VOLUME_CHART;
	HRESULT hr = AddStudy(pGraph, 0, 0, LineParms, PointParms);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	ASSERT(SUCCEEDED(hr));
	if (SUCCEEDED(hr) && ((hr = pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart)) == S_OK) ){
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iChart->put_ChartStyle(CHART_STYLE_BAR);
		iGraphToolbar->AddStudyButton(0, "Volume", this);
		}

 	return hr;
}

HRESULT CMavg::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	
	m_nPeriod = ntohs(pHead->period);
	m_strCalcRtId = pHead->field_id;

	m_pLineParameters = pHead->line_params;
	ConvertLineParameters(m_pLineParameters, 1);

	m_pPointParameters = pHead->point_params;
	ConvertPointParameters(m_pPointParameters, 1);
	return S_OK;
}

HRESULT CExponentialMavg::DoStudy(IBLPGraphData *pGraph)
{
	if (m_nPeriod < 2)
		return S_OK;
    /* DRQS 3561715. Preventive check. If pGraph is NULL this is suppose to return preventing
	 * a crash
	 */
	if(pGraph == NULL)
		return E_HANDLE;
	
	pGraph->get_NumPoints(&m_nNumPoints);

	BLPVector<float> vfSMavg(m_nNumPoints, HOLEF);
	float *pfSMavg = &vfSMavg[0];
	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	pGraph->ExponentialAverage(m_nPeriod, m_nNumPoints, pfData, &pfSMavg);

	SetStudyDataLine(0, pfSMavg);
	return S_OK;
}

HRESULT CSimpleMavg::DoStudy(IBLPGraphData *pGraph)
{

	/* DRQS 3561715. Preventive check. If pGraph is NULL this is suppose to return preventing
	 * a crash
	 */
	if(pGraph == NULL)
		return E_HANDLE;
	
	pGraph->get_NumPoints(&m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfSMavg(m_nNumPoints, HOLEF);
	float *pfSMavg = &vfSMavg[0];
	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	pGraph->MovingAverage(m_nPeriod, m_nNumPoints, pfData, &pfSMavg);

	SetStudyDataLine(0, pfSMavg);
	return S_OK;
}

HRESULT CWeightedMavg::DoStudy(IBLPGraphData *pGraph)
{
	/* DRQS 3561715. Preventive check. If pGraph is NULL this is suppose to return preventing
	 * a crash
	 */

	if(pGraph == NULL)
		return E_HANDLE;

	pGraph->get_NumPoints(&m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	if (m_nPeriod <= 0)
		return E_HANDLE;

	BLPVector<float> vfWeightedMavg(m_nNumPoints, HOLEF);
	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	long nCurrentBar; 
	for (nCurrentBar = m_nPeriod - 1; nCurrentBar < m_nNumPoints; nCurrentBar++) {
		short nBarsBack;
		if (pfData[nCurrentBar] == HOLEF)
			continue;
		vfWeightedMavg[nCurrentBar] = 0.0;
		float fWeightedDataSum = 0.0;
		float fWeightSum = 0.0;
		for (nBarsBack = 0; nBarsBack < m_nPeriod; nBarsBack++) {
			float fDataPoint = pfData[nCurrentBar + nBarsBack + 1 - m_nPeriod];
			if (fDataPoint != HOLEF) {
				fWeightedDataSum +=  fDataPoint * m_vfWeights[nBarsBack];
				fWeightSum += m_vfWeights[nBarsBack];
				}
			}

		vfWeightedMavg[nCurrentBar] = fWeightedDataSum / fWeightSum;
		}

	SetStudyDataLine(0, &vfWeightedMavg[0]);
	return S_OK;
}

HRESULT CMavg::CreateStudy(IBLPGraphStudyList * pGraph)
{
	HRESULT rc;

	rc = AddStudy(pGraph, 1, 1, m_pLineParameters, m_pPointParameters);
	ASSERT(SUCCEEDED(rc));
	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, m_strButtonText, this);
		}
	return S_OK;
}

HRESULT CTriangularMavg::CreateStudy(IBLPGraphStudyList * pGraph)
{
	m_vfWeights.resize(m_nPeriod);
	short i1, i2;
	for (i1 = 0, i2 = m_nPeriod - 1; i1 <= i2; i1++, i2--) {
		m_vfWeights[i1] = m_vfWeights[i2] = i1 + 1;
		}
	return CMavg::CreateStudy(pGraph);
}

HRESULT CWeightedMavg::CreateStudy(IBLPGraphStudyList * pGraph)
{
	m_vfWeights.resize(m_nPeriod);
	short i;
	for (i = 0; i < m_nPeriod; i++) {
		m_vfWeights[i] = m_vfWeights[i] = i + 1;
		}
	return CMavg::CreateStudy(pGraph);
}

HRESULT CVariableMavg::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints);
	if (m_nPeriod + 12 > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfVariableMavg(m_nNumPoints, HOLEF);
	float *pfData = GetStudyCalcRoute(pGraph, m_strCalcRtId);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;

	vfVariableMavg[m_nPeriod + 12] = pfData[m_nPeriod + 12];
	long nCurrentBar; 
	for (nCurrentBar = m_nPeriod + 13; nCurrentBar < m_nNumPoints; nCurrentBar++) {
		float fPeriodHigh = pfData[nCurrentBar - 1];
		float fPeriodLow = pfData[nCurrentBar - 1];
		float fDiffSum = 0.0;
		short nBarsBack;
		for (nBarsBack = 1; nBarsBack <= m_nPeriod; nBarsBack++) {
			fDiffSum += fabs(pfData[nCurrentBar - nBarsBack] - pfData[nCurrentBar - nBarsBack - 1]);
			if ((CBLPPrice)pfData[nCurrentBar - nBarsBack] > fPeriodHigh)
				fPeriodHigh = pfData[nCurrentBar - nBarsBack];
			if ((CBLPPrice)pfData[nCurrentBar - nBarsBack] < fPeriodLow)
				fPeriodLow = pfData[nCurrentBar - nBarsBack];
			}
		
		float fDiffRatio = (fDiffSum == 0) ? 1.0 : (fPeriodHigh - fPeriodLow) / fDiffSum;

		long nBar12Back = nCurrentBar - 12;
		fPeriodHigh = pfData[nBar12Back - 1];
		fPeriodLow = pfData[nBar12Back - 1];
		fDiffSum = 0.0;
		for (nBarsBack = 1; nBarsBack <= m_nPeriod; nBarsBack++) {
			fDiffSum += fabs(pfData[nBar12Back - nBarsBack] - pfData[nBar12Back - nBarsBack - 1]);
			if ((CBLPPrice)pfData[nBar12Back - nBarsBack] > fPeriodHigh)
				fPeriodHigh = pfData[nBar12Back - nBarsBack];
			if ((CBLPPrice)pfData[nBar12Back - nBarsBack] < fPeriodLow)
				fPeriodLow = pfData[nBar12Back - nBarsBack];
			}

		float fPeriodHighLowDiff = fPeriodHigh - fPeriodLow;
		
		if(fabs(fPeriodHighLowDiff) < DELTA)
		{
			vfVariableMavg[nCurrentBar] = vfVariableMavg[nCurrentBar - 1];
		}
		else
		{
			float fDiffRatio12 = (fDiffSum == 0) ? 1.0 : (fPeriodHigh - fPeriodLow) / fDiffSum;
			if (fDiffRatio12 != 0)
			{
				float fFactor = 0.078 * (fDiffRatio / fDiffRatio12);

				vfVariableMavg[nCurrentBar] = fFactor * pfData[nCurrentBar] + (1 - fFactor) * vfVariableMavg[nCurrentBar - 1];
			}
		}
	}

	SetStudyDataLine(0, &vfVariableMavg[0]);
	return S_OK;
}

HRESULT CRelativeStrengthIndex::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod = ntohs(pHead->period);
	strncpy(m_zCalcRouteId, pHead->field_id, FIELD_LEN);
	m_zCalcRouteId[FIELD_LEN] = '\0';

	ConvertOscillatorParameters(&pHead->zone);
	if (pHead->zone.overbought != 0)
		m_zones.overbought = pHead->zone.overbought;
	if (pHead->zone.oversold != 0)
		m_zones.oversold = pHead->zone.oversold;

	if (pHead->version >= 1) {
		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	m_pLineParameters = pHead->line_params;
	ConvertLineParameters(m_pLineParameters, 1);

	m_pPointParameters = pHead->point_params;
	ConvertPointParameters(m_pPointParameters, 1);
	return S_OK;
}

HRESULT CRelativeStrengthIndex::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(m_pLineParameters[0]);
	pChartLine->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pChartLine);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iGraphToolbar->AddStudyButton(0, "RSI", this);
		}

	return S_OK;
}

HRESULT CRelativeStrengthIndex::DoStudy(IBLPGraphData *pGraph)
{
	/* DRQS 3561715. Preventive check. If pGraph is NULL this is suppose to return preventing
	 * a crash
	 */

	if(pGraph == NULL)
		return E_HANDLE;

	pGraph->get_NumPoints(&m_nNumPoints);	

	if (m_nNumPoints < m_nPeriod)
		return S_OK;

	float *pfData = GetStudyCalcRoute(pGraph, m_zCalcRouteId);

	/* DRQS 3517468 
	 * This check is to fix the problem with WINTRV crashing when an RSI study is added 
	 * over a data which is not supported by a security but G allowed to add data to it
	 */
	if(pfData==NULL)
		return E_HANDLE;

	BLPVector<float> vfRelativeStrengthIndex(m_nNumPoints, HOLEF);

	long nBar, nStartBar; 
	float fAccumUp = 0, fAccumDown = 0;


	for (nStartBar = 1; nStartBar < m_nNumPoints; nStartBar++) {
		BOOL bNoHoles = TRUE;
		if (pfData[nStartBar - 1] == HOLEF) 
			continue;
		else {
			short nLookAhead;
			for (nLookAhead = 0; nLookAhead < m_nPeriod && nLookAhead + nStartBar < m_nNumPoints;  nLookAhead++)  
				if (pfData[nStartBar + nLookAhead] == HOLEF) {
					bNoHoles = FALSE;
					break;
					}
			}
		if (bNoHoles)
			break;
		}


	int nEnd = min(nStartBar + m_nPeriod, m_nNumPoints);

	for (nBar = nStartBar; nBar < nEnd; nBar++) {
		float fDiff = pfData[nBar] - pfData[nBar - 1];
		if (fDiff > 0)
			fAccumUp += fDiff;
		else
			fAccumDown -= fDiff;
		}

	fAccumUp = fAccumUp / m_nPeriod;
	fAccumDown = fAccumDown / m_nPeriod;

	if ((fAccumDown == 0) || (fAccumUp == 0)) 
		vfRelativeStrengthIndex[nBar - 1] = HOLEF;
	else {
		float fRatio = fAccumUp / fAccumDown;
		vfRelativeStrengthIndex[nBar - 1] = 100.0f - (100.0f / (1.0f + fRatio));;
		}

	for (; nBar < m_nNumPoints; nBar++) {
		if ((pfData[nBar] == HOLEF) || (pfData[nBar - 1] == HOLEF))
				continue;

		float fDiff = pfData[nBar] - pfData[nBar - 1];
		if (fDiff > 0) {
			fAccumUp = (fAccumUp * (m_nPeriod - 1) + fDiff) / m_nPeriod;
			fAccumDown = fAccumDown * (m_nPeriod - 1) / m_nPeriod;
			}
		else {
			fAccumUp = fAccumUp * (m_nPeriod - 1) / m_nPeriod;
			fAccumDown = (fAccumDown * (m_nPeriod - 1) - fDiff) / m_nPeriod;
			}

		if ((fAccumDown == 0) || (fAccumUp == 0)) 
			vfRelativeStrengthIndex[nBar] = HOLEF;
		else {
			float fRatio = fAccumUp / fAccumDown;
			vfRelativeStrengthIndex[nBar] = 100.0f - (100.0f / (1.0f + fRatio));;
			}
		}

//	SetStudyDataLine(0, vfRelativeStrengthIndex.begin());
	CComPtr<IStudyData> iSD;
	HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;
	iSD->put_StudyDataLine(0, m_nNumPoints, &vfRelativeStrengthIndex[0]);
	return S_OK;
}

HRESULT CCMCI::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod = ntohs(pHead->period);
	if (m_nPeriod < 3)
		m_nPeriod = 3;

	ConvertLineParameters(pHead->line_params, 1);
	m_pLineParameters = pHead->line_params;
	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 1);
		m_pPointParameters = pHead->point_params;
		}
	else
		m_pPointParameters = NULL;

	ConvertOscillatorParameters(&pHead->zone);
	m_zones.overbought = pHead->zone.overbought;
	m_zones.oversold = pHead->zone.oversold;
    
	if (pHead->version >= 1) {
		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	return S_OK;
}

HRESULT CCMCI::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(m_pLineParameters[0]);
	pChartLine->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pChartLine);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iGraphToolbar->AddStudyButton(0, "CMCI", this);
		}

	return S_OK;
}

HRESULT CCMCI::DoStudy(IBLPGraphData *pGraph)
{
	long nBar;

	pGraph->get_NumPoints(&m_nNumPoints);
//	ASSERT(m_nPeriod <= m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	float fInversePeriod = 1.0f / m_nPeriod;

	BLPVector<float> vfCMCILine(m_nNumPoints, HOLEF);
	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		return E_INVALIDARG;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	if (pfClose == NULL)
		return E_INVALIDARG;


	for (nBar = m_nPeriod - 1; nBar < m_nNumPoints; nBar++) {
		long nBarsBack;
		BLPVector<float> vfCMCITemp(m_nPeriod, 0);
		float fMavg = 0;
		for (nBarsBack = nBar - m_nPeriod + 1; nBarsBack <= nBar; nBarsBack++) {
			short nNumItems = 0;
			if ((pfHigh[nBarsBack] != HOLEF) && (pfHigh[nBarsBack] != INTERPOLATEF)) {
				vfCMCITemp[nBar - nBarsBack] += pfHigh[nBarsBack];
				nNumItems++;
				}

			if ((pfLow[nBarsBack] != HOLEF) && (pfLow[nBarsBack] != INTERPOLATEF)) {
				vfCMCITemp[nBar - nBarsBack] += pfLow[nBarsBack];
				nNumItems++;
				}

			if ((pfClose[nBarsBack] != HOLEF) && (pfClose[nBarsBack] != INTERPOLATEF)) {
				vfCMCITemp[nBar - nBarsBack] += pfClose[nBarsBack];
				nNumItems++;
				}

			if (nNumItems > 0)
				vfCMCITemp[nBar - nBarsBack] /= (float)nNumItems;
			else {
				break;
				}
			fMavg += vfCMCITemp[nBar - nBarsBack];
			}

		if (nBar >= nBarsBack)
			continue;

		fMavg *= fInversePeriod;
		float fMeanDeviation = 0;
		for (nBarsBack = nBar - m_nPeriod + 1; nBarsBack <= nBar; nBarsBack++) {
			fMeanDeviation += (float)fabs((double)(vfCMCITemp[nBar - nBarsBack] - fMavg));
			}

		fMeanDeviation *= fInversePeriod;
		if (fMeanDeviation != 0)
			vfCMCILine[nBar] = (vfCMCITemp[0] - fMavg) / (fMeanDeviation  * 0.015f);
		else
			vfCMCILine[nBar] = 0;
		}

	SetStudyDataLine(0, &vfCMCILine[0]);
	return S_OK;
}

HRESULT CWilliam::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod = ntohs(pHead->period);
	if (m_nPeriod < 3)
		m_nPeriod = 3;

	ConvertLineParameters(pHead->line_params, 1);
	m_pLineParameters = pHead->line_params;
	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 1);
		m_pPointParameters = pHead->point_params;
		}
	else
		m_pPointParameters = NULL;

	ConvertOscillatorParameters(&pHead->zone);
    
 	if (pHead->zone.overbought != 0)
		m_zones.overbought = pHead->zone.overbought;
	if (pHead->zone.oversold != 0)
		m_zones.oversold = pHead->zone.oversold;

	if (pHead->version >= 1) {
		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	return S_OK;
}

HRESULT CWilliam::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(m_pLineParameters[0]);
	pChartLine->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pChartLine);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iGraphToolbar->AddStudyButton(0, "WLPR", this);
		}

	return S_OK;
}

HRESULT CWilliam::DoStudy(IBLPGraphData *pGraph)
{
	long nBar;

	pGraph->get_NumPoints(&m_nNumPoints);
//	ASSERT(m_nPeriod <= m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfWilliamLine(m_nNumPoints, HOLEF);
	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		return E_INVALIDARG;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	if (pfClose == NULL)
		return E_INVALIDARG;


	for (nBar = m_nPeriod - 1; nBar < m_nNumPoints; nBar++) {
		if ((pfClose[nBar] == HOLEF) || (pfClose[nBar] == INTERPOLATEF))
			continue;

		short nBarsBack;
		float fHigh = pfHigh[nBar];
		float fLow = pfLow[nBar];
		for (nBarsBack = 1; nBarsBack < m_nPeriod; nBarsBack++) {
			if ((CBLPPrice) pfHigh[nBar - nBarsBack] > fHigh)
				fHigh = pfHigh[nBar - nBarsBack];

			if ((CBLPPrice) pfLow[nBar - nBarsBack] < fLow)
				fLow = pfLow[nBar - nBarsBack];
			}

		if (fHigh != fLow)
			vfWilliamLine[nBar] = ((pfClose[nBar] - fHigh) / (fHigh - fLow)) * 100.0f;
		}

	SetStudyDataLine(0, &vfWilliamLine[0]);
	return S_OK;
}

HRESULT CHurst::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
	m_nPeriod = ntohs(pHead->period);

    strncpy(m_zCalcRouteId, pHead->field_id, FIELD_LEN );
	ConvertLineParameters(pHead->line_params, 1);
	m_pLineParameters = pHead->line_params;
	if (pHead->version >= 1) {
		ConvertPointParameters(pHead->point_params, 1);
		m_pPointParameters = pHead->point_params;
		}
	else
		m_pPointParameters = NULL;
   
 	if (pHead->version >= 1) {
		ConvertChartColors(&pHead->chart_colors);
		m_crBackground = pHead->chart_colors.chart_background;
		}

	return S_OK;
}

HRESULT CHurst::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pChartLine = new CChartLine;
	pChartLine->SetLineParams(m_pLineParameters[0]);
	pChartLine->SetPointParams(m_pPointParameters[0]);
	m_vChartLine.push_back(pChartLine);

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	CComPtr<IChart> iChart;
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
		AddChart(iChart);
		iChart->put_BackgroundColor(m_crBackground);
		iGraphToolbar->AddStudyButton(0, "Hurst", this);
		}

	return S_OK;
}

HRESULT CHurst::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints);
//	ASSERT(m_nPeriod <= m_nNumPoints);
	if (m_nPeriod > m_nNumPoints)
		return S_OK;

	BLPVector<float> vfHurstLine(m_nNumPoints, HOLEF);
	float *pfDataPoints = GetStudyCalcRoute(pGraph, m_zCalcRouteId);
	if (pfDataPoints == NULL)
		return E_INVALIDARG;

	HurstExponents(m_nPeriod, m_nNumPoints, pfDataPoints, &vfHurstLine[0]);

	SetStudyDataLine(0, &vfHurstLine[0]);
	return S_OK;
}
