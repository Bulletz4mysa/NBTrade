#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#include "blpvector.h"
#include <math.h>


HRESULT CTDPressure::ConvertBuf(void * pBigBuf)
{
	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;

	pHead->nPeriod     = ntohs(pHead->nPeriod);
	pHead->nROCPeriod  = ntohs(pHead->nROCPeriod);
	pHead->nFormulaRatio    = ntohs(pHead->nFormulaRatio); //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
	pHead->nUsePriceRatio   = ntohs(pHead->nUsePriceRatio); //enum tagEnumPrice {P, PV, V}
	
    int *ptemp = (int *)&(pHead->fGapPercent);
	*ptemp = ntohl(*ptemp);

	// boolean
	pHead->bDisplayPI            = pHead->bDisplayPI;
	pHead->bDisplayROCRatio      = pHead->bDisplayROCRatio;
	pHead->bDisplayPressureRatio = pHead->bDisplayPressureRatio;

	ConvertLineParameters(pHead->line_params, 3);
		
	m_nVersion = ntohs(pHead->nVersion);
	if (m_nVersion  >= 1) { // If this is the new BIG
		pHead->nRatioPeriod   = ntohs(pHead->nRatioPeriod);
		pHead->nOverbought    = ntohs(pHead->nOverbought);
		pHead->nOversold      = ntohs(pHead->nOversold);
		pHead->fStartDate     = ntohl(pHead->fStartDate);
		pHead->n_startBar     = ntohs(pHead->n_startBar);
		// Data and Formula to be use for Ratio ROC 
		pHead->nFormula[0]    = ntohs(pHead->nFormula[0]); //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
		pHead->nUsePrice[0]   = ntohs(pHead->nUsePrice[0]); //enum tagEnumPrice {P, PV, V}
		// Data and Formula to be use for Cum Index
		pHead->nFormula[1]    = ntohs(pHead->nFormula[1]); //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
		pHead->nUsePrice[1]   = ntohs(pHead->nUsePrice[1]); //enum tagEnumPrice {P, PV, V}
		if  (m_nVersion  >= 2) { // If this is the Version 2,Shi F 01/30/03
			pHead->nOverboughtROC       = ntohs(pHead->nOverboughtROC);
			pHead->nOversoldROC			= ntohs(pHead->nOversoldROC);
			pHead->nDurationBarsRatio   = ntohs(pHead->nDurationBarsRatio);
			pHead->nDurationBarsROC     = ntohs(pHead->nDurationBarsROC);
		}
	}
	else{ // using old big, default the values
		pHead->nRatioPeriod   = pHead->nROCPeriod;
		pHead->nOverbought    = HOLEF;
		pHead->nOversold      = HOLEF;
		pHead->fStartDate     = 19990101;
		pHead->n_startBar     = 1;
		pHead->nFormula[0]    = 5; //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
		pHead->nUsePrice[0]   = 0; //enum tagEnumPrice {P, PV, V}
		pHead->nFormula[1]    = 5; //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
		pHead->nUsePrice[1]   = 1; //enum tagEnumPrice {P, PV, V}
	}

	m_nPeriod      = pHead->nPeriod;
	m_nROCPeriod   = pHead->nROCPeriod;
	m_nRatioFormula     = (enum tagEnumFormula)pHead->nFormulaRatio;//enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
	m_nRatioUsePrice    = (enum tagEnumPrice)pHead->nUsePriceRatio;	//enum tagEnumPrice {P, PV, V}
	m_fGapPercent  = (float)pHead->fGapPercent * .01f;
	
	// boolean
	m_bDisplayPI            = pHead->bDisplayPI;
	m_bDisplayROCRatio      = pHead->bDisplayROCRatio;
	m_bDisplayPressureRatio = pHead->bDisplayPressureRatio;

	m_pLineParameters  = pHead->line_params;
	memcpy(&m_pPressureLineParameters[0],&pHead->line_params[0],sizeof(ChartLineParameters));
	memcpy(&m_pPressureLineParameters[1],&pHead->line_params[1],sizeof(ChartLineParameters));
	memcpy(&m_pPressureLineParameters[2],&pHead->line_params[2],sizeof(ChartLineParameters));

	m_nRatioPeriod     = pHead->nRatioPeriod;
	m_nOverbought      = pHead->nOverbought;
	m_nOversold        = pHead->nOversold;
	m_fStartDate       = pHead->fStartDate;
	m_nStartBar        = pHead->n_startBar;  

	m_nROCFormula      = (enum tagEnumFormula)pHead->nFormula[0]; 
	m_nIndexFormula    = (enum tagEnumFormula)pHead->nFormula[1];

	m_nROCUsePrice     = (enum tagEnumPrice)pHead->nUsePrice[0];
    m_nIndexUsePrice   = (enum tagEnumPrice)pHead->nUsePrice[1];

	// New variables for version 2,  Shi F
	if (m_nVersion  >= 2) {
		m_nOverboughtROC              = pHead->nOverboughtROC;
		m_nOversoldROC                = pHead->nOversoldROC;
		m_nDurationBarsRatio          = pHead->nDurationBarsRatio;
		m_nDurationBarsROC            = pHead->nDurationBarsROC;
		m_bLowRiskUpArrowRatio        = (pHead->cLowRiskUpArrowRatio == 'Y') ? TRUE : FALSE;
		m_bLowRiskDownArrowRatio      = (pHead->cLowRiskDownArrowRatio == 'Y') ? TRUE : FALSE;
		m_bLowRiskUpArrowROC          = (pHead->cLowRiskUpArrowROC == 'Y') ? TRUE : FALSE;
		m_bLowRiskDownArrowROC        = (pHead->cLowRiskDownArrowROC == 'Y') ? TRUE : FALSE;
		m_bConsecutiveUpArrowRatio    = (pHead->cConsecutiveUpArrowRatio == 'Y') ? TRUE : FALSE;
		m_bConsecutiveDownArrowRatio  = (pHead->cConsecutiveDownArrowRatio == 'Y') ? TRUE : FALSE;
		m_bDurationEnabledRatio       = (pHead->cDurationEnabledRatio == 'Y') ? TRUE : FALSE;
		m_bDurationEnabledROC         = (pHead->cDurationEnabledROC == 'Y' ) ? TRUE : FALSE;
	}
	return NOERROR;
} // End of ConvertBuf()



