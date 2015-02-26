#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

/***********************************************************************************
 ************************  TD Relative Retracement Up/Down ************************* 
 ***********************************************************************************/

HRESULT CTDRelativeRetracement::DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph)
{
	ITrendlineManager *iTrendlineMgr;
	HRESULT hr = pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineMgr);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;
	 
	pGraph->get_NumPoints(&m_nNumPoints);
	long	N = m_nNumPoints;

	float *pfHigh = GetStudyDataLine(pGraph, High);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;
	
	float *pfLow = GetStudyDataLine(pGraph, Low);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;
	
	float *pfProjectionPrice = GetStudyDataLine(pGraph, m_nProjectionPrice);
	ASSERT(pfProjectionPrice != NULL);
	if (pfProjectionPrice == NULL)
		return E_HANDLE;
	
	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;
	
	float *pfOpen = GetStudyDataLine(pGraph, Open);
	ASSERT(pfOpen != NULL);
	if (pfOpen == NULL)
		return E_HANDLE;
	
	int i, nMagnetBar;
	BLPVector<float> vRetracement(m_nNumPoints, HOLEF);
	float *Retracement = &vRetracement[0];

	CBLPPrice fHigh, fLow, fOpen,fMagnet;
	float fProjectionPrice = pfProjectionPrice[x_value];
	
	float fRatios[] = { m_fPrecentRetracement[0], 
		m_fPrecentRetracement[1], 
		m_fPrecentRetracement[2], 
		m_fPrecentRetracement[3], 
		m_fPrecentRetracement[4], 
		m_fPrecentRetracement[5]};
	
	long *pnRetracementId = &m_nRetracementIds[0];
	
	long nLookback = 0;//Looking back as far as data is available.
	
	if (m_LineParms[0].line_width == -1) {
		ChartLineParameters DownLineParms[2] =  {
			{1, XRT_LPAT_SOLID, RGB(255, 0, 0)},
			{1, XRT_LPAT_LONG_DASH, RGB(0xF0, 0x64, 0)}
		};
		ChartLineParameters UpLineParms[2] =  {
			{1, XRT_LPAT_SOLID, RGB(0, 255, 0)},
			{1, XRT_LPAT_LONG_DASH, RGB(0xF0, 0x64, 0)}
		};
		memcpy(m_LineParms, m_bDirection ? DownLineParms : UpLineParms, sizeof(m_LineParms));
	}

	if (!bDirection) {
		for (i = x_value; i >= nLookback; i--) {
			if ((CBLPPrice)pfHigh[i] > fHigh ) {
				fHigh = pfHigh[i];
				fOpen = pfOpen[i];
				fMagnet = pfClose[i];
				nMagnetBar = i;
			}
			
			if ((CBLPPrice)pfLow[i] < pfLow[x_value])
				break;
		}
	}
	else {
		for (i = x_value; i >= nLookback; i--) {
			if ((CBLPPrice)pfLow[i] < fLow ) {
				fLow = pfLow[i];
				fOpen = pfOpen[i];
				fMagnet = pfClose[i];
				nMagnetBar = i;
			}
			
			if ((CBLPPrice)pfHigh[i] > pfHigh[x_value])
				break;
		}
	}
	
	float fRetracement;
	float fRetracement_stored[sizeof(fRatios)/sizeof(float)];
	int nLevel, nIntersectBar;
	
	for (nLevel = 0; nLevel <  sizeof(fRatios)/sizeof(float); nLevel++) {
		fRetracement = bDirection ? fProjectionPrice + (fLow - fProjectionPrice) * fRatios[nLevel] :								 
									fProjectionPrice + (fHigh - fProjectionPrice) * fRatios[nLevel]; 
		fRetracement_stored[nLevel] = fRetracement;
	for (nIntersectBar = x_value + 1; nIntersectBar < N; nIntersectBar++) {
		if (bDirection) {
			if ((CBLPPrice)pfLow[nIntersectBar] < fRetracement)
				break;
		}
		else
			if ((CBLPPrice)pfHigh[nIntersectBar] > fRetracement)
				break;
	}
	
	if (nIntersectBar == N) {
		nIntersectBar = N - 1;
		m_LineParms[0].line_style = XRT_LPAT_SOLID;
	}
	else
		m_LineParms[0].line_style = QualifyBreakout(pGraph, this, !bDirection, nIntersectBar, fRetracement) ?  XRT_LPAT_SOLID : XRT_LPAT_LONG_DASH;
	
	if(m_bToggle[nLevel])
		iTrendlineMgr->AddStudyTrendLine(x_value, fRetracement, nIntersectBar, fRatios[nLevel], 
		m_LineParms, line_type_study_retracement, pnRetracementId++);
	}
	
	for (nIntersectBar = x_value + 1; nIntersectBar < N - 1; nIntersectBar++) {
		if (bDirection) {
			if ((CBLPPrice)pfLow[nIntersectBar] < fMagnet)
				break;
		}
		else
			if ((CBLPPrice)pfHigh[nIntersectBar] > fMagnet)
				break;
	}
	
	if (nIntersectBar == N) {
		nIntersectBar = N - 1;
		m_LineParms[1].line_style = DEMARK_MAGNET_QUALIFIED;
	}
	else
		m_LineParms[1].line_style = QualifyBreakout(pGraph, this, !bDirection, nIntersectBar, fMagnet) ?  DEMARK_MAGNET_QUALIFIED : DEMARK_MAGNET_DISQUALIFIED;
	
	if (m_bDisplayMagnet)
		iTrendlineMgr->AddStudyTrendLine(x_value/*nMagnetBar*/, fMagnet, nIntersectBar/*N - 1*/, fMagnet, m_LineParms + 1, //6
		line_type_study_retracement, &m_nMagnetId);
	
	iTrendlineMgr->UpdateTrendLines();
	iTrendlineMgr->Release();
	m_bSaveTrendline = FALSE;

	//Determing which/where to put arrows
	m_nNumStudyLines = 1;
	if(bDirection){ //Relative Down
		if(m_nNum_retracements == 2){
			if(fOpen>fRetracement_stored[0] && fOpen>fRetracement_stored[1])//opens above 2 levels
				if(fLow<=fRetracement_stored[0] && fLow<=fRetracement_stored[1]){//trades below 2 levels
					Retracement[m_nDate] = fLow; //Place Arrow below price bar
					SetStudyDataLine(0, Retracement);
				}
			}
		else {
			if(fOpen>fRetracement_stored[0] && fOpen>fRetracement_stored[1] && fOpen>fMagnet)
				if(fLow<=fRetracement_stored[0] && fLow<=fRetracement_stored[1] && fLow<=fMagnet){//trades below 2 levels
					Retracement[m_nDate] = fLow; //Place Arrow below price bar
					SetStudyDataLine(0, Retracement);
				}
		}
	}
	else { //Relative Up
		if(m_nNum_retracements == 2){
			if(fOpen<fRetracement_stored[0] && fOpen<fRetracement_stored[1])//opens above 2 levels
				if(fHigh>=fRetracement_stored[0] && fHigh>=fRetracement_stored[1]){//trades below 2 levels
					Retracement[m_nDate] = fHigh; //Place Arrow below price bar
					SetStudyDataLine(0, Retracement);
				}
			}
		else {
			if(fOpen<fRetracement_stored[0] && fOpen<fRetracement_stored[1] && fOpen<fMagnet)
				if(fHigh>=fRetracement_stored[0] && fHigh>=fRetracement_stored[1] && fHigh>=fMagnet){//trades below 2 levels
					Retracement[m_nDate] = fHigh; //Place Arrow below price bar
					SetStudyDataLine(0, Retracement);
				}
		}
		
	}

	return NOERROR;
}

