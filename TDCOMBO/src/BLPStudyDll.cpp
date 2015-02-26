#include <stdafx.h>

#include "blpstudydll.h"
#include "blpstudydll2.h"
#include "blpstudy.h"
#include "demarkstudies.h"

HRESULT CStudyDllExponentialMAVG::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr * pData = (StudyMessagePtr *)pBuf;

	pData->ExponMAVG.period = ntohs(pData->ExponMAVG.period);
  
#ifdef _DEBUG
  TRACE( "ConvertExponMAVG  period = %d version = %d",
	  pData->ExponMAVG.period,
	  pData->ExponMAVG.version );
#endif

	if (pData->ExponMAVG.version >= 2) 
	{
		pData->ExponMAVG.window_owner_id = ntohs(pData->ExponMAVG.window_owner_id);
#ifdef _DEBUG
		TRACE( " window_owner_id = %d", pData->ExponMAVG.window_owner_id );
#endif
	}

#ifdef _DEBUG
	TRACE( "\n" );
#endif

	ConvertLineParameters(pData->ExponMAVG.line_params, 1);
	if (pData->ExponMAVG.version >= 1)
		ConvertPointParameters(pData->ExponMAVG.point_params, 1);

	m_pLineParameters  = pData->ExponMAVG.line_params;
	m_pPointParameters = pData->ExponMAVG.point_params;
	m_nPeriod = pData->ExponMAVG.period;
	return NOERROR;
}

HRESULT CStudyDllExponentialMAVG::DoStudy(IBLPGraphData *pGraph)
{


	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;
	
	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

//	For ExpMAve we use close price only.  
//	But if your calculations depand on 
//	HLOC you can retrieve this data the same way


/*
	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		pfHigh = pfClose;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		pfLow = pfClose;

	float *pfOpen = GetStudyDataLine(pGraph, Open);
	if (pfOpen == NULL)
		pfOpen = pfClose;
*/
	
	if (m_nNumPoints < 2) 
		return E_HANDLE;			// not enough data points to do anything

	if(m_nPeriod < 1) m_nPeriod=1;

	m_pfEMAVG = new float[m_nNumPoints];
	
	pGraph->ExponentialAverage(m_nPeriod,m_nNumPoints,m_pfEMAVG,&pfClose);

	// Find first valid data point
	SetStudyDataLine(0, m_pfEMAVG);
	delete []m_pfEMAVG;

	return NOERROR;
}

HRESULT CStudyDllExponentialMAVG::CreateStudy(IBLPGraphStudyList * pGraph)
{
	
	int i = pGraph->AddStudy(m_nInstanceId, 
					m_nBaseId, 
					m_zCalcRouteId,
					1, // total number lines this study creates
					1, // visible lines 
					m_pLineParameters, 
					m_pPointParameters, 
					&pSD, 
					this);
	ASSERT(i == 0);
	if (i < 0)
		return i;

	return NOERROR;
}


/* ************************************* */
/* ************************************* */
/*    On Balance Volume (1201)           */
/* ************************************* */
/* ************************************* */



HRESULT CStudyDllOBV::ConvertBuf(void * pBuf)
{
    CBLPStudy::ConvertBuf(pBuf);
    StudyMessagePtr  pData = (StudyMessagePtr)pBuf;
 
    pData->numbars=ntohs(pData->numbars);
    pData->show_OBVZero = ntohs(pData->show_OBVZero);
    ConvertLineParameters(pData->line_params, 1);

    if (pData->version >= 1)
        ConvertPointParameters(pData->point_params, 1);

    memcpy(&m_pOBVLineParameters[0],pData->line_params,sizeof(ChartLineParameters));
    memcpy(&m_pOBVPointParameters[0],pData->point_params,sizeof(ChartPointParameters));

    m_nBarnum = pData->numbars;
    if(m_nBarnum > 0) m_nBarnum += 1;
    m_showZero = pData->show_OBVZero;

    return NOERROR;
}


HRESULT CStudyDllOBV::CreateStudy(IBLPGraphStudyList * pGraph)
{
    //If you do not have extra panels, just return S_OK here.
    //Oscillator line
    CChartLine *pChartLineOBV = new CChartLine;
    pChartLineOBV->SetLineParams(m_pOBVLineParameters[0]);
    pChartLineOBV->SetPointParams(m_pOBVPointParameters[0]);
    m_vChartLine.push_back(pChartLineOBV);

     HRESULT rc = pGraph->AddStudy(m_nInstanceId, 0, m_zCalcRouteId, 1, 0, m_pLineParameters, 
						m_pPointParameters, &pSD, this);
    //If there is no data for this security, it will fail at this point
    if (FAILED(rc))
        return rc;

	if (m_showZero) 
    {
        m_zones.overbought=0;
        m_OverboughtLine.put_LineColor(blgYellow);
    }

    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
    CComPtr<IChart> iChart;
    if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
        AddChart(iChart);
        iGraphToolbar->AddStudyButton(0, "OBV", this);
    }

    return S_OK;//CBLPStudy::CreateStudy(pGraph);
}