HRESULT CTDPressure::CreateStudy(IBLPGraphStudyList * pGraph)
{
	CChartLine *pChartLine = new CChartLine;
	m_pDurationTopLine     = new CChartLine;
	m_pDurationBottomLine  = new CChartLine;

	short nChartNum = 1;
//	DebugBreak();


	
    //Set up Arrows    Shi F 01/30/03
    ChartLineParameters TDPress_Arrow_LineParms[] = 
        {{1, XRT_LPAT_SOLID, blgGreen},        // Ratio Low Risk Up Arrow Color  
         {1, XRT_LPAT_SOLID, blgRed},          // Ratio Low Risk Down Arrow Color
         {1, XRT_LPAT_SOLID, blgMagenta},      // Ratio Consecutive Up Arrow Color
         {1, XRT_LPAT_SOLID, blgMagenta},      // Ratio Consecutive	Down Arrow Color
         {1, XRT_LPAT_SOLID, blgYellow},        // ROC Low Risk Up Arrow Color
		 {1, XRT_LPAT_SOLID, blgCyan}};         // ROC Low Risk Down Arrow Color

    ChartPointParameters TDPress_Arrow_PointParms[] =
        {{XRT_POINT_UPARROW,0,0},			   // Ratio Low Risk Up Arrow 	   
         {XRT_POINT_DOWNARROW,0,0},			   // Ratio Low Risk Down Arrow
         {XRT_POINT_UPARROW,0,0},			   // ROC Low Risk Up Arrow
         {XRT_POINT_DOWNARROW,0,0},			   // ROC Low Risk Down Arrow
		 {XRT_POINT_UPARROW,0,0},			   // Ratio Consecutive Up Arrow
		 {XRT_POINT_DOWNARROW,0,0}};		   // Ratio Consecutive	Down Arrow

    //Set up duration box parameters
    ChartLineParameters TDPress_Box_LineParms[] = 
  		{{1, XRT_TOP_FILL, blgMagenta},                 // Upper duration box
         {1, XRT_LPAT_SOLID, blgMagenta}};              // Lower duration box

    ChartPointParameters TDPress_Box_PointParms[] =    
        {{XRT_POINT_NONE,0,0},							// Upper duration box
         {XRT_POINT_NONE,0,0}};							// Lower duration box

    //Add 6 Arrow Lines, total 17
    HRESULT rc = AddStudy(pGraph, 17, 6, TDPress_Arrow_LineParms, TDPress_Arrow_PointParms);	
	
	
	if (FAILED(rc))
		return rc;

	if(m_bDisplayPressureRatio){

		pChartLine->SetLineParams(m_pPressureLineParameters[0]);
		m_vChartLine.push_back(pChartLine);

		m_zones.overbought = m_nOverbought;
		m_zones.oversold = m_nOversold;

		//Upper duration box
		m_pDurationTopLine->SetLineParams(TDPress_Box_LineParms[0]);
		m_pDurationTopLine->SetPointParams(TDPress_Box_PointParms[0]);
		m_vChartLine.push_back(m_pDurationTopLine);

		//Bottom duration box
		m_pDurationBottomLine->SetLineParams(TDPress_Box_LineParms[1]);
		m_pDurationBottomLine->SetPointParams(TDPress_Box_PointParms[1]);
		m_vChartLine.push_back(m_pDurationBottomLine);

		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, this, &m_pPRatio_Chart) == S_OK)
			AddChart(m_pPRatio_Chart);
		else
			return S_FALSE;

		}

	if(m_bDisplayROCRatio){

		m_zones.overbought = m_nOverboughtROC;
		m_zones.oversold = m_nOversoldROC;
		pChartLine->SetLineParams(m_pPressureLineParameters[1]);
		m_vChartLine.push_back(pChartLine);

		//Upper duration box
		m_pDurationTopLine->SetLineParams(TDPress_Box_LineParms[0]);
		m_pDurationTopLine->SetPointParams(TDPress_Box_PointParms[0]);
		m_vChartLine.push_back(m_pDurationTopLine);

		//Bottom duration box
		m_pDurationBottomLine->SetLineParams(TDPress_Box_LineParms[1]);
		m_pDurationBottomLine->SetPointParams(TDPress_Box_PointParms[1]);
		m_vChartLine.push_back(m_pDurationBottomLine);

		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, this, &m_pPROC_Chart) == S_OK)
			AddChart(m_pPROC_Chart);
		else
			return S_FALSE;
	}
	
	if(m_bDisplayPI){

		m_zones.overbought = HOLEF;
		m_zones.oversold = HOLEF;

		pChartLine->SetLineParams(m_pPressureLineParameters[2]);
		m_vChartLine.push_back(pChartLine);

		CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
		if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, nChartNum++, this, &m_pPI_Chart) == S_OK)
			AddChart(m_pPI_Chart);
		else
			return S_FALSE;
	}
	
	//Arrows
	if((m_bLowRiskUpArrowRatio&&m_bDisplayPressureRatio) || (m_bLowRiskDownArrowRatio&&m_bDisplayPressureRatio) || 
		(m_bConsecutiveUpArrowRatio&&m_bDisplayPressureRatio) || (m_bConsecutiveDownArrowRatio&&m_bDisplayPressureRatio) ||
		(m_bLowRiskUpArrowROC&&m_bDisplayROCRatio) || (m_bLowRiskDownArrowROC&&m_bDisplayROCRatio) )
	{
		if (pSD->QueryInterface(IID_IChart, (LPVOID *)&m_pArrow_Chart) == S_OK)
			AddChart(m_pArrow_Chart);
		else
			return S_FALSE;
	}

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	iGraphToolbar->AddStudyButton(0, m_strButtonText, this);

	return NOERROR;
} // End of CreateStudy()



