#include <stdafx.h>
#include "blpstudydlltd.h"
#include "blpstudy.h"
#include "demarkstudies.h"
#include "CTDAlignment.h"

CBLPStudy2 *CStudyDllTD::CreateStudyInstance(unsigned long nMsgId)
{
	CBLPStudy2 *pStudy;

	switch (nMsgId)
	{			
// TD Studies
			
		case CStudyDllTD::blgStudyDllTDCamouflage:	//1301
			pStudy = new CStudyDllTDCamouflage;
			break;
		case CStudyDllTD::blgStudyDllTDDifferential: //1302
			pStudy = new CTDDifferential;
			break;
		case CStudyDllTD::blgStudyDllTDReverseDifferential: //1303
			pStudy = new CTDReverseDifferential;
			break;
		case CStudyDllTD::blgStudyDllTDAntiDifferential: //1304
			pStudy = new CTDAntiDifferential;
			break;
		case CStudyDllTD::blgStudyDllTDMAVEX: //1305
			pStudy = new CTDMav1ex;
			break;
		case CStudyDllTD::blgStudyDllTDROCStudy: //1306
			pStudy = new CTDROC;
			break;
		case CStudyDllTD::blgTDAlignment:
			pStudy = new CTDAlignment;
			break;
		default:
			return NULL;
	}
	return pStudy;
}



/****** STUDIES ************/


/*** TD Camouflage */


HRESULT CStudyDllTDCamouflage::ConvertBuf(void * pBuf)
{
	int i;
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr * pData = (StudyMessagePtr *)pBuf;
  	
	pData->showback=ntohl(pData->showback);
	pData->qualifier[0]=ntohs(pData->qualifier[0]);
	pData->qualifier[1]=ntohs(pData->qualifier[1]);
	pData->version = ntohs(pData->version);

	m_Qualifier[0] = pData->qualifier[0];
	m_Qualifier[1] = pData->qualifier[1];
	m_ShowBack = pData->showback;
	m_version = pData->version;

	if (m_version == 0) {
		// N.A to version 0, so set to default
		memset(m_signal_on,' ',4);
		memset(m_signal_on,'X',2);
		memset(m_condition4_on,' ',2);
		memset(m_condition5_on,' ',2);
        m_color[0] = blgGreen;
		m_color[1] = blgRed;
		m_color[2] = blgMagenta;
		m_color[3] = blgMagenta;
		return NOERROR;
	} else {
		memcpy(m_signal_on,pData->signal_on,4);
		memcpy(m_condition4_on,pData->condition4_on,2);
		memcpy(m_condition5_on,pData->condition5_on,2);
		for (i=0; i<4; i++) {
			pData->color[i] = ntohl(pData->color[i]);
			m_color[i] = pData->color[i];
		}
	}
	return NOERROR;
}