HRESULT CStudyDllOBV::DoStudy(IBLPGraphData *pGraph)
{
    int i,last,j,startBar;
        
    pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;

    float *pfClose = GetStudyDataLine(pGraph, Close);
    ASSERT(pfClose != NULL);
    if (pfClose == NULL)
        return E_HANDLE;
    float *pfVolume;

    pGraph->get_VolumeData(m_nNumPoints,&pfVolume);
    ASSERT(pfVolume != NULL);
    if (pfVolume == NULL)
        return E_HANDLE;

    if (m_nNumPoints < 1) 
        return E_HANDLE;                        // not enough data points to do anything

    BLPVector<float> pfOBV(m_nNumPoints);

    //Set starting point of calculation
    if(m_nBarnum<=0) 
        startBar=0;
    else
        startBar =      m_nNumPoints-m_nBarnum-1;

    if(startBar < 0)
        startBar = 0;


    i=startBar;
    for(j=0;j<i;j++) pfOBV[j]=HOLEF;
    while ((pfClose[i] == HOLEF || pfClose[i]==INTERPOLATEF)
           && i<m_nNumPoints) {
        pfOBV[i]=pfClose[i];
        i++;
    }
    pfOBV[i]=0;
    last=i;
    i++;
    while (i<m_nNumPoints) {
        while ((pfClose[i] == HOLEF || pfClose[i]==INTERPOLATEF)
               && i<m_nNumPoints) {
            pfOBV[i]=pfClose[i];
            i++;
        }
        if(pfClose[i]>pfClose[last]) 
            pfOBV[i]=pfOBV[last]+pfVolume[i];
        else if(pfClose[i]<pfClose[last])
            pfOBV[i]=pfOBV[last]-pfVolume[i];
        else pfOBV[i]=pfOBV[last];
        last=i;
        i++;
    }

    // Copy the study lines onto the Oscillator Chart
    CComPtr<IStudyData> iSD;
    HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    iSD->put_StudyDataLine(0, m_nNumPoints, &pfOBV[0]);
    
    return NOERROR;
}



/* ************************************* */
/* ************************************* */
/*    Simple MAVG (1202)           */
/* ************************************* */
/* ************************************* */



HRESULT CStudyDllSimpleMAVG::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr * pData = (StudyMessagePtr *)pBuf;

	pData->SimpleMAVG.period = ntohs(pData->SimpleMAVG.period);
 
#ifdef _DEBUG
TRACE( "ConvertSimpleMAVG period = %d version = %d",
	  pData->SimpleMAVG.period,
	  pData->SimpleMAVG.version);
#endif 

	if (pData->SimpleMAVG.version >= 2) 
	{
		pData->SimpleMAVG.window_owner_id = ntohs(pData->SimpleMAVG.window_owner_id);
#ifdef _DEBUG
		TRACE( " window_owner_id = %d", pData->SimpleMAVG.window_owner_id );
#endif
	}

#ifdef _DEBUG
	TRACE( "\n" );
#endif

	ConvertLineParameters(pData->SimpleMAVG.line_params, 1);

	if (pData->SimpleMAVG.version >= 1) {
		ConvertPointParameters(pData->SimpleMAVG.point_params, 1);
	}
		m_pPointParameters = pData->SimpleMAVG.point_params;

	m_pLineParameters  = pData->SimpleMAVG.line_params; 
	m_nPeriod = pData->SimpleMAVG.period;
	
	return NOERROR;
}

HRESULT CStudyDllSimpleMAVG::DoStudy(IBLPGraphData *pGraph)
{
	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;
	
	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

//	For ExpMAve we use close price only.  
//	But if your calculations depand on 
//	HLOC you can retrieve this data the same way


/*
	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		pfHigh = pfClose;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		pfLow = pfClose;

	float *pfOpen = GetStudyDataLine(pGraph, Open);
	if (pfOpen == NULL)
		pfOpen = pfClose;
*/
	
	m_pfMAVG = new float[m_nNumPoints];


	if (m_nPeriod < 1) 
		m_nPeriod= 2;	// just to be safe

	if (m_nNumPoints < m_nPeriod) 
		return E_HANDLE;			// not enough data points to do anything

	// Find first valid data point

	pGraph->MovingAverage(m_nPeriod,m_nNumPoints,pfClose,&m_pfMAVG);

	SetStudyDataLine(0, m_pfMAVG);
	delete []m_pfMAVG;

	return NOERROR;
}

HRESULT CStudyDllSimpleMAVG::CreateStudy(IBLPGraphStudyList * pGraph)
{
	
	int i = pGraph->AddStudy(m_nInstanceId, 
					m_nBaseId, 
					m_zCalcRouteId,
					1, // total number lines this study creates
					1, // visible lines 
					m_pLineParameters, 
					m_pPointParameters, 
					&pSD, 
					this);
	ASSERT(i == 0);
	if (i < 0)
		return i;

	return NOERROR;
}



/* ************************************* */
/* ************************************* */
/*    Relative Strength Index (1203)     */
/* ************************************* */
/* ************************************* */



HRESULT CStudyDllRSI::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	m_nBaseId=INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW;
	StudyMessagePtr * pData = (StudyMessagePtr *)pBuf;
	
	pData->RSI.period=ntohs(pData->RSI.period);
	ConvertOscillatorParameters(&pData->RSI.zone);

	// How many bars from last point to use for start of calculation
	pData->RSI.numStartBar=ntohs(pData->RSI.numStartBar);

	if (pData->RSI.version >= 2) 
	{
		pData->RSI.window_owner_id = ntohs(pData->RSI.window_owner_id);
#ifdef _DEBUG
		TRACE( " window_owner_id = %d", pData->RSI.window_owner_id );
#endif
	}

	ConvertLineParameters(pData->RSI.line_params, 1);
	if (pData->RSI.version >= 1)
		ConvertPointParameters(pData->RSI.point_params, 1);

	//Choose which data buffer to do RSI on
	memset(m_field_id, 0, 9);
    strncpy(m_field_id, pData->RSI.field_id, 8);

	m_nPeriod = pData->RSI.period;

	// RSI Line params
	memcpy(&m_pLineParameters3[0],pData->RSI.line_params,sizeof(ChartLineParameters));
	memcpy(&m_pPointParameters3[0],pData->RSI.point_params,sizeof(ChartPointParameters));
	
	m_nOverSold = pData->RSI.zone.oversold;
	m_nOverBought = pData->RSI.zone.overbought;
	m_nStartBar = pData->RSI.numStartBar;

	// Params for overbought/sold lines
	m_pLineParameters3[1].line_color=RGB(0,255,0);// Overbougt
	m_pLineParameters3[1].line_style=2;
	m_pLineParameters3[1].line_width=1;

	m_pPointParameters3[1].point_color=RGB(255,255,255); 
	m_pPointParameters3[1].point_style=1;
	m_pPointParameters3[1].point_width=1;

	m_pLineParameters3[2].line_color=RGB(255,0,0); //Oversold
	m_pLineParameters3[2].line_style=2;
	m_pLineParameters3[2].line_width=1;

	m_pPointParameters3[2].point_color=RGB(255,255,255);
	m_pPointParameters3[2].point_style=1;
	m_pPointParameters3[2].point_width=1;
	
	return NOERROR;
}


