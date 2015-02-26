#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

/***********************************************************************************
 *************************  TD Channel  ******************************************** 
 ***********************************************************************************/

HRESULT CTDChannel::ConvertBuf(void * pBuf)
{
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr pData = (StudyMessagePtr)pBuf;
    int *ptemp = (int *)&(pData->TDChannel.fHighFactor);
	*ptemp = ntohl(*ptemp);

	ptemp = (int *)&(pData->TDChannel.fLowFactor);
	*ptemp = ntohl(*ptemp);

	pData->TDChannel.nSmooth = ntohs(pData->TDChannel.nSmooth);
	pData->TDChannel.nMAType = ntohs(pData->TDChannel.nMAType);
	pData->TDChannel.nPeriod = ntohs(pData->TDChannel.nPeriod);
	pData->TDChannel.nPriceHigh = ntohs(pData->TDChannel.nPriceHigh);
	pData->TDChannel.nPriceLow = ntohs(pData->TDChannel.nPriceLow);

	ConvertLineParameters(pData->TDChannel.line_params, 2);
	m_fHighFactor = pData->TDChannel.fHighFactor;
	m_fLowFactor = pData->TDChannel.fLowFactor;
	m_nMAType = (enum tagEnumMAType)pData->TDChannel.nMAType;
	if (pData->TDChannel.nPeriod != 0)
		m_nPeriod = pData->TDChannel.nPeriod;
	m_nPriceHigh = (enumPriceType)pData->TDChannel.nPriceHigh;
	m_nPriceLow = (enumPriceType)pData->TDChannel.nPriceLow;
	m_pLineParameters = pData->TDChannel.line_params;
	m_strButtonText.Format("TD Channel %d", pData->StudyHeader.msg_id == blgTDChannel1 ? 1 : 2);

	return NOERROR;
}


HRESULT CTDChannel::DoStudy(IBLPGraphData *pGraph)
{
//	if ( !pSD )
//		return E_HANDLE;

	long N;

	pGraph->get_NumPoints(&N);	

	float *pfHigh = GetStudyDataLine(pGraph, m_nPriceHigh);
	if (pfHigh == NULL)
		return E_HANDLE;

	float *pfLow = GetStudyDataLine(pGraph, m_nPriceLow);
	if (pfLow == NULL)
		return E_HANDLE;

	float *pfTDHighChannel, *pfTDLowChannel;
	if ( !(pfTDHighChannel = GetStudyDataPoints(0)) )
		return E_HANDLE;
	
	if ( !(pfTDLowChannel = GetStudyDataPoints(1)) )
		return E_HANDLE;

	if (m_nMAType == Simple) {
		pGraph->MovingAverage(m_nPeriod, N, pfLow, &pfTDLowChannel);
		pGraph->MovingAverage(m_nPeriod, N, pfHigh, &pfTDHighChannel);
		}
	else {
		pGraph->ExponentialAverage(m_nPeriod, N, pfLow, &pfTDLowChannel);
		pGraph->ExponentialAverage(m_nPeriod, N, pfHigh, &pfTDHighChannel);
		}
	for (int i = 0; i < N; i++) {
			pfTDLowChannel[i] = (CBLPPrice)pfTDLowChannel[i] * m_fLowFactor;
			pfTDHighChannel[i] = (CBLPPrice)pfTDHighChannel[i] * m_fHighFactor;
		}

	return NOERROR;
}


HRESULT CTDChannel::CreateStudy(IBLPGraphStudyList * pGraph)
{
	ChartPointParameters pp[2] = {{1,1,1},{1,1,1}};
	pp[0].point_color = m_pLineParameters[0].line_color;
	pp[1].point_color = m_pLineParameters[1].line_color;

	HRESULT rc = AddStudy(pGraph, 2, 2, m_pLineParameters, pp);
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