HRESULT CTDRelativeRetracement::ConvertBuf(void * pBigBuf)
{	
	int i;
	long *pConvertFloat;
	
	CTDRetracement::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;
	
	pHead->nProjectionPrice = (enumPriceType)ntohl(pHead->nProjectionPrice);
	pHead->nMaxLookback = ntohl(pHead->nMaxLookback);
	pHead->nPenetration = ntohs(pHead->nPenetration);
	pHead->nVersion = ntohl(pHead->nVersion);
	
	ConvertLineParameters(pHead->line_params, 2);
	
	for(i=0;i<6;i++){
		pConvertFloat  = (long *)&(pHead->fPrecent_retracement[i]);
		*pConvertFloat = ntohl(*pConvertFloat);
		pHead->fPrecent_retracement[i] = *((float *)pConvertFloat);
	}
	
//	m_bSaveTrendline = pHead->bSave;
	m_bDirection = pHead->bDirection;
	m_nProjectionPrice = pHead->nProjectionPrice;
	m_nMaxLookback = pHead->nMaxLookback;
	m_nPenetration = pHead->nPenetration;
	m_bQualifier1 = pHead->bQualifier1;
	m_bQualifier2 = pHead->bQualifier2;
	m_bQualifier3 = pHead->bQualifier3;
	m_bCancel1 = pHead->bCancel1;
	m_bCancel2 = pHead->bCancel2;
	m_bCancel3 = pHead->bCancel3;
	m_bDisplayProjections = pHead->bDisplayProjections;
	m_bDisplayMagnet = pHead->bDisplayMagnet;
	m_bDisplayAdditional = pHead->bDisplayAdditional;
	m_nNum_retracements = pHead->nNum_retracements;
	m_nNumStudyLines = 1;

	m_strButtonText = pHead->zButtonText;
	m_strErrorText = pHead->zErrorText;
	memcpy(m_LineParms, pHead->line_params, sizeof(m_LineParms));

	//Assign arrows
	if(m_bDirection) {//Relative Down, Put green arrow pointing up
		ChartLineParameters ArrowLineParms[1] =
		{{1, XRT_LPAT_SOLID, RGB(0, 255, 0)}};
		memcpy(m_ArrowLineParms, ArrowLineParms, sizeof(m_ArrowLineParms));
		m_pLineParameters = m_ArrowLineParms;

		ChartPointParameters ArrowPointParms[1] =
		{{XRT_POINT_UPARROW,0,0}};
		memcpy(m_ArrowPointParms, ArrowPointParms, sizeof(m_ArrowLineParms));
		m_pPointParameters = m_ArrowPointParms;
	}
	else {//Relative Up, Put red arrow pointing down
		ChartLineParameters ArrowLineParms[1] =
		{{1, XRT_LPAT_SOLID, RGB(255, 0, 0)}};
		memcpy(m_ArrowLineParms, ArrowLineParms, sizeof(m_ArrowLineParms));
		m_pLineParameters = m_ArrowLineParms;

		ChartPointParameters ArrowPointParms[1] =
		{{XRT_POINT_DOWNARROW,0,0}};
		memcpy(m_ArrowPointParms, ArrowPointParms, sizeof(m_ArrowLineParms));
		m_pPointParameters = m_ArrowPointParms;
	}

	if(pHead->nVersion == 0){
		//Set defaults
		m_fPrecentRetracement[0] = 0.381f;
		m_fPrecentRetracement[1] = 0.618f;
		m_fPrecentRetracement[2] = 1.3812f;
		m_fPrecentRetracement[3] = 1.618f;
		m_fPrecentRetracement[4] = 2.236f;
		m_fPrecentRetracement[5] = 2.618f;
		for(i=0;i<6;i++){
			m_bToggle[i] = 1;
		}
	}
	else{
		for(i=0;i<6;i++){
			m_fPrecentRetracement[i] = pHead->fPrecent_retracement[i];
			m_bToggle[i] = pHead->bToggle[i];
		} 
	}
	return NOERROR;
}