HRESULT CStudyDllRSI::DoStudy(IBLPGraphData *pGraph)
{
	int i,last,j,k,current;
	float up_avg,down_avg;
	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;

	
	float *pfData = GetStudyCalcRoute(pGraph, m_field_id);
	ASSERT(pfData != NULL);
	if (pfData == NULL)
		return E_HANDLE;


	if (m_nNumPoints < 1) 
		return E_HANDLE;			// not enough data points to do anything

	// Assign memor for RSI and overbought/sold lines
	m_pfRSI = new float[m_nNumPoints];
	m_pfOverBought = new float[m_nNumPoints];
	m_pfOverSold = new float[m_nNumPoints];
	
	// Find first valid data point

	if(m_nStartBar < 0){  //Start at beginning of data
		i=0;
		k=0;
		while(k < m_nPeriod){
			while ((pfData[i] == HOLEF || pfData[i]==INTERPOLATEF)
				&& i<m_nNumPoints) {
				m_pfRSI[i]=pfData[i];
				i++;
			}
			m_pfRSI[i]=0;
			i++;
			k++;
		}
	}
	else{   // User defined start date
		i = m_nNumPoints-m_nStartBar-1;
		if(i < 0)
			i = 0;

		for(j=0;j<i;j++) m_pfRSI[j]=HOLEF;
		while ((pfData[i] == HOLEF || pfData[i]==INTERPOLATEF)
				&& i<m_nNumPoints) {
			m_pfRSI[i]=pfData[i];
			i++;
		}	
	}

	
	m_pfRSI[i]=0;
	last=i;
	current=i;
	up_avg=0;	
	down_avg=0;
	k=0;

	// Get first average point
	while ( k < m_nPeriod ){
		while ((pfData[i] == HOLEF || pfData[i]==INTERPOLATEF)
				&& i<m_nNumPoints) {
			m_pfRSI[i]=pfData[i];
			i--;
		}
		m_pfRSI[i]=HOLEF;
		if (pfData[i] > pfData[last])
			up_avg += (pfData[i] - pfData[last]);
		else
			down_avg += (pfData[last] - pfData[i]);
		k++;
		last=i;
		i--;
	}
	last=current;
	i=current+1;
	up_avg = up_avg/m_nPeriod;
	down_avg = down_avg/m_nPeriod;
	m_pfRSI[last]= 100 - ( 100 / (1 + (up_avg / down_avg)));
	
	// RSI calculation
	while (i<m_nNumPoints) {
		while ((pfData[i] == HOLEF || pfData[i]==INTERPOLATEF)
				&& i<m_nNumPoints) {
			m_pfRSI[i]=pfData[i];
			i++;
		}
		if (pfData[i] > pfData[last]){
			up_avg = ((up_avg * (m_nPeriod - 1)) + (pfData[i]-pfData[last]))/m_nPeriod;
			down_avg = (down_avg * (m_nPeriod -1)) / m_nPeriod;
		}
		else {
			up_avg = (up_avg * (m_nPeriod - 1))/m_nPeriod;
			down_avg = ((down_avg * (m_nPeriod -1))+pfData[last]-pfData[i]) / m_nPeriod;
		}
		m_pfRSI[i]= 100 - ( 100 / (1 + (up_avg / down_avg)));	
		last=i;
		i++;
	}
	
	// Set overbought/sold lines
	for(i=0;i<m_nNumPoints;i++){
		m_pfOverBought[i]=m_nOverBought;
		m_pfOverSold[i]=m_nOverSold;
	}

	//Assign study lines
	SetStudyDataLine(0, m_pfRSI);
	SetStudyDataLine(1, m_pfOverBought);
	SetStudyDataLine(2, m_pfOverSold);

	//Free memory space allocated for calculations
	delete []m_pfRSI;
	delete []m_pfOverBought;
	delete []m_pfOverSold;
	return NOERROR;
}


HRESULT CStudyDllRSI::CreateStudy(IBLPGraphStudyList * pGraph)
{
	
#ifdef _DEBUG
		TRACE( " BadId=%d\n",m_nBaseId);
#endif
		int i = pGraph->AddStudy(m_nInstanceId, 
					m_nBaseId, 
					m_zCalcRouteId,
					3, // total number lines this study creates
					3, // RSI, overbought, oversold
					m_pLineParameters3, 
					m_pPointParameters3, 
					&pSD, 
					this);
	ASSERT(i == 0);
	if (i < 0)
		return i;

	return NOERROR;
}




/* ************************************* */
/* ************************************* */
/*    Stochastics Oscillator(TAS)-1204   */
/* ************************************* */
/* ************************************* */