//This DoStudy did all the calculation.
//In the future we are going to use BLPVector to replace new
HRESULT CStudyDllTDCamouflage::DoStudy(IBLPGraphData *pGraph)
{
	int i,count=0,index,start;
	short q1=0,q2=0;  //inclusive vs. exclusive logic
	short c4[2],c5[2],c4ok=1,c5ok=1; //condition4 and 5
	float min,max,temp,CurrOpen;

	if(m_Qualifier[0]==1) q1=1;
	if(m_Qualifier[1]==1) q2=1;

	for (i=0;i<2;i++) {
		c4[i]=c5[i]=0;
		if (m_condition4_on[i]!=' ') c4[i]=1;
		if (m_condition5_on[i]!=' ') c5[i]=1;
	}
	
	pGraph->get_NumPoints(&m_nNumPoints); //m_GraphData.number_of_points;
	
	float *pfOpen = GetStudyDataLine(pGraph, Open);
	float *pfClose = GetStudyDataLine(pGraph, Close);
	float *pfHigh = GetStudyDataLine(pGraph, High);
	float *pfLow = GetStudyDataLine(pGraph, Low);
	float *pfTrueHigh = GetStudyDataLine(pGraph, TrueHigh);
	float *pfTrueLow = GetStudyDataLine(pGraph, TrueLow);



	if (m_nNumPoints < 3) 
		return E_HANDLE;			// not enough data points to do anything

	GetDataRange(m_nNumPoints,pfHigh,&temp,&max);
	GetDataRange(m_nNumPoints,pfLow,&min,&temp);

	temp=max-min;
	temp=temp*(float)0.02; // offset for where to place the arrow
	float *pfTDCamSell = new float[m_nNumPoints];
	float *pfTDCamBuy = new float[m_nNumPoints];
	float *pfTDCamSellReverse = new float[m_nNumPoints];
	float *pfTDCamBuyReverse = new float[m_nNumPoints];
	float *pfTemp = new float[m_nNumPoints];
	
	// Initialize TD lines
	for(i=0;i<m_nNumPoints;i++) {
		if(IsInvalid(pfClose[i],pfOpen[i],pfLow[i],pfHigh[i])) {
			pfTemp[i]=HOLEF;
		}
		else pfTemp[i]=0;
		pfTDCamSell[i]=pfTDCamBuy[i]=pfTDCamSellReverse[i]=pfTDCamBuyReverse[i]=HOLEF;
	}

	// Find first valid data point	
	if(m_ShowBack<=0) {
		for(i=0;i<m_nNumPoints && count<2;i++) {
			if(!IsInvalid(pfTemp[i]))
				count++;
		}
		start=i;
	}
	else start=m_nNumPoints-m_ShowBack;

	if(start<2) start=2;


	// Calculate study
	for(i=start;i<m_nNumPoints;i++) {
		if(IsInvalid(pfTemp[i])) continue;
		CurrOpen=pfOpen[i];
		count=0; index=i;
		while(count<1 && index>=0) {   //Find Open and Close one day ago
			index--;
			if(!IsInvalid(pfTemp[index]))
				count++;
		}
		if(count<1) continue;
		float PrevCloseBar1=pfClose[index];
		float PrevOpenBar1=pfOpen[index];
		float PrevTrueLowBar1=pfTrueLow[index];
		float PrevTrueHighBar1=pfTrueHigh[index];

		while(count<2 && index>=0) {   //Find TrueHigh and TrueLow two days ago
			index--;
			if(!IsInvalid(pfTemp[index]))
				count++;
		}
		if(count<2) continue;
		float PrevTrueHighBar2=pfTrueHigh[index];
		float PrevTrueLowBar2=pfTrueLow[index];
		
		//Check for Camouflage buy
		if (m_signal_on[0]!=' ') {
			c4ok=1,c5ok=1; 
			if(pfClose[i]>CurrOpen || (pfClose[i]==CurrOpen && q1)) {
				if(pfClose[i]<PrevCloseBar1 || (pfClose[i]==PrevCloseBar1 && q2)) {
					if(pfLow[i]<PrevTrueLowBar2) {					// add condition4/5 here
						if (c4[0] && PrevCloseBar1<=PrevOpenBar1) c4ok=0;
						if (c5[0] && pfHigh[i]>=PrevTrueHighBar2) c5ok=0;
						if (c4ok && c5ok) pfTDCamBuy[i]=pfLow[i]-temp;
					}
				}
			}
		}

		//Check for Camouflage sell
		if (m_signal_on[1]!=' ') {
			c4ok=1,c5ok=1; 
			if(pfClose[i]<CurrOpen || (pfClose[i]==CurrOpen && q1)) {
				if(pfClose[i]>PrevCloseBar1 || (pfClose[i]==PrevCloseBar1 && q2)) {
					if(pfHigh[i]>PrevTrueHighBar2) {
						if (c4[1] && PrevCloseBar1>=PrevOpenBar1) c4ok=0;
						if (c5[1] && pfLow[i]<=PrevTrueLowBar2) c5ok=0;
						if (c4ok && c5ok) pfTDCamSell[i]=pfHigh[i]+temp;
					}
				}
			}
		}

		//Check for Camouflage buy reverse
		if (m_signal_on[2]!=' ') {
			if(pfClose[i]<CurrOpen || (pfClose[i]==CurrOpen && q1)) {
				if(pfClose[i]>PrevCloseBar1 || (pfClose[i]==PrevCloseBar1 && q2)) {
					if(pfHigh[i]<PrevTrueHighBar2 && pfHigh[i]<PrevTrueHighBar1)
					    pfTDCamBuyReverse[i]=pfLow[i]-temp;
				}
			}
		}

		//Check for Camouflage sell reverse
		if (m_signal_on[3]!=' ') {
			if(pfClose[i]>CurrOpen || (pfClose[i]==CurrOpen && q1)) {
				if(pfClose[i]<PrevCloseBar1 || (pfClose[i]==PrevCloseBar1 && q2)) {
					if(pfLow[i]>PrevTrueLowBar2 && pfLow[i]>PrevTrueLowBar1)
					    pfTDCamSellReverse[i]=pfHigh[i]+temp;
				}
			}
		}

		//Indicators are mutually exclusive. If more than two are valid, make them all INVALID
		int num_valid = 0;
	
		if(!IsInvalid(pfTDCamBuy[i])) num_valid++;
		if(!IsInvalid(pfTDCamSell[i])) num_valid++;
		if(!IsInvalid(pfTDCamBuyReverse[i])) num_valid++;
		if(!IsInvalid(pfTDCamSellReverse[i])) num_valid++;

		if (num_valid>1) 
			pfTDCamSell[i]=pfTDCamBuy[i]=pfTDCamSellReverse[i]=pfTDCamBuyReverse[i]=HOLEF;
	
	}

	SetStudyDataLine(0, pfTDCamSell);
	SetStudyDataLine(1, pfTDCamBuy);
	SetStudyDataLine(2, pfTDCamSellReverse);
	SetStudyDataLine(3, pfTDCamBuyReverse);

	delete []pfTDCamSell;
	delete []pfTDCamBuy;
	delete []pfTDCamSellReverse;
	delete []pfTDCamBuyReverse;
	delete []pfTemp;

	return NOERROR;
}