/***********************************************************************************
 ************************  TD Absolute Retracement Up/Down ************************* 
 ***********************************************************************************/

HRESULT CTDAbsoluteRetracement::DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph)
{
	ITrendlineManager *iTrendlineMgr;
	HRESULT hr = pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineMgr);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;
	
	long	N;
	pGraph->get_NumPoints(&N);
	
	float *pfHigh = GetStudyDataLine(pGraph, High);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;
	
	float *pfLow = GetStudyDataLine(pGraph, Low);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;
	
	float *pfProjectionPrice = GetStudyDataLine(pGraph, m_nProjectionPrice);
	ASSERT(pfProjectionPrice != NULL);
	if (pfProjectionPrice == NULL)
		return E_HANDLE;
	
	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;
	
	float *pfOpen = GetStudyDataLine(pGraph, Open);
	ASSERT(pfOpen != NULL);
	if (pfOpen == NULL)
		return E_HANDLE;
	
	float fProjectionPrice = pfProjectionPrice[x_value];
	
	float fRatios[7];
	if(m_bDirection){ /*Absolute Down*/
		fRatios[0] = m_fPrecentRetracement[0];
		fRatios[1] = m_fPrecentRetracement[1];
		fRatios[2] = 0.0f;
		fRatios[3] = 0.0f;
		fRatios[4] = 0.0f;
		fRatios[5] = 0.0f;
		fRatios[6] = 0.0f;
	}
	else{ /*Absolute Up*/
		fRatios[0] = 0.0f;
		fRatios[1] = 0.0f;
		fRatios[2] = m_fPrecentRetracement[0]; 
		fRatios[3] = m_fPrecentRetracement[1]; 
		fRatios[4] = m_fPrecentRetracement[2]; 
		fRatios[5] = m_fPrecentRetracement[3]; 
		fRatios[6] = m_fPrecentRetracement[4];
	}
	long *pnRetracementId = &m_nRetracementIds[0];
	
	float fRetracement;
	int nLevel, nIntersectBar;
	
	for (nLevel = 0; nLevel < (bDirection ? 2 : 5); nLevel++) {
		fRetracement=bDirection ? fProjectionPrice*fRatios[nLevel]:fProjectionPrice*fRatios[nLevel+2];
		
		for (nIntersectBar = x_value + 1; nIntersectBar < N; nIntersectBar++) {
			if (bDirection) {
				if ((CBLPPrice)pfLow[nIntersectBar] < fRetracement)
					break;
			}
			else {
				if ((CBLPPrice)pfHigh[nIntersectBar] > fRetracement)
					break;
			}
		}
		if (nIntersectBar == N) {
			nIntersectBar = N - 1;
			m_LineParms.line_style = XRT_LPAT_SOLID;
		}
		else
			m_LineParms.line_style = QualifyBreakout(pGraph, this, !bDirection, nIntersectBar, fRetracement) ?  XRT_LPAT_SOLID : XRT_LPAT_LONG_DASH;
		
		if(m_bToggle[nLevel]) /*If the toggle bit is ON, display line*/
			iTrendlineMgr->AddStudyTrendLine(x_value, fRetracement, nIntersectBar, (bDirection ? fRatios[nLevel] : fRatios[nLevel + 2]), &m_LineParms, //6
			line_type_study_retracement, pnRetracementId++);
	}
	
	iTrendlineMgr->UpdateTrendLines();
	iTrendlineMgr->Release();
	return NOERROR;
}