HRESULT CStudyDllTAS::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	struct tagStudyHeader{
		short msg_id;   
		short instance_id;
		short base_id;
	  } *pHeader = (struct tagStudyHeader *)pBuf;

	
	if (pHeader->msg_id == CStudyDll::blgStudyDllTAS) {
		StudyMessagePtr pData = (StudyMessagePtr)pBuf;
		
		pData->period1=ntohs(pData->period1);
		pData->period2=ntohs(pData->period2);
		pData->period3=ntohs(pData->period3);
		pData->period4=ntohs(pData->period4);

		pData->show_tas_fast=ntohs(pData->show_tas_fast);
		pData->show_tas_slow=ntohs(pData->show_tas_slow);
		pData->show_tas_d_ds = ntohs(pData->show_tas_d_ds);
		
		ConvertOscillatorParameters(&pData->zone);

		ConvertLineParameters(pData->line_params, 4);
		if (pData->version >= 1)
			ConvertPointParameters(pData->point_params, 4);

		if (pData->version >= 2) 
		{
			pData->window_owner_id = ntohs(pData->window_owner_id);
	#ifdef _DEBUG
			TRACE( " window_owner_id = %d", pData->window_owner_id );
	#endif
		}
		
		m_nKPeriod=pData->period1;
		m_nDPeriod=pData->period2;
		m_nDSPeriod=pData->period3;
		m_nDSSPeriod=pData->period4;

		m_zones.overbought = pData->zone.overbought;
		m_zones.oversold = pData->zone.oversold;

		m_show_k_d = pData->show_tas_fast;
		m_show_ds_dss = pData->show_tas_slow;
		m_show_d_ds = pData->show_tas_d_ds;

		/* Parameters for %K and %D lines with Overbought and Oversold Lines*/
		memcpy(&m_pK_D_LineParameters[0],&pData->line_params[0],sizeof(ChartLineParameters));
		memcpy(&m_pK_D_PointParameters[0],&pData->point_params[0],sizeof(ChartPointParameters));

		memcpy(&m_pK_D_LineParameters[1],&pData->line_params[1],sizeof(ChartLineParameters));
		memcpy(&m_pK_D_PointParameters[1],&pData->point_params[1],sizeof(ChartPointParameters));
		
		/* Parameters for %D And %DS lines with Overbought and Oversold Lines*/
		memcpy(&m_pD_DS_LineParameters[0],&pData->line_params[1],sizeof(ChartLineParameters));
		memcpy(&m_pD_DS_PointParameters[0],&pData->point_params[1],sizeof(ChartPointParameters));

		memcpy(&m_pD_DS_LineParameters[1],&pData->line_params[2],sizeof(ChartLineParameters));
		memcpy(&m_pD_DS_PointParameters[1],&pData->point_params[2],sizeof(ChartPointParameters));

		/* Parameters for %DS and %DSS lines and Overbought and Oversold Lines*/
		memcpy(&m_pDS_DSS_LineParameters[0],&pData->line_params[2],sizeof(ChartLineParameters));
		memcpy(&m_pDS_DSS_PointParameters[0],&pData->point_params[2],sizeof(ChartPointParameters));
		
		memcpy(&m_pDS_DSS_LineParameters[1],&pData->line_params[3],sizeof(ChartLineParameters));
		memcpy(&m_pDS_DSS_PointParameters[1],&pData->point_params[3],sizeof(ChartPointParameters));

		/* Convert the charts colors for each TAS Panel*/
		ConvertChartColors(&pData->chart_colors);
		ConvertChartColors(&pData->second_chart_colors);
		ConvertChartColors(&pData->third_chart_colors);
		m_crBackgroundKDchart   = pData->chart_colors.chart_background;
		m_crBackgroundDDSchart  = pData->second_chart_colors.chart_background;
		m_crBackgroundDSDSSchart= pData->third_chart_colors.chart_background;
		}
	else if (pHeader->msg_id == blgTAS) {
		OldStudyMessagePtr pData = (OldStudyMessagePtr)pBuf;

		m_nKPeriod = ntohs(pData->period1);
		m_nDPeriod = ntohs(pData->period2);
		m_nDSPeriod = ntohs(pData->period3);
		m_nDSSPeriod = ntohs(pData->period4);

		m_show_k_d = pData->show_tas_fast;
		m_show_ds_dss = pData->show_tas_slow;
		m_show_d_ds = FALSE;

		m_pLineParameters = pData->line_params;
		ConvertLineParameters(m_pLineParameters, 4);

		ChartPointParameters nullPointParms[4] = {{0}};
		if (pData->version >= 1) {
			m_pPointParameters = pData->point_params;
			ConvertPointParameters(m_pPointParameters, 4);
			ConvertChartColors(&pData->chart_colors);
			m_crBackgroundKDchart = pData->chart_colors.chart_background;
			ConvertChartColors(&pData->second_chart_colors);
			m_crBackgroundDDSchart = pData->second_chart_colors.chart_background;
			}
		else {
			m_pPointParameters = nullPointParms;
			}
	

		/* Parameters for %K and %D lines with Overbought and Oversold Lines*/
		memcpy(&m_pK_D_LineParameters[0],&m_pLineParameters[0],sizeof(ChartLineParameters));
		memcpy(&m_pK_D_PointParameters[0],&m_pPointParameters[0],sizeof(ChartPointParameters));

		memcpy(&m_pK_D_LineParameters[1],&m_pLineParameters[1],sizeof(ChartLineParameters));
		memcpy(&m_pK_D_PointParameters[1],&m_pPointParameters[1],sizeof(ChartPointParameters));

		/* Parameters for %D And %DS lines with Overbought and Oversold Lines*/
		memcpy(&m_pDS_DSS_LineParameters[0],&m_pLineParameters[1],sizeof(ChartLineParameters));
		memcpy(&m_pDS_DSS_PointParameters[0],&m_pPointParameters[1],sizeof(ChartPointParameters));

		memcpy(&m_pDS_DSS_LineParameters[1],&m_pLineParameters[2],sizeof(ChartLineParameters));
		memcpy(&m_pDS_DSS_PointParameters[1],&m_pPointParameters[2],sizeof(ChartPointParameters));

		ConvertOscillatorParameters(&pData->zone);
		m_zones.overbought = pData->zone.overbought;
		m_zones.oversold = pData->zone.oversold;
		}

	return NOERROR;
}