HRESULT CTDPressure::DoStudy(IBLPGraphData *pGraph)
{
	if (m_vCharts.empty())
		return E_HANDLE;
	
	float *pfOpen, *pfHigh, *pfLow, *pfClose, *pfTrueHigh, *pfTrueLow, *pfVolume;

	pGraph->get_NumPoints(&m_nNumPoints);
	long nPoints = m_nNumPoints; // Get the Total Number of Points

	long FirstBar = 0, LastBar = nPoints -1;  
	// Get Price and Volume Data
	pGraph->get_CloseData(nPoints, &pfClose);
	pGraph->get_OpenData(nPoints, &pfOpen);
	pGraph->get_HighData(nPoints, &pfHigh);
	pGraph->get_LowData(nPoints, &pfLow);
	pGraph->get_TrueHighData(nPoints, &pfTrueHigh);
	pGraph->get_TrueLowData(nPoints, &pfTrueLow);
	pGraph->get_VolumeData(nPoints, &pfVolume);

	// Allocate Visible Data Arrays 
	BLPVector<float>	vPressureRatio(nPoints, HOLEF);
	BLPVector<float>	vROCRatio(nPoints, HOLEF);
	BLPVector<float>	vPressureIndex(nPoints, 0.0f);

	// set up the study line, for version 2, added by Shi F
	BLPVector<float>	vRatioDurationInside(nPoints, HOLEF);
    BLPVector<float>	vRatioDurationOutside(nPoints, HOLEF);
	BLPVector<float>	vROCDurationInside(nPoints, HOLEF);
    BLPVector<float>	vROCDurationOutside(nPoints, HOLEF);
    BLPVector<float>    vLowRiskUpArrowRatio(nPoints, HOLEF);
	BLPVector<float>    vLowRiskDownArrowRatio(nPoints, HOLEF);
	BLPVector<float>    vLowRiskUpArrowROC(nPoints, HOLEF);
	BLPVector<float>    vLowRiskDownArrowROC(nPoints, HOLEF);
	BLPVector<float>    vConsecutiveUpArrowRatio(nPoints, HOLEF);
	BLPVector<float>    vConsecutiveDownArrowRatio(nPoints, HOLEF);
	BLPVector<BOOL>     vIsRatioDurationTrue(nPoints, FALSE);
	BLPVector<BOOL>     vIsROCDurationTrue(nPoints, FALSE);
	

 
	float *pfPressureRatio             = &vPressureRatio[0] ; 
	float *pfROCRatio                  = &vROCRatio[0]; 
	float *pfPressureIndex             = &vPressureIndex[0];
    float *RatioDurationInside         = &vRatioDurationInside[0];
    float *RatioDurationOutside        = &vRatioDurationOutside[0];
	float *ROCDurationInside           = &vROCDurationInside[0];
    float *ROCDurationOutside          = &vROCDurationOutside[0];
    float *pfLowRiskUpArrowRatio       = &vLowRiskUpArrowRatio[0];
	float *pfLowRiskDownArrowRatio     = &vLowRiskDownArrowRatio[0];
	float *pfLowRiskUpArrowROC         = &vLowRiskUpArrowROC[0];
	float *pfLowRiskDownArrowROC       = &vLowRiskDownArrowROC[0];
	float *pfConsecutiveUpArrowRatio   = &vConsecutiveUpArrowRatio[0];
	float *pfConsecutiveDownArrowRatio = &vConsecutiveDownArrowRatio[0]; 
	BOOL  *IsRatioDurationFlag         = &vIsRatioDurationTrue[0];
	BOOL  *IsROCDurationFlag           = &vIsROCDurationTrue[0];
	

	
	//Calculate each data line for the panel
	if(m_bDisplayPressureRatio)
		CreatePressureRatio(
			pfOpen,
			pfHigh,
			pfLow,
			pfClose,
			pfTrueHigh,
			pfTrueLow,
			pfVolume, 
			nPoints, 
			pfPressureRatio);
	
	if(m_bDisplayROCRatio)
		CreateROCRatio(
			pfOpen,
			pfHigh,
			pfLow,
			pfClose,
			pfTrueHigh,
			pfTrueLow,
			pfVolume, 
			nPoints, 
			pfROCRatio);

	if(m_bDisplayPI)
		CreatePressureIndex(
			pfOpen,
			pfHigh,
			pfLow,
			pfClose,
			pfTrueHigh,
			pfTrueLow,
			pfVolume, 
			nPoints, 
			pfPressureIndex);
	

	// Set Up Charts
	// Pressure Ratio Chart, 1 data line and 2 oscillator lines(overbought and oversold)
	if(m_pPRatio_Chart != NULL){

		// Calculate Ratio Duration
		if(m_bDurationEnabledRatio == TRUE)
		   DurationCalc(
				pfPressureRatio,
				RatioDurationInside,
				RatioDurationOutside,
				m_nDurationBarsRatio,
				m_nPeriod,
				m_nOverbought,
				m_nOversold,
				FirstBar,
				LastBar,
				TDPressureDurationRatioCalc,
				IsRatioDurationFlag);
			
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pPRatio_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;


		// Copy the study lines onto the oscillator chart
		if(pfPressureRatio != NULL)
			iSD->put_StudyDataLine(0, nPoints, pfPressureRatio);
		// Copy the Duration Line onto the Osciallator Chart
		if(m_bDurationEnabledRatio == TRUE){
			if(RatioDurationInside != NULL)
				iSD->put_StudyDataLine(1, nPoints, RatioDurationInside);
			if(RatioDurationOutside != NULL)
				iSD->put_StudyDataLine(2, nPoints, RatioDurationOutside);
			}
  
	}

	//Pressure Ratio ROC Chart, 1 data line and 2 oscillator lines(overbought and oversold)
	if(m_pPROC_Chart != NULL){

		// Calculate ROC Duration
		if(m_bDurationEnabledROC == TRUE)
		   DurationCalc(
				pfROCRatio,
				ROCDurationInside,
				ROCDurationOutside,
				m_nDurationBarsROC,
				m_nROCPeriod,
				m_nOverboughtROC,
				m_nOversoldROC,
				FirstBar,
				LastBar,
				TDPressureDurationROCCalc,
				IsROCDurationFlag);
				
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pPROC_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;
		
		// Copy the study lines onto the Oscillator Chart
		if(pfROCRatio != NULL)
			iSD->put_StudyDataLine(0, nPoints, pfROCRatio);
		// Copy the Duration Line onto the Osciallator Chart
		if(m_bDurationEnabledROC == TRUE){
			if(ROCDurationInside != NULL)
				iSD->put_StudyDataLine(1, nPoints, ROCDurationInside);
			if(ROCDurationOutside != NULL)
				iSD->put_StudyDataLine(2, nPoints, ROCDurationOutside);	
			}

	}
	
	// Pressure Index Chart
	if(m_pPI_Chart != NULL){
		CComPtr<IStudyData> iSD;
		HRESULT hr = m_pPI_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;

		// Copy the study lines onto the Oscillator Chart
		if(pfPressureIndex != NULL)
			iSD->put_StudyDataLine(0, nPoints, pfPressureIndex);
	}


		//Calculate Arrows
	if (m_pArrow_Chart != NULL) {
		if((m_bLowRiskUpArrowRatio&&m_bDisplayPressureRatio) || (m_bLowRiskDownArrowRatio&&m_bDisplayPressureRatio) || 
		(m_bConsecutiveUpArrowRatio&&m_bDisplayPressureRatio) || (m_bConsecutiveDownArrowRatio&&m_bDisplayPressureRatio) ||
		(m_bLowRiskUpArrowROC&&m_bDisplayROCRatio) || (m_bLowRiskDownArrowROC&&m_bDisplayROCRatio) )
		
		{
			ArrowCalc(
				pfOpen, 
				pfHigh, 
				pfLow, 
				pfClose, 
				pfTrueHigh, 
				pfTrueLow, 
				pfPressureRatio,
				pfROCRatio,
				pfLowRiskUpArrowRatio, 
				pfLowRiskDownArrowRatio, 
				pfConsecutiveUpArrowRatio, 
				pfConsecutiveDownArrowRatio,
				pfLowRiskUpArrowROC, 
				pfLowRiskDownArrowROC,			
				m_nOverbought, 
				m_nOversold, 
				m_nOverboughtROC,
				m_nOversoldROC,
				FirstBar, 
				LastBar,
				IsRatioDurationFlag,
				IsROCDurationFlag);
				

			CComPtr<IStudyData> iSD;
			HRESULT hr = m_pArrow_Chart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
			ASSERT(SUCCEEDED(hr));
			if (FAILED(hr))
				return hr;

			// Copy the Arrow lines onto the graph
			if(pfLowRiskUpArrowRatio != NULL)
				if (m_bLowRiskUpArrowRatio&&m_bDisplayPressureRatio)
					iSD->put_StudyDataLine(0, nPoints, pfLowRiskUpArrowRatio);
			if(pfLowRiskDownArrowRatio != NULL)
				if (m_bLowRiskDownArrowRatio&&m_bDisplayPressureRatio)
					iSD->put_StudyDataLine(1, nPoints, pfLowRiskDownArrowRatio);
			if(pfConsecutiveUpArrowRatio != NULL)
				if (m_bConsecutiveUpArrowRatio&&m_bDisplayPressureRatio)
					iSD->put_StudyDataLine(2, nPoints, pfConsecutiveUpArrowRatio);
			if(pfConsecutiveDownArrowRatio != NULL)
				if (m_bConsecutiveDownArrowRatio&&m_bDisplayPressureRatio)
					iSD->put_StudyDataLine(3, nPoints, pfConsecutiveDownArrowRatio);
			if(pfLowRiskUpArrowROC !=NULL)
				if (m_bLowRiskUpArrowROC&&m_bDisplayROCRatio)
					iSD->put_StudyDataLine(4, nPoints, pfLowRiskUpArrowROC);
			if(pfLowRiskDownArrowROC !=NULL)
				if (m_bLowRiskDownArrowROC&&m_bDisplayROCRatio)
					iSD->put_StudyDataLine(5, nPoints, pfLowRiskDownArrowROC);
		
		}

	}
	return NOERROR;
} // End of DoStudy()