HRESULT CTDAbsoluteRetracement::ConvertBuf(void * pBigBuf)
{		
	CTDRetracement::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;	
	int i,counter;
	long *pConvertFloat;
	
	pHead->nProjectionPrice = (enumPriceType)ntohl(pHead->nProjectionPrice);
	ConvertLineParameters(&pHead->line_params, 1);
	
	if(pHead->bDirection)counter=2; /*Absolute Down*/
	else counter = 5;               /*Absolute Up*/
	for(i=0;i<counter;i++){
		pConvertFloat  = (long *)&(pHead->fPrecent_retracement[i]);
		*pConvertFloat = ntohl(*pConvertFloat);
		pHead->fPrecent_retracement[i] = *((float *)pConvertFloat);
	}
	
//	m_bSaveTrendline = pHead->bSave;
	m_bDirection = pHead->bDirection;
	m_nProjectionPrice = pHead->nProjectionPrice;
	m_bQualifier1 = pHead->bQualifier1;
	m_bQualifier2 = pHead->bQualifier2;
	m_bQualifier3 = pHead->bQualifier3;
	m_bCancel1 = pHead->bCancel1;
	m_bCancel2 = pHead->bCancel2;
	m_bCancel3 = pHead->bCancel3;
	m_bDisplayProjections = pHead->bDisplayProjections;
	
	m_strButtonText = pHead->zButtonText;
	m_strErrorText = pHead->zErrorText;
	
	if(pHead->nVersion == 0){
		if(m_bDirection){ //Retrace down
			m_fPrecentRetracement[0] = 0.382f;
			m_fPrecentRetracement[1] = 0.618f;
		}
		else{ //Retrace up
			m_fPrecentRetracement[0] = 1.382f;
			m_fPrecentRetracement[1] = 1.618f;
			m_fPrecentRetracement[2] = 2.618f;
			m_fPrecentRetracement[3] = 3.618f;
			m_fPrecentRetracement[4] = 4.618f;
		}
		for(i=0;i<5;i++)
			m_bToggle[i] = 1;
	}
	else{
		for(i=0;i<counter;i++){
			m_fPrecentRetracement[i] = pHead->fPrecent_retracement[i];
			m_bToggle[i] = pHead->bToggle[i];
		} 
	}
	memcpy(&m_LineParms, &pHead->line_params, sizeof(m_LineParms));
	return NOERROR;
}