HRESULT CStudyDllTAS::DoStudy(IBLPGraphData *pGraph)
{
//	%k  = ((close - lowest low in period1)/(highest high - lowest low in period1)) * 100
//	%d  = period2-simple mavg of %k
//	%ds = period3-simple mavg of %d
//	%dss= period4-simple mavg of %ds

	int i,l,m;
	float ll, hh;
//	float c;

	static long nNumPoints = 0;
	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;
	nNumPoints = m_nNumPoints;
	//Assign memory for K,D,DS,DSS
	m_pfK = new float[m_nNumPoints];
	m_pfD = new float[m_nNumPoints];
	m_pfDS = new float[m_nNumPoints];
	m_pfDSS = new float[m_nNumPoints];

	for(i=0;i<m_nNumPoints;i++){
		m_pfK[i]=HOLEF;
		m_pfD[i]=HOLEF;
		m_pfDS[i]=HOLEF;
		m_pfDSS[i]=HOLEF;
	}

	float *pfHigh = GetStudyDataLine(pGraph, High);
	if (pfHigh == NULL)
		return E_HANDLE;
	float *pfLow = GetStudyDataLine(pGraph, Low);
	if (pfLow == NULL)
		return E_HANDLE;
	float *pfClose = GetStudyDataLine(pGraph, Close);
	if (pfClose == NULL)
		return E_HANDLE;
	
	if (m_nNumPoints < 1) 
		return E_HANDLE;			// not enough data points to do anything
	if (m_nNumPoints < m_nKPeriod) 
		return E_HANDLE; // not enough data points to do anything

    //Calculate the %K line
    hh = 0;
    ll = 0;
    for (i=m_nNumPoints-1; i>m_nKPeriod-2; i--) {
	    if (!IsInvalid(pfHigh[i],pfHigh[i],pfLow[i],pfClose[i])) {
		    l = 0; // how many spots I've gone back
	        m = 1; // how many valid points I've got (max is m_nKPeriod)
		    hh = pfHigh[i];
		    ll = pfLow[i];
		    while (m < m_nKPeriod) {
			    l++;
			    if (i >= l) {
			        if (!IsInvalid(pfHigh[i-l],pfHigh[i-l],pfLow[i-l],pfClose[i-l])) {
				        m++;
			            if (hh < pfHigh[i-l]) hh = pfHigh[i-l];
				        if (ll > pfLow[i-l]) ll = pfLow[i-l];
					}
				} else {
				    break;
				}
			}

		    if (m == m_nKPeriod) {
			    if ((hh-ll) != 0) {
				    m_pfK[i]=((pfClose[i]-ll)/(hh-ll))*100;
				} else {
				    m_pfK[i] = 100;
				}
			}
		}
	}

	//%D Line
	pGraph->MovingAverage(m_nDPeriod,m_nNumPoints,m_pfK,&m_pfD);
	//%DS Line
	pGraph->MovingAverage(m_nDSPeriod,m_nNumPoints,m_pfD,&m_pfDS);
	//%DSS Line
	pGraph->MovingAverage(m_nDSSPeriod,m_nNumPoints,m_pfDS,&m_pfDSS);

	int nLineNum = 0;
	if (m_pK_D_Chart != NULL) {
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pK_D_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;

		// Copy the study lines onto the Oscillator Chart
		iSD->put_StudyDataLine(0, m_nNumPoints, m_pfK);
		iSD->put_StudyDataLine(1, m_nNumPoints, m_pfD);
		}

	if (m_pD_DS_Chart != NULL) {
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pD_DS_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;

		// Copy the study lines onto the Oscillator Chart
		iSD->put_StudyDataLine(0, m_nNumPoints, m_pfD);
		iSD->put_StudyDataLine(1, m_nNumPoints, m_pfDS);
		}

	if (m_pDS_DSS_Chart != NULL) {
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pDS_DSS_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;

		// Copy the study lines onto the Oscillator Chart
		iSD->put_StudyDataLine(0, m_nNumPoints, m_pfDS);
		iSD->put_StudyDataLine(1, m_nNumPoints, m_pfDSS);
		}

	delete[]m_pfK;
	delete[]m_pfD;
	delete[]m_pfDS;
	delete[]m_pfDSS;
	return NOERROR;
}

HRESULT CStudyDllTAS::CreateStudy(IBLPGraphStudyList * pGraph)
{
	BOOL bAddButton = FALSE;
	
#ifdef _DEBUG
		TRACE( " BadId=%d\n",m_nBaseId);
#endif
		 HRESULT hr = pGraph->AddStudy(m_nInstanceId,
						0,
						m_zCalcRouteId,
						12, // total number lines this study creates
						0, // no visible lines on Main Chart
						m_pK_D_LineParameters, 
						m_pK_D_PointParameters, 
						&pSD, 
						this);

		 ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	short nChartNum = 1;
	CChartLine *pChartLine;
	if (m_show_k_d) {
		CStudyDllTAS K_D_Study(*this);
		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pK_D_LineParameters[0]);
		pChartLine->SetPointParams(m_pK_D_PointParameters[0]);
		K_D_Study.m_vChartLine.push_back(pChartLine);

		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pK_D_LineParameters[1]);
		pChartLine->SetPointParams(m_pK_D_PointParameters[1]);
		K_D_Study.m_vChartLine.push_back(pChartLine);
		
		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, &K_D_Study, &m_pK_D_Chart) == S_OK) {
			AddChart(m_pK_D_Chart);
			m_pK_D_Chart->put_BackgroundColor(m_crBackgroundKDchart);
			bAddButton = TRUE;
			}
		}

	if (m_show_d_ds) {
		CStudyDllTAS D_DS_Study(*this);
		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pD_DS_LineParameters[0]);
		pChartLine->SetPointParams(m_pD_DS_PointParameters[0]);
		D_DS_Study.m_vChartLine.push_back(pChartLine);

		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pD_DS_LineParameters[1]);
		pChartLine->SetPointParams(m_pD_DS_PointParameters[1]);
		D_DS_Study.m_vChartLine.push_back(pChartLine);		

		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, &D_DS_Study, &m_pD_DS_Chart) == S_OK) {
			AddChart(m_pD_DS_Chart);
			m_pD_DS_Chart->put_BackgroundColor(m_crBackgroundDDSchart);
			bAddButton = TRUE;
			}
		}

	if (m_show_ds_dss) {
		CStudyDllTAS DS_DSS_Study(*this);
		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pDS_DSS_LineParameters[0]);
		pChartLine->SetPointParams(m_pDS_DSS_PointParameters[0]);
		DS_DSS_Study.m_vChartLine.push_back(pChartLine);

		pChartLine = new CChartLine;
		pChartLine->SetLineParams(m_pDS_DSS_LineParameters[1]);
		pChartLine->SetPointParams(m_pDS_DSS_PointParameters[1]);
		DS_DSS_Study.m_vChartLine.push_back(pChartLine);
		
		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, &DS_DSS_Study, &m_pDS_DSS_Chart) == S_OK) {
			AddChart(m_pDS_DSS_Chart);
			m_pDS_DSS_Chart->put_BackgroundColor(m_crBackgroundDSDSSchart);
			bAddButton = TRUE;
			}
		}

	if (bAddButton) {
		CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
		iGraphToolbar->AddStudyButton(0, "TAS", this);
		}
	
	return NOERROR;
}