HRESULT CTDPressure::CreatePressureRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH,
									  float *TRL, float *VOL, long nPoints, float *pfPressureRatio)
{
	long i;

	// Working Data Arrays
	BLPVector<float>	vPriceRatio(nPoints, HOLEF);
	float *pfPriceRatio = &vPriceRatio[0]; 

	GetPriceRatio(OPEN,HIGH,LOW,CLOSE,TRH,TRL,VOL,nPoints,pfPriceRatio,
					m_nRatioFormula,m_nRatioUsePrice);

	
	// Calculate Pressure Ratio 
	for( i = m_nPeriod; i< nPoints ; i++){
		if(!IsInvalid(pfPriceRatio[i])) // checking to see if the first point is valid. If not skip to next point.
			pfPressureRatio[i] = GetnRatioBack(pfPriceRatio, i, m_nPeriod);
		else
			pfPressureRatio[i] = HOLEF;	
	}
	return NOERROR;
}  // End of CreatePressureRatio()



HRESULT CTDPressure::CreateROCRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH, 
								 float *TRL, float *VOL, long nPoints, float *pfROCRatio)
{

	long i;

	// Allocate Invisible Data Arrays
	BLPVector<float>	vPriceRatio(nPoints, HOLEF);
	BLPVector<float>	vRatioForROC(nPoints, HOLEF);

	float *pfPriceRatio = &vPriceRatio[0]; 
	float *pfRatioForROC = &vRatioForROC[0]; 

	GetPriceRatio(OPEN,HIGH,LOW,CLOSE,TRH,TRL,VOL,nPoints,pfPriceRatio,
					m_nROCFormula,m_nROCUsePrice);

	// Calculate Pressure Ratio ROC 
	for( i = m_nRatioPeriod; i< nPoints ; i++){
		if(!IsInvalid(pfPriceRatio[i])) // If the first point is not valid, skip to next point.
			pfRatioForROC[i] = GetnRatioBack(pfPriceRatio, i, m_nRatioPeriod);
		else
			pfRatioForROC[i] = HOLEF;
	}
	
	for(i = m_nROCPeriod + m_nRatioPeriod ; i<nPoints; i++){
		if (!IsInvalid(pfRatioForROC[i])  && !IsInvalid(pfRatioForROC[i - m_nROCPeriod]))
			pfROCRatio[i] = pfRatioForROC[i] * 100.0f/ pfRatioForROC[i - m_nROCPeriod];
		else
			pfROCRatio[i] = HOLEF;	
	}

		return NOERROR;
}  // End of CreateROCRatio()



