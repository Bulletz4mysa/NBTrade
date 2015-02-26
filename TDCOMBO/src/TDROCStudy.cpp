#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

HRESULT CTDROC::DoStudy(IBLPGraphData *pGraph)
{
	if (m_vCharts.empty())
		return E_HANDLE;

	int i,current,last;	
	int num_oversold=0,num_overbought=0;

	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;

	float *pfData = GetStudyCalcRoute(pGraph, m_zField);
	if (pfData == NULL)
		return E_HANDLE;
	
	if (m_nNumPoints < 1) 
		return E_HANDLE;	// not enough data points to do anything

	
	// Assign memory
	BLPVector<float> vTDROC(m_nNumPoints, HOLEF);
	BLPVector<float> vShadeTop(m_nNumPoints, HOLEF);
	BLPVector<float> vShadeBottom(m_nNumPoints, HOLEF);
	float *fTDROC   = &vTDROC[0];
	float *pfShadeTop = &vShadeTop[0];
	float *pfShadeBottom = &vShadeBottom[0];
	
	// Calculating ROC Line
	i=0;
	for(current=m_nNumPoints-1;current>=m_nPeriod;current--){
		last=current;
		while(i<m_nPeriod && last>=0){//find m_nPeriods back
			while( (pfData[last]==HOLEF || pfData[last]==INTERPOLATEF) 
				&& i<m_nNumPoints && last>=0)
				last--;
			last--;
			i++; 
		}
		if(last<0) break;
		if(i==m_nPeriod){//calculate
			fTDROC[current]=(pfData[current]/pfData[last])*100;
			i=0;
		}
	}

	if(m_nDuration_type != NONE){//shading	
		//loop thru values and assign shading
		for(current=m_nPeriod;current<m_nNumPoints-1;current++){
			if(m_nDuration_type == OVERBOUGHT || m_nDuration_type == BOTH){//user requested oversold
				if(fTDROC[current]>m_fOverbought) num_overbought++;//count number of overbought
				else num_overbought=0;

				if(num_overbought>=m_nDuration_length){
					pfShadeTop[current] = fTDROC[current];
					pfShadeBottom[current] = m_fOverbought;
				}
			}
			if(m_nDuration_type == OVERSOLD || m_nDuration_type == BOTH){ //user requested overbought
				if(fTDROC[current]<m_fOversold) num_oversold++;//count number of oversold
				else num_oversold=0;
	
				if(num_oversold>=m_nDuration_length){
					pfShadeTop[current] = m_fOversold;
					pfShadeBottom[current] = fTDROC[current];
				}
			}
		}
	}

	//Put up oscillating lines
	CComPtr<IStudyData> iSD;
	HRESULT hr = m_pTDROC_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;


	// Copy the study lines onto the Oscillator Chart
	iSD->put_StudyDataLine(0, m_nNumPoints, fTDROC);
	iSD->put_StudyDataLine(1, m_nNumPoints, pfShadeTop);
	iSD->put_StudyDataLine(2, m_nNumPoints, pfShadeBottom);

	return NOERROR;
}


HRESULT CTDROC::CreateStudy(IBLPGraphStudyList * pGraph)
{

	CChartLine *pChartLine;
	short nChartNum = 1;

	ChartLineParameters LineParms[] = 
	{
	{1, XRT_LPAT_SOLID, RGB(255, 255, 255)},	// DeMarker line
	{1, XRT_TOP_FILL, RGB(255, 0, 255)},		// Upper duration
	{1, XRT_LPAT_SOLID, RGB(255, 0, 255)}};	// Lower duration
	ChartPointParameters PointParms[] = 
	{{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0}};

	if (m_pLineParameters != NULL)
		memcpy( (void*)(LineParms), m_pLineParameters, sizeof( ChartLineParameters ) );

	HRESULT rc = AddStudy(pGraph,9, 1, LineParms, PointParms);
    //If there is no data for this security, it will fail at this point
    if (FAILED(rc))
        return rc;

	pChartLine = new CChartLine;
	pChartLine->SetLineParams(LineParms[0]);
	pChartLine->SetPointParams(PointParms[0]);
	m_vChartLine.push_back(pChartLine);

	m_pDurationTopLine = new CChartLine;
	m_pDurationTopLine->SetLineParams(LineParms[1]);
	m_pDurationTopLine->SetPointParams(PointParms[1]);

	m_pDurationBottomLine = new CChartLine;
	m_pDurationBottomLine->SetLineParams(LineParms[2]);
	m_pDurationBottomLine->SetPointParams(PointParms[2]);

	m_zones.overbought = m_fOverbought;
	m_zones.oversold = m_fOversold;

	CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
	if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum, this, &m_pTDROC_Chart) == S_OK)
		AddChart(m_pTDROC_Chart);
	else
		return S_FALSE;
	
	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	iGraphToolbar->AddStudyButton(0, m_strButtonText, this);
	
	return NOERROR;
}

HRESULT CTDROC::ConvertBuf(void * pBigBuf)
{
	long *pConvertFloat;

	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;
	pHead->period = ntohs(pHead->period);
	pHead->duration_type = ntohs(pHead->duration_type);
	pHead->duration_length = ntohs(pHead->duration_length);

	pConvertFloat 	 = (long *)&(pHead->overbought);
	*pConvertFloat 	 = ntohl(*pConvertFloat);
	pHead->overbought = *((float *)pConvertFloat);

	pConvertFloat 	 = (long *)&(pHead->oversold);
	*pConvertFloat 	 = ntohl(*pConvertFloat);
	pHead->oversold = *((float *)pConvertFloat);

	ConvertLineParameters(&pHead->line_params, 1);
	ConvertPointParameters(&pHead->point_params, 1);
	
	pHead->version = ntohs(pHead->version);
	
	//Assign member variables
	if (pHead->period != 0)
		m_nPeriod = pHead->period;		

	m_nDuration_type = pHead->duration_type;
	m_nDuration_length = pHead->duration_length;
	m_fOverbought = pHead->overbought;
	m_fOversold = pHead->oversold;

	m_pLineParameters = &pHead->line_params;
	m_pPointParameters = &pHead->point_params;

	memset( m_zField, 0, FIELD_LEN + 1 );
	strncpy(m_zField, pHead->field_id, FIELD_LEN );

	m_nVersion = pHead->version;

	return NOERROR;
}