/* ************************************* */
/* ************************************* */
/*    Chaikin Oscillator(CHKO) - 1205    */
/* ************************************* */
/* ************************************* */

HRESULT CStudyDllCHKO::ConvertBuf(void * pBuf)
{
    CBLPStudy::ConvertBuf(pBuf);
    StudyMessagePtr pData  = (StudyMessagePtr)pBuf;
 
    pData->show_CHKOZero = ntohs(pData->show_CHKOZero);

    ConvertLineParameters(pData->line_params, 1);
    if (pData->version >= 1)
        ConvertPointParameters(pData->point_params, 1);

    memcpy(&m_pCHKOLineParameters[0],pData->line_params,sizeof(ChartLineParameters));
    memcpy(&m_pCHKOPointParameters[0],pData->point_params,sizeof(ChartPointParameters));

    // Choice to turn zero reference line on/off
    m_showZero = pData->show_CHKOZero;
        
    return NOERROR;
}


HRESULT CStudyDllCHKO::CreateStudy(IBLPGraphStudyList * pGraph)
{
    //Oscillator line
    CChartLine *pChartLineCHKO = new CChartLine;
    pChartLineCHKO->SetLineParams(m_pCHKOLineParameters[0]);
    pChartLineCHKO->SetPointParams(m_pCHKOPointParameters[0]);
    m_vChartLine.push_back(pChartLineCHKO);

	HRESULT rc = pGraph->AddStudy(m_nInstanceId, 0, m_zCalcRouteId, 1, 0, m_pLineParameters, 
						m_pPointParameters, &pSD, this);
    //If there is no data for this security, it will fail at this point
    if (FAILED(rc))
        return rc;

    if (m_showZero) 
    {
        m_zones.overbought=0;
        m_OverboughtLine.put_LineColor(blgYellow);
    }

    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
    CComPtr<IChart> iChart;
    if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &iChart) == S_OK) {
        AddChart(iChart);
        iGraphToolbar->AddStudyButton(0, "CHKO", this);
    }

    return S_OK;
}


HRESULT CStudyDllCHKO::DoStudy(IBLPGraphData *pGraph)
{
    int i,per,last;
    float *pfVolume;

    pGraph->get_NumPoints(&m_nNumPoints);

    float *pfHigh = GetStudyDataLine(pGraph, High);
    if (pfHigh == NULL)
        return E_HANDLE;
        
    float *pfLow = GetStudyDataLine(pGraph, Low);
    if (pfLow == NULL)
        return E_HANDLE;
        
    float *pfClose = GetStudyDataLine(pGraph, Close);
    if (pfClose == NULL)
        return E_HANDLE;
        
                
    pGraph->get_VolumeData(m_nNumPoints,&pfVolume);
    if (pfVolume == NULL)
        return E_HANDLE;
        
    if (m_nNumPoints < 1) 
        return E_HANDLE;        // not enough data points to do anything

    BLPVector<float> pfCHKO(m_nNumPoints); //Actual Dataline
    BLPVector<float> pfACD(m_nNumPoints);  
    BLPVector<float> pf3MAVG(m_nNumPoints);
    BLPVector<float> pf10MAVG(m_nNumPoints);

    // Calculate Accumulation/Distribution line
    i=0;
    pfACD[i]=0;
    last=0;
    i++;
    while (i<m_nNumPoints) {
        while ((pfClose[i] == HOLEF || pfClose[i]==INTERPOLATEF)
               && i<m_nNumPoints) {
            pfACD[i]=pfClose[i];
            i++;
        }
        if((pfHigh[i]-pfLow[i])!=0)
            pfACD[i]=pfACD[last]+(((pfClose[i]-pfLow[i])-(pfHigh[i]-pfClose[i]))*pfVolume[i])/(pfHigh[i]-pfLow[i]);
        else
            pfACD[i]=0;
        last++;
        i++;
    }

    float* ppf3MAVG=&pf3MAVG[0];
    float* ppf10MAVG=&pf10MAVG[0];
    // Moving Average line with period of 3 days.
    per = 3;
    pGraph->MovingAverage(per,m_nNumPoints,&pfACD[0],&ppf3MAVG);
    per = 10;
    // Moving Average line with period of 10 days.
    pGraph->MovingAverage(per,m_nNumPoints,&pfACD[0],&ppf10MAVG);

    i=0;
    while(i<m_nNumPoints){
        while(pf10MAVG[i] == HOLEF || pf10MAVG[i] == INTERPOLATEF){
            pfCHKO[i]=pf10MAVG[i];
            i++;
        }
        pfCHKO[i]=(pf3MAVG[i]-pf10MAVG[i]);
        i++;
    }

    // Copy the study lines onto the Oscillator Chart
    CComPtr<IStudyData> iSD;
    HRESULT hr = (m_vCharts[0])->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    iSD->put_StudyDataLine(0, m_nNumPoints, &pfCHKO[0]);
        
    return NOERROR;
}



/* ************************************* */
/* ************************************* */
/*    Rate of Change (ROC) - 1206        */
/* ************************************* */
/* ************************************* */