HRESULT CTDPressure::CreatePressureIndex(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH, 
									  float *TRL, float *VOL, long nPoints, float *pfPressureIndex)
{
	long i;
	
	// Working Data Arrays
	BLPVector<float>	vPriceRatio(nPoints, HOLEF);
	float *pfPriceRatio = &vPriceRatio[0]; 

	GetPriceRatio(OPEN,HIGH,LOW,CLOSE,TRH,TRL,VOL,nPoints,pfPriceRatio,
					m_nIndexFormula,m_nIndexUsePrice);
	
	// Calculate Pressure Cummulative Index
	for(i = m_nStartBar ; i<nPoints ; i++){
		if(!IsInvalid(pfPriceRatio[i])){
			pfPressureIndex[i] = pfPressureIndex[i-1] + pfPriceRatio[i];
		}
		else
			pfPressureIndex[i] = pfPressureIndex[i-1];
	}
	pfPressureIndex[0]= HOLEF;
	return NOERROR;
} // End of CreatePressureIndex()


// Calculate ratio and ratioROC duration box 
void CTDPressure::DurationCalc(float *pfPressureRatioROC, float *RatioROCDurationInside, 
							   float *RatioROCDurationOutside, short nDurationBarsRatioROC, 
							   short nRatioROCPeriod, short nOverBought, short nOverSold,
							   long FirstBar, long LastBar, long RatioROC, BOOL *RatioROCFlag)
{

	if(pfPressureRatioROC == NULL) 
		return;

    long ii, UpperDurationCount = 0, LowerDurationCount = 0;
	float max = HOLEF, min = HOLEF;

	if (TDPressureDurationROCCalc == RatioROC)
		GetDataRange(LastBar+1, pfPressureRatioROC, &min, &max);

    ASSERT(nRatioROCPeriod >= 1);
    for (ii = nRatioROCPeriod - 1;ii <= LastBar; ii++)
    {
        //We will stop counting once we encounter invalid Pressure Ratio value
        if (IsInvalid(pfPressureRatioROC[ii]))
        {
            UpperDurationCount = 0;
            LowerDurationCount = 0;
            continue;
        }
        
        if (pfPressureRatioROC[ii] >= nOverBought)
        {
            ++UpperDurationCount;
            if (UpperDurationCount >= nDurationBarsRatioROC)
            {
				if (TDPressureDurationRatioCalc == RatioROC)
				{
					RatioROCDurationOutside[ii] = 100.0f;
					RatioROCDurationInside[ii]  = nOverBought;
				}
				else
				{
					RatioROCDurationOutside[ii] = max;
					RatioROCDurationInside[ii]  = nOverBought;
				}

				RatioROCFlag[ii] = TRUE;
            }
        }
        else {
            UpperDurationCount = 0;
        }

        if (pfPressureRatioROC[ii] <= nOverSold)
        {
            ++LowerDurationCount;
            if (LowerDurationCount >= nDurationBarsRatioROC)
            {
				if (TDPressureDurationRatioCalc == RatioROC)
				{
					RatioROCDurationOutside[ii] = nOverSold;
					RatioROCDurationInside[ii]  = 0.0f;
				}
				else
				{
					RatioROCDurationOutside[ii] = nOverSold;
					RatioROCDurationInside[ii]  = min;
				}

				RatioROCFlag[ii] =TRUE;
            }
        }
        else {
            LowerDurationCount = 0;
        }
           
    }


} //End of DurationCalc()