HRESULT CStudyDllTDCamouflage::CreateStudy(IBLPGraphStudyList * pGraph)
{
		
    ChartLineParameters LineParms[] = 
	{
		{1, XRT_LPAT_NONE, m_color[1]}, // Sell/Low Risk Down, default blgRed
		{1, XRT_LPAT_NONE, m_color[0]}, // Buy/Low Risk Up, default blgGreen
		{1, XRT_LPAT_NONE, m_color[3]}, // Sell Reverse/Reverse Low Risk Down, default blgMagenta
		{1, XRT_LPAT_NONE, m_color[2]}  // Buy Reverse/Reverse Low Risk Up, default blgMagenta
	};


    ChartPointParameters PointParms[] =
	{
		{XRT_POINT_DOWNARROW, 1, m_color[1]}, // Sell/Low Risk Down, default blgRed
		{XRT_POINT_UPARROW, 1, m_color[0]},   // Buy/Low Risk Up, default blgGreen
		{XRT_POINT_DOWNARROW, 1, m_color[3]}, // Sell Reverse/Reverse Low Risk Down, default blgMagenta
		{XRT_POINT_UPARROW, 1, m_color[2]}    // Buy Reverse/Reverse Low Risk Up, default blgMagenta
	};
    
    HRESULT rc = AddStudy(pGraph, 4, 4, LineParms, PointParms);
    //If there is no data for this security, it will fail at this step
    if (FAILED(rc))
        return rc;

    CComPtr<IChart> iChart;
    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
        AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "TD Camouflage", this);
    }
    
    return NOERROR;
}