HRESULT CStudyDllROC::ConvertBuf(void * pBuf)
{	
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr *pData = (StudyMessagePtr *)pBuf;
	
	m_nBaseId=INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW;
	pData->ROC.period1 = ntohs(pData->ROC.period1);
	pData->ROC.period2 = ntohs(pData->ROC.period2);
	pData->ROC.period3 = ntohs(pData->ROC.period3);
	pData->ROC.period4 = ntohs(pData->ROC.period4);
	
	ConvertLineParameters(pData->ROC.line_params, 4);
	if (pData->ROC.version >= 1)
		ConvertPointParameters(pData->ROC.point_params, 4);

	if (pData->ROC.version >= 2) 
	{
		pData->ROC.window_owner_id = ntohs(pData->ROC.window_owner_id);
#ifdef _DEBUG
		TRACE( " window_owner_id = %d", pData->ROC.window_owner_id );
#endif
	}
	
	// Choose data buffer for ROC(ex. open, high, low, close)
	memset(m_field_id, 0, 9);
    strncpy(m_field_id, pData->ROC.field_id, 8);

	// Set params for ROC Lines
	memcpy(&m_pROCLineParameters[0],&pData->ROC.line_params[0],sizeof(ChartLineParameters));
	memcpy(&m_pROCPointParameters[0],&pData->ROC.point_params[0],sizeof(ChartPointParameters));
	
	memcpy(&m_pROCLineParameters[1],&pData->ROC.line_params[1],sizeof(ChartLineParameters));
	memcpy(&m_pROCPointParameters[1],&pData->ROC.point_params[1],sizeof(ChartPointParameters));

	memcpy(&m_pROCLineParameters[2],&pData->ROC.line_params[2],sizeof(ChartLineParameters));
	memcpy(&m_pROCPointParameters[2],&pData->ROC.point_params[2],sizeof(ChartPointParameters));

	memcpy(&m_pROCLineParameters[3],&pData->ROC.line_params[3],sizeof(ChartLineParameters));
	memcpy(&m_pROCPointParameters[3],&pData->ROC.point_params[3],sizeof(ChartPointParameters));

	m_nPeriod1=pData->ROC.period1;
	m_nPeriod2=pData->ROC.period2;
	m_nPeriod3=pData->ROC.period3;
	m_nPeriod4=pData->ROC.period4;
		
	
	m_lines_to_draw=0;
	if(m_nPeriod1!= 0)
		m_lines_to_draw++;
	if(m_nPeriod2!= 0)
		m_lines_to_draw++;
	if(m_nPeriod3!= 0)
		m_lines_to_draw++;
	if(m_nPeriod4!= 0)
		m_lines_to_draw++;
	return NOERROR;
}


HRESULT CStudyDllROC::DoStudy(IBLPGraphData *pGraph)
{
	int i,current,last;

	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;
	
	float *pfData = GetStudyCalcRoute(pGraph, m_field_id);
	if (pfData == NULL)
		return E_HANDLE;
	
	if (m_nNumPoints < 1) 
		return E_HANDLE;	// not enough data points to do anything

	
	// Assign memory
	m_pfROC1=new float[m_nNumPoints];
	m_pfROC2=new float[m_nNumPoints];
	m_pfROC3=new float[m_nNumPoints];
	m_pfROC4=new float[m_nNumPoints];

	for(i=0;i<m_nNumPoints;i++){
		m_pfROC1[i]=HOLEF;
		m_pfROC2[i]=HOLEF;
		m_pfROC3[i]=HOLEF;
		m_pfROC4[i]=HOLEF;
	}
	
	// Calculating 1st ROC Line
	i=0;
	for(current=m_nNumPoints-1;current>=m_nPeriod1;current--){
		last=current;
		while(i<m_nPeriod1){
			while( (pfData[last]==HOLEF || pfData[last]==INTERPOLATEF)
				&& i<m_nNumPoints)
				last--;
			last--;
			i++;
		}
		if(i==m_nPeriod1){
			m_pfROC1[current]=((pfData[current]-pfData[last])/pfData[last])*100;
			i=0;
		}
	}
	//Calculating 2nd ROC Line if its period is not 0.
	if(m_nPeriod2 != 0){
		i=0;
		for(current=m_nNumPoints-1;current>=m_nPeriod2;current--){
			last=current;
			while(i<m_nPeriod2){
				while( (pfData[last]==HOLEF || pfData[last]==INTERPOLATEF)
					&& i<m_nNumPoints)
					last--;
				last--;
				i++;
			}
			if(i==m_nPeriod2){
				m_pfROC2[current]=((pfData[current]-pfData[last])/pfData[last])*100;
				i=0;
			}
		}
	}
	//Calculating 3rd ROC Line if its period is not 0.
	if(m_nPeriod3 != 0){
		i=0;
	
		for(current=m_nNumPoints-1;current>=m_nPeriod3;current--){
			last=current;
			while(i<m_nPeriod3){
				while( (pfData[last]==HOLEF || pfData[last]==INTERPOLATEF)
					&& i<m_nNumPoints)
					last--;
				last--;
				i++;
			}
			if(i==m_nPeriod3){
				m_pfROC3[current]=((pfData[current]-pfData[last])/pfData[last])*100;
				i=0;
			}
		}
	}
	//Calculating 4th ROC Line if its period is not 0.
	if(m_nPeriod4 !=0) { 
		i=0;
	
		for(current=m_nNumPoints-1;current>=m_nPeriod4;current--){
			last=current;
			while(i<m_nPeriod4){
				while( (pfData[last]==HOLEF || pfData[last]==INTERPOLATEF)
					&& i<m_nNumPoints)
					last--;
				last--;
				i++;
			}
			if(i==m_nPeriod4){
				m_pfROC4[current]=((pfData[current]-pfData[last])/pfData[last])*100;
				i=0;
			}
		}
	}

	// Setting Study lines and deleting memory spaces 
	SetStudyDataLine(0,m_pfROC1);

	if(m_nPeriod2 != 0){
		SetStudyDataLine(1,m_pfROC2);
	}
	if(m_nPeriod3 != 0){
		SetStudyDataLine(2,m_pfROC3);
	}
	if(m_nPeriod4 !=0){
		SetStudyDataLine(3,m_pfROC4);
	}
	
	delete []m_pfROC1;
	delete []m_pfROC2;
	delete []m_pfROC3;
	delete []m_pfROC4;
	return NOERROR;
}