// calculate Arrowa for ratio and ratioROC
void CTDPressure::ArrowCalc(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRUEHIGH, float *TRUELOW, 
							float *pfPressureRatio, float *pfPressureROC, float *LowRiskUpArrowRatio, float *LowRiskDownArrowRatio, 
							float *ConsecutiveUpArrowRatio, float *ConsecutiveDownArrowRatio, float *LowRiskUpArrowROC, 
							float *LowRiskDownArrowROC, short OverBoughtRatio, short OverSoldRatio, short OverBoughtROC, 
							short OverSoldROC,long FirstBar, long LastBar, BOOL *RatioFlag, BOOL *ROCFlag)
{
	if((pfPressureRatio == NULL)&&(pfPressureROC == NULL))
		return;

    long nNumPoints = LastBar + 1, ii;
	long nOneBarBack;
    float LowOffset1, HighOffset1;
	float LowOffset2, HighOffset2;
	float LowOffset3, HighOffset3;
	float Max, Min;

	// get the maximum and minimum price
    GetDataRange(nNumPoints, HIGH, &HighOffset1, &Max);
    GetDataRange(nNumPoints, LOW, &Min, &LowOffset1);
	// for Ratio low risk arrows
    LowOffset1 = (Min - Max) * 0.02f;
    HighOffset1 = (Max - Min) * 0.02f;
	// for ratio consecutive arrows
	LowOffset2 = (Min - Max) * 0.035f;
    HighOffset2 = (Max - Min) * 0.035f;
	// for ROC low risk arrows
	LowOffset3 = (Min - Max) * 0.05f;
    HighOffset3 = (Max - Min) * 0.05f;

	for (ii = FirstBar; ii < nNumPoints; ii++) {
		if(!IsInvalid(pfPressureRatio[ii])) {  // skip invalid data
			
			nOneBarBack = GetnBarsBack(pfPressureRatio, FirstBar, ii, 1);

			if((nOneBarBack!=-1)&&(!RatioFlag[ii])&&(pfPressureRatio[ii] < OverBoughtRatio)&&(pfPressureRatio[nOneBarBack] >= OverBoughtRatio))
				LowRiskDownArrowRatio[ii] = HIGH[ii] + HighOffset1;

			if((nOneBarBack!=-1)&&(!RatioFlag[ii])&&(pfPressureRatio[ii] > OverSoldRatio)&&(pfPressureRatio[nOneBarBack] <= OverSoldRatio))	
				LowRiskUpArrowRatio[ii] = LOW[ii] + LowOffset1;

			if((fabs(100.0f - pfPressureRatio[ii])<DELTA)&&(!RatioFlag[ii])) //blpstudy.h: #define DELTA 1E-10
				ConsecutiveDownArrowRatio[ii] = HIGH[ii] + HighOffset2;

			if((fabs(0.0f - pfPressureRatio[ii])<DELTA)&&(!RatioFlag[ii]))
				ConsecutiveUpArrowRatio[ii] = LOW[ii] + LowOffset2;
		}


		if(!IsInvalid(pfPressureROC[ii])) {  // skip invalid data

			nOneBarBack = GetnBarsBack(pfPressureROC, FirstBar, ii, 1);

			if((nOneBarBack!=-1)&&(!ROCFlag[ii])&&(pfPressureROC[ii] < OverBoughtROC)&&(pfPressureROC[nOneBarBack] >= OverBoughtROC))
				LowRiskDownArrowROC[ii] = HIGH[ii] + HighOffset3;

			if((nOneBarBack!=-1)&&(!ROCFlag[ii])&&(pfPressureROC[ii] > OverSoldROC)&&(pfPressureROC[nOneBarBack] <= OverSoldROC))
				LowRiskUpArrowROC[ii] = LOW[ii] + LowOffset3;
		}

	}

}// End of ArrowCalc();

	

long CTDPressure::ValidOHLC(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH, float *TRL, short useFormula)
{
	if( (CLOSE == NULL) || (HIGH == NULL) || (LOW == NULL) ) // These are always used in the calculation
		return -1;

	switch(useFormula)
	{
		case COHL: // only OPEN data need to be checked
			if(OPEN==NULL)
				return -1;
			else
				return 0;

		case CO: // only OPEN data need to be checked
			if(OPEN==NULL)
				return -1;
			else
				return 0;

		case CpCtR:
			if((TRH==NULL) || (TRL==NULL))
				return -1;
			else
				return 0;

		case COtR:
			if((TRH==NULL) || (TRL==NULL))
				return -1;
			else
				return 0;
	}
	return 0;
}  // End of ValidOHLC()



// Go back N number of days to looks for M number of valid bars.
float CTDPressure::GetnRatioBack(float *DATA, long nStartBar, long nBarsBack)
{
    long  nCounter    = 0;
    long  nCurrentBar = nStartBar;
    float fTotalAccum = 0.0f; 
	float fBuyAccum = 0.0f; 
	float fRatioVal;

    if (nBarsBack < 0 || nStartBar == 0)
        return HOLEF;

    while ((nCounter < nBarsBack) && (nCurrentBar > 0))
    {
		if (!IsInvalid(DATA[nCurrentBar])){
            if(DATA[nCurrentBar]>0)
				fBuyAccum += DATA[nCurrentBar];
			fTotalAccum += (float)fabs(DATA[nCurrentBar]);
			nCounter++;
		}
		nCurrentBar--;
    }

    if (nCounter == nBarsBack){
		fRatioVal = fBuyAccum * 100.0f / fTotalAccum;
		return fRatioVal;
	}
    else
        return HOLEF;
} // End of GetnRatioBack()


//return the Bar number that is nBarsBack from nStartBar in DATA array or -1 if fails.
long CTDPressure::GetnBarsBack(float *DATA, long nFirstBar, long nStartBar, long nBarsBack)
{
    long        nCounter    = 0;
    long        nCurrentBar = nStartBar;
    
    if (nBarsBack < 0 || nStartBar == nFirstBar)
        return -1;
    
    while ((nCounter < nBarsBack) && (nCurrentBar > nFirstBar))
    {
        nCurrentBar--;
        if (!IsInvalid(DATA[nCurrentBar]))  // skip invalid data
            nCounter++;
    }

    if (nCounter == nBarsBack)
        return nCurrentBar;
    else
        return -1;
} //End of GetnBarsBack()