HRESULT CStudyDllROC::CreateStudy(IBLPGraphStudyList * pGraph)
{
	
#ifdef _DEBUG
		TRACE( " BadId=%d\n",m_nBaseId);
#endif
	int i;
	
	m_nBaseId = INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW;
	i= pGraph->AddStudy(m_nInstanceId,
					m_nBaseId,
					m_zCalcRouteId,
					4, // total number lines this study creates
					m_lines_to_draw,
					m_pROCLineParameters, 
					m_pROCPointParameters, 
					&pSD, 
					this);
	
	if (i < 0)
		return i;

	return NOERROR;
}






/* ************************************* */
/* ************************************* */
/*    Keltner Bands (KLTN) - 1207        */
/* ************************************* */
/* ************************************* */


HRESULT CKeltnerBands::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);

	StudyMessagePtr pHead = (StudyMessagePtr)pBuf;


    m_nVersion = ntohs(pHead->version);
	m_nBandPeriod = ntohs(pHead->band_period);
	m_nMiddlePeriod = ntohs(pHead->middle_period);

	int *ptemp = (int *)&(pHead->upper_band_percent);
	*ptemp = ntohl(*ptemp);
	m_fUpperBandPercent = pHead->upper_band_percent/100.0f;

	ptemp = (int *)&(pHead->lower_band_percent);
	*ptemp = ntohl(*ptemp);
	m_fLowerBandPercent = pHead->lower_band_percent/100.0f;

	ConvertLineParameters(pHead->line_params, 3);
	ConvertPointParameters(pHead->point_params, 3);

	m_pLineParameters = pHead->line_params;
	m_pPointParameters = pHead->point_params;
	return S_OK;
}


HRESULT CKeltnerBands::DoStudy(IBLPGraphData *pGraph)
{

	pGraph->get_NumPoints(&m_nNumPoints);
	
	float *pfCloseOld = GetStudyDataLine(pGraph, Close);
	if (pfCloseOld == NULL)
		return E_HANDLE;

	float *pfHighOld = GetStudyDataLine(pGraph, High);
	if (pfHighOld == NULL)
		return E_HANDLE;

	float *pfLowOld = GetStudyDataLine(pGraph, Low);
	if (pfLowOld == NULL)
		return E_HANDLE;

	BLPVector<float> vfClose(m_nNumPoints, HOLEF);
	BLPVector<float> vfHigh(m_nNumPoints, HOLEF);
	BLPVector<float> vfLow(m_nNumPoints, HOLEF);
    BLPVector<float> vfAveragePrice(m_nNumPoints, HOLEF);
	BLPVector<long> vnBadIndex(m_nNumPoints, HOLEF);

	float *pfClose        = &vfClose[0];
	float *pfHigh         = &vfHigh[0];
	float *pfLow          = &vfLow[0];
	float *pfAveragePrice = &vfAveragePrice[0];
	long  *pnBadIndex     = &vnBadIndex[0];


	BLPVector<float> vfMiddleBand(m_nNumPoints, HOLEF);
	BLPVector<float> vfUpperBand(m_nNumPoints, HOLEF);
	BLPVector<float> vfLowerBand(m_nNumPoints, HOLEF);

	// copy High, Low, and Closing prices to local buffers to avoid to call
	// ScreenForMissing multiple times on the original High, Low, Close buffers.
	memcpy(pfClose, pfCloseOld, (sizeof (float)) * m_nNumPoints);
	memcpy(pfHigh, pfHighOld, (sizeof (float)) * m_nNumPoints);
	memcpy(pfLow, pfLowOld, (sizeof (float)) * m_nNumPoints);

	float *pfData[3] = {pfClose, pfHigh, pfLow};
	long nNumBadPoints;

	// if any one of high, low, close is missing on a period, ScreenForMissing
	// will filter out that period
	ScreenForMissing(pfData, 3, m_nNumPoints, pnBadIndex, &nNumBadPoints);

	long nValidPoints = m_nNumPoints - nNumBadPoints;

	if (m_nBandPeriod > nValidPoints || m_nMiddlePeriod > nValidPoints)
		return E_INVALIDARG;

	// Average Price
	long nBar;
	for(nBar=0; nBar < nValidPoints; nBar++)
	{
		pfAveragePrice[nBar] = (pfClose[nBar] + pfHigh[nBar] + pfLow[nBar])/3.0;
	}

	// compute the bands
	for (nBar = 0; nBar < nValidPoints; nBar++) {
		if(nBar < (__max(m_nBandPeriod, m_nMiddlePeriod) - 1) )
			continue;
		// Band Moving Average
		long count;
		float sum=0;
		for (count = nBar; count > nBar - m_nBandPeriod; count--) {
			sum = sum + (pfHigh[count] - pfLow[count]);
		}
		float bandMovingAverage = sum/m_nBandPeriod;

		// Middle Band
		sum=0;
		for (count = nBar; count > nBar - m_nMiddlePeriod; count--) {
			sum = sum + pfAveragePrice[count] ;
		}
		vfMiddleBand[nBar] = sum/m_nMiddlePeriod;

		// Upper and Lower bands
		vfUpperBand[nBar] = vfMiddleBand[nBar] + bandMovingAverage*m_fUpperBandPercent;
		vfLowerBand[nBar] = vfMiddleBand[nBar] - bandMovingAverage*m_fLowerBandPercent;
	}

	MergeGoodValuesWithMissing(&vfMiddleBand[0], m_nNumPoints, pnBadIndex, nNumBadPoints);
	MergeGoodValuesWithMissing(&vfLowerBand[0], m_nNumPoints, pnBadIndex, nNumBadPoints);
	MergeGoodValuesWithMissing(&vfUpperBand[0], m_nNumPoints, pnBadIndex, nNumBadPoints);

	SetStudyDataLine(0, &vfMiddleBand[0]);
	SetStudyDataLine(1, &vfUpperBand[0]);
	SetStudyDataLine(2, &vfLowerBand[0]);


	return S_OK;
}


HRESULT CKeltnerBands::CreateStudy(IBLPGraphStudyList * pGraph)
{

	HRESULT hr = AddStudy(pGraph, 3, 3, m_pLineParameters, m_pPointParameters);

	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "KLTN", this);
		}

	return S_OK;
}