HRESULT CTDPressure::GetPriceRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH, 
								 float *TRL, float *VOL, long nPoints, float *pfWorkArray,
								 short Formula, short Price)
{
	long i, rcode;

		// Error Checking - if there is no Data return 
	rcode = ValidOHLC(OPEN,HIGH,LOW,CLOSE,TRH,TRL,Formula);
	if(rcode == -1)
		return E_HANDLE;	
	
	if((Price==PV) || (Price==V))
	{
		if(VOL == NULL)
			return E_HANDLE;
	}

	// Calculate PriceRatio, based on the formula and data for calculation
	for ( i = 1; i < nPoints; i++) {
		switch (Formula) {
			case COHL:
				if(!IsInvalid(HIGH[i]) && !IsInvalid(LOW[i]) && !IsInvalid(CLOSE[i]) &&
					!IsInvalid(OPEN[i]) ){
					if (HIGH[i] == LOW[i])
						pfWorkArray[i] = 0.0f;
					else
						pfWorkArray[i] = (CLOSE[i] - OPEN[i]) / (HIGH[i] - LOW[i]);
				}
				else
					pfWorkArray[i] = HOLEF;
				break;

			case CpC:
				if(!IsInvalid(CLOSE[i]) && !IsInvalid(CLOSE[i-1]))
					pfWorkArray[i] = CLOSE[i] - CLOSE[i - 1];
				else
					pfWorkArray[i] = HOLEF;
				break;

			case CO:
				if(!IsInvalid(CLOSE[i]) && !IsInvalid(OPEN[i]))
					pfWorkArray[i] = CLOSE[i] - OPEN[i];
				else
					pfWorkArray[i] = HOLEF;
				break;

			case CpCHL:
				if(!IsInvalid(HIGH[i]) && !IsInvalid(LOW[i]) && !IsInvalid(CLOSE[i]) &&
					!IsInvalid(CLOSE[i-1]) ){
					if (HIGH[i] == LOW[i])
						pfWorkArray[i] = 0.0f;
					else
						pfWorkArray[i] = (CLOSE[i] - CLOSE[i - 1]) / (HIGH[i] - LOW[i]);
				}
				else
					pfWorkArray[i] = HOLEF;
				break;

			case CpCtR:
				if(!IsInvalid(TRH[i]) && !IsInvalid(TRL[i]) && !IsInvalid(CLOSE[i]) &&
					!IsInvalid(CLOSE[i-1]) ){
					if (TRH[i] == TRL[i])
						pfWorkArray[i] = 0.0f;
					else
						pfWorkArray[i] = (CLOSE[i] - CLOSE[i - 1]) / (TRH[i] - TRL[i]);
				}
				else
					pfWorkArray[i] = HOLEF;
				break;

			case COtR:
				if(!IsInvalid(TRH[i]) && !IsInvalid(TRL[i]) && !IsInvalid(CLOSE[i]) &&
					!IsInvalid(OPEN[i]) ){
					if (TRH[i] == TRL[i])
						pfWorkArray[i] = 0.0f;
					else
						pfWorkArray[i] = (CLOSE[i] - OPEN[i]) / (TRH[i] - TRL[i]);
				}
				else
					pfWorkArray[i] = HOLEF;
				break;
		}


		// check for opening gap, use alternative formula
		if(!IsInvalid(CLOSE[i - 1]) && !IsInvalid(HIGH[i]) && 
			!IsInvalid(CLOSE[i]) && !IsInvalid(LOW[i]) 
			&& !IsInvalid(OPEN[i])){
	
			float fGapUp = (CBLPPrice)CLOSE[i - 1] + (CLOSE[i - 1] * m_fGapPercent);
			float fGapDn = (CBLPPrice)CLOSE[i - 1] - (CLOSE[i - 1] * m_fGapPercent);
      
			if ((CBLPPrice)OPEN[i] > fGapUp) {	
				if ((CLOSE[i-1] - LOW[i]) > 0 ) { 
					if ( HIGH[i]!=LOW[i] )
						pfWorkArray[i] = ((HIGH[i] - CLOSE[i - 1]) + (CLOSE[i] - LOW[i])) / (HIGH[i] - CLOSE[i-1] + CLOSE[i] - LOW[i] + HIGH[i] - CLOSE[i]  + CLOSE[i-1] - LOW[i]);
				} 
				else {	
					if ( 0!=(HIGH[i] - CLOSE[i-1] + HIGH[i] - LOW[i] ))
						pfWorkArray[i] = ((HIGH[i] - CLOSE[i - 1]) + (CLOSE[i] - LOW[i])) / (HIGH[i] - CLOSE[i-1] + CLOSE[i] - LOW[i] + HIGH[i] - CLOSE[i]);
					}
			}	
      
			if ((CBLPPrice)OPEN[i] < fGapDn) { 
				if ((HIGH[i] - CLOSE[i-1]) > 0) { 
					if ( HIGH[i]!=LOW[i] )
						pfWorkArray[i] = ((CLOSE[i - 1] - LOW[i]) + (HIGH[i] - CLOSE[i])) / (CLOSE[i-1] - LOW[i] + HIGH[i] - CLOSE[i] + CLOSE[i] - LOW[i] + HIGH[i] - CLOSE[i-1]);
				}
				else {
					if (0!=(CLOSE[i-1] - LOW[i] + HIGH[i]  - LOW[i]))
					pfWorkArray[i] = ((CLOSE[i - 1] - LOW[i]) + (HIGH[i] - CLOSE[i])) / (CLOSE[i-1] - LOW[i] + HIGH[i] - CLOSE[i] + CLOSE[i] - LOW[i]);
				}
			} 
		}
		
		//	enum tagEnumPrice {P, PV, V} m_nUsePrice;
		if (VOL)
		switch (Price) {
			case P:
				break;

			case PV:
					if(!IsInvalid(pfWorkArray[i]) && !IsInvalid(VOL[i]))
					pfWorkArray[i] = pfWorkArray[i] * VOL[i];
				else
					pfWorkArray[i] = HOLEF;
				break;

			case V:
				if(!IsInvalid(pfWorkArray[i]) && !IsInvalid(VOL[i])){
					if (pfWorkArray[i] > 0)
						pfWorkArray[i] = VOL[i];
					else if (pfWorkArray[i] < 0)
						pfWorkArray[i] = -VOL[i];
					else
						pfWorkArray[i] = 0.0f;
				}
				else
					pfWorkArray[i] = HOLEF;
				break;
			} 
	}// end of PriceRatio Calc
	return NOERROR;
} // End of GetPriceRatio()
