#include <stdafx.h>
#include "compileropts.h"
#include "blpvector.h"

#include "blpstudydlltd.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif
#include <math.h>

HRESULT CTDDifferential::ConvertBuf(void * pBuf)
{
	int i;
	CBLPStudy::ConvertBuf(pBuf);
	StudyMessagePtr * pData = (StudyMessagePtr *)pBuf;

	pData->version=ntohs(pData->version);
	
	pData->show_buy=ntohs(pData->show_buy);
	pData->show_sell=ntohs(pData->show_sell);
	for(i=0;i<2;i++) {
		pData->color[i]=ntohl(pData->color[i]);
	}
	pData->num_days = ntohl(pData->num_days);

	memcpy(&m_nVersion,&pData->version,sizeof(short));

	memcpy(&m_nShowBuy,&pData->show_buy,sizeof(short));
	memcpy(&m_nShowSell,&pData->show_sell,sizeof(short));
	for(i=0;i<2;i++) {
		memcpy(&m_pnColor[i],&pData->color[i],sizeof(long));
	}
	memcpy(&m_nNumDays,&pData->num_days,sizeof(long));

	return NOERROR;
}

//This DoStudy did all the calculation.
HRESULT CTDDifferential::DoStudy(IBLPGraphData *pGraph)
{
	// get number of points in the graph
	pGraph->get_NumPoints(&m_nNumPoints);
	HRESULT hr = pSD->put_NumStudyPoints(m_nNumPoints + 1);
	pGraph->put_NumFuturePoints(m_nNumPoints + 1);
	// create output arrays and initialize to "hole"
	BLPVector<float> vTDDiffBuy(m_nNumPoints+1, HOLEF);
	BLPVector<float> vTDDiffSell(m_nNumPoints+1, HOLEF);

	// get data points
	float *pfOpen = GetStudyDataLine(pGraph, Open);
	float *pfClose = GetStudyDataLine(pGraph, Close);
	float *pfHigh = GetStudyDataLine(pGraph, High);
	float *pfLow = GetStudyDataLine(pGraph, Low);
	float *pfTrueHigh = GetStudyDataLine(pGraph, TrueHigh);
	float *pfTrueLow = GetStudyDataLine(pGraph, TrueLow);

	// determine physical offset of indicator from high/low
	float offset, max, min;
	GetDataRange(m_nNumPoints, pfHigh,&offset,&max);
	GetDataRange(m_nNumPoints, pfLow, &min,&offset);
	offset=(max-min)*(float).035;

	BLPVector<float> vOpen,vClose,vHigh,vLow,vTrueHigh,vTrueLow;

	// iterate through data and assign output
	for(int i=0;i<m_nNumPoints;i++) {
		if(m_nNumDays<=0 || (m_nNumPoints-i <= m_nNumDays)) { // check that we are in a requested time range
			// get (sub) vectors of points from (potentially) subclassed method
			m_FindRelevantData(	vOpen, vClose, vHigh, vLow, vTrueHigh, vTrueLow,
							   pfOpen,pfClose,pfHigh,pfLow,pfTrueHigh,pfTrueLow,i);

			if(m_ShowBuy() && m_BuySignal(vOpen,vClose,vHigh,vLow,vTrueHigh,vTrueLow) &&
				!IsInvalid(pfLow[i])) {
				// set appropriate output array value
				vTDDiffBuy[i]=pfLow[i]-offset;
			}

			if(m_ShowSell() && m_SellSignal(vOpen,vClose,vHigh,vLow,vTrueHigh,vTrueLow) &&
				!IsInvalid(pfHigh[i])) {
				// set appropriate output array value
				vTDDiffSell[i]=pfHigh[i]+offset;
			}
		}
	}

		// calculate dynamic buying and selling markers
	BLPVector<float> vTDDiffBuyMark(m_nNumPoints + 1, HOLEF);
	BLPVector<float> vTDDiffSellMark(m_nNumPoints + 1, HOLEF);

	// get today's data
	m_FindRelevantData(	vOpen, vClose, vHigh, vLow, vTrueHigh, vTrueLow,
						pfOpen,pfClose,pfHigh,pfLow,pfTrueHigh,pfTrueLow,m_nNumPoints-1);


	// range projection needs at least two consecutive days of data to display, so we
	// use today, and one day in the future
	vTDDiffBuyMark[m_nNumPoints]=vTDDiffBuyMark[m_nNumPoints-1]=
		m_CalcBuyMark(vOpen, vClose, vHigh, vLow, vTrueHigh, vTrueLow);
	vTDDiffSellMark[m_nNumPoints]=vTDDiffSellMark[m_nNumPoints-1]=
		m_CalcSellMark(vOpen, vClose, vHigh, vLow, vTrueHigh, vTrueLow);

	CComPtr<IStudyData> iSD;
	hr = m_vCharts[0]->QueryInterface(IID_IStudyData, (LPVOID *)(&iSD));
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	iSD->put_StudyDataLine(0, vTDDiffBuyMark.size(), &vTDDiffBuyMark[0]);
	iSD->put_StudyDataLine(1, vTDDiffSellMark.size(), &vTDDiffSellMark[0]);
	iSD->put_StudyDataLine(2, vTDDiffBuy.size(), &vTDDiffBuy[0]);
	iSD->put_StudyDataLine(3, vTDDiffSell.size(), &vTDDiffSell[0]);

	return NOERROR;
}


HRESULT CTDDifferential::CreateStudy(IBLPGraphStudyList * pGraph)
{
		
    ChartLineParameters LineParms[] = 
	{
		{1, XRT_LPAT_SOLID, m_pnColor[0]}, // Dynamic buying Pressure Marker
		{1, XRT_LPAT_SOLID, m_pnColor[1]}, // Dynamic selling pressure marker
		{1, XRT_LPAT_NONE, m_pnColor[0]}, // Up - Buy Signal
		{1, XRT_LPAT_NONE, m_pnColor[1]}, // Down - Sell Signal
	};

    ChartPointParameters PointParms[] =
	{
		{XRT_POINT_NONE, 0, 0}, // dynamic buying pressure marker
		{XRT_POINT_NONE, 0, 0},   // dynamic selling pressure marker
		{XRT_POINT_UPARROW, 1, m_pnColor[0]}, // Up
		{XRT_POINT_DOWNARROW, 1, m_pnColor[1]},   // Down
	};
    
    HRESULT rc = AddStudy(pGraph, 4, 4, LineParms, PointParms);
    //If there is no data for this security, it will fail at this step
    if (FAILED(rc))
        return rc;

    CComPtr<IChart> iChart;
    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
        AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, m_GetIDString(), this);
    }

	pSD->put_StudyStyle(0, STUDY_STYLE_OPEN_STEP);
	pSD->put_StudyStyle(1, STUDY_STYLE_OPEN_STEP);
	
    return rc;
}

void CTDDifferential::m_FindRelevantData(BLPVector<float> &vOpen,
										 BLPVector<float> &vClose,
										 BLPVector<float> &vHigh,
										 BLPVector<float> &vLow,
										 BLPVector<float> &vTrueHigh,
										 BLPVector<float> &vTrueLow,
										 float * pfOpen,
										 float * pfClose,
										 float * pfHigh,
										 float * pfLow,
										 float * pfTrueHigh,
										 float * pfTrueLow,
										 int i) {
	// initialize vectors to empty
	vOpen.erase(vOpen.begin(),vOpen.end());
	vClose.erase(vClose.begin(),vClose.end());
	vHigh.erase(vHigh.begin(),vHigh.end());
	vLow.erase(vLow.begin(),vLow.end());
	vTrueHigh.erase(vTrueHigh.begin(),vTrueHigh.end());
	vTrueLow.erase(vTrueLow.begin(),vTrueLow.end());

	// push the last m_nDaysToConsider days worth of data into vectors
	for(int j=i;j>=0 && vOpen.size()<m_nDaysToConsider;j--) {
		if(m_IsValidDay(pfOpen[j],pfClose[j],pfHigh[j],pfLow[j],pfTrueHigh[j],pfTrueLow[j])) {
			vOpen.insert(vOpen.begin(),pfOpen[j]);
			vClose.insert(vClose.begin(),pfClose[j]);
			vHigh.insert(vHigh.begin(),pfHigh[j]);
			vLow.insert(vLow.begin(),pfLow[j]);
			vTrueHigh.insert(vTrueHigh.begin(),pfTrueHigh[j]);
			vTrueLow.insert(vTrueLow.begin(),pfTrueLow[j]);
		}
	}

}


// as some days have invalid, or incomplete data, this function determines whether
// we have enough data to use for this specific study
bool CTDDifferential::m_IsValidDay(float nOpen,
								  float nClose,
								  float nHigh,
								  float nLow,
								  float nTrueHigh,
								  float nTrueLow) {
	if(this->IsInvalid(nClose)) {
		return false;
	}

	if(this->IsInvalid(nHigh)) {
		return false;
	}

	if(this->IsInvalid(nLow)) {
		return false;
	}
	
	if(this->IsInvalid(nTrueHigh)) {
		return false;
	}
	
	if(this->IsInvalid(nTrueLow)) {
		return false;
	}

	return true;
}

// boolean function looks at input data and determines whether to display a
// buy signal at the appropriate data point
bool CTDDifferential::m_BuySignal(BLPVector<float> &vOpen,
								  BLPVector<float> &vClose,
								  BLPVector<float> &vHigh,
								  BLPVector<float> &vLow,
								  BLPVector<float> &vTrueHigh,
								  BLPVector<float> &vTrueLow) {

	// check if we have enough data to make evaluation
	if(vOpen.size() < m_nDaysToConsider ||
	   vClose.size() < m_nDaysToConsider ||
	   vHigh.size() < m_nDaysToConsider ||
	   vLow.size() < m_nDaysToConsider ||
	   vTrueHigh.size() < m_nDaysToConsider ||
	   vTrueLow.size() < m_nDaysToConsider) {

		return false;
	}

	// check for 2 consecutive down closes:
	if( ! (vClose[vClose.size()-1] < vClose[vClose.size()-2] &&
		   vClose[vClose.size()-2] < vClose[vClose.size()-3]) ) {
		return false;
	}

	// check that buying pressure is greater for current bar than previous bar
	BLPVector<float> vBuyingPressure(m_nDaysToConsider);

	int i;
	for(i=0;i<m_nDaysToConsider;i++) {
		vBuyingPressure[i] = vClose[i] - vTrueLow[i];
	}

	if( ! (vBuyingPressure[vBuyingPressure.size()-1] > vBuyingPressure[vBuyingPressure.size()-2])) {
		return false;
	}

	// check that selling pressure is less for current bar than previous bar
	BLPVector<float> vSellingPressure(m_nDaysToConsider);

	for(i=0;i<m_nDaysToConsider;i++) {
		vSellingPressure[i] = vTrueHigh[i] - vClose[i];
	}

	if( ! (vSellingPressure[vSellingPressure.size()-1] < vSellingPressure[vSellingPressure.size()-2])) {
		return false;
	}

	// if we have made it to this point, we have a valid point
	return true;
}

// boolean function looks at input data and determines whether to display a
// Sell signal at the appropriate data point
bool CTDDifferential::m_SellSignal(BLPVector<float> &vOpen,
								   BLPVector<float> &vClose,
								   BLPVector<float> &vHigh,
								   BLPVector<float> &vLow,
								   BLPVector<float> &vTrueHigh,
								   BLPVector<float> &vTrueLow) {

	// check if we have enough data to make evaluation
	if(vOpen.size() < m_nDaysToConsider ||
	   vClose.size() < m_nDaysToConsider ||
	   vHigh.size() < m_nDaysToConsider ||
	   vLow.size() < m_nDaysToConsider ||
	   vTrueHigh.size() < m_nDaysToConsider ||
	   vTrueLow.size() < m_nDaysToConsider) {
		return false;
	}

	// check for 2 consecutive up closes:
	if( ! (vClose[vClose.size()-1] > vClose[vClose.size()-2] &&
		   vClose[vClose.size()-2] > vClose[vClose.size()-3]) ) {
		return false;
	}

	// check that buying pressure is less for current bar than previous bar
	BLPVector<float> vBuyingPressure(m_nDaysToConsider);

	int i;
	for(i=0;i<m_nDaysToConsider;i++) {
		vBuyingPressure[i] = vClose[i] - vTrueLow[i];
	}

	if( ! (vBuyingPressure[vBuyingPressure.size()-1] < vBuyingPressure[vBuyingPressure.size()-2])) {
		return false;
	}

	// check that selling pressure is greater for current bar than previous bar
	BLPVector<float> vSellingPressure(m_nDaysToConsider);

	for(i=0;i<m_nDaysToConsider;i++) {
		vSellingPressure[i] = vTrueHigh[i] - vClose[i];
	}

	if( ! (vSellingPressure[vSellingPressure.size()-1] > vSellingPressure[vSellingPressure.size()-2])) {
		return false;
	}

	// if we have made it to this point, we have a valid point
	return true;
}


// calculates the location of the dynamic buying pressure marker for a study
// if no marker should be displayed, this returns HOLEF
float CTDDifferential::m_CalcBuyMark(BLPVector<float> &vOpen,
						    BLPVector<float> &vClose,
						    BLPVector<float> &vHigh,
						    BLPVector<float> &vLow,
						    BLPVector<float> &vTrueHigh,
							BLPVector<float> &vTrueLow) {
	// check if we have enough data to make evaluation
	if(vOpen.size() < m_nDaysToConsider ||
	   vClose.size() < m_nDaysToConsider ||
	   vHigh.size() < m_nDaysToConsider ||
	   vLow.size() < m_nDaysToConsider ||
	   vTrueHigh.size() < m_nDaysToConsider ||
	   vTrueLow.size() < m_nDaysToConsider) {

		return HOLEF;
	}
	
	BLPVector<float> vBuyingPressure(2);
	BLPVector<float> vSellingPressure(2);

	// calculate buying and selling pressure
	for(int i=0;i<2;i++) {
		vBuyingPressure[vBuyingPressure.size()-i-1]=vClose[vClose.size()-i-1]-vTrueLow[vTrueLow.size()-i-1];
		vSellingPressure[vSellingPressure.size()-i-1]=vTrueHigh[vTrueHigh.size()-i-1]-vClose[vClose.size()-i-1];
	}

	// display dynamic buying pressure marker for a TD Diff buy
	if(m_ShowBuy() && vClose[vClose.size()-3] > vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] > vClose[vClose.size()-1]
//		&& vBuyingPressure[vBuyingPressure.size()-2] < vBuyingPressure[vBuyingPressure.size()-1]
//		&& vSellingPressure[vSellingPressure.size()-2] > vSellingPressure[vSellingPressure.size()-1]
		) {
		return vTrueLow[vTrueLow.size()-1] + vBuyingPressure[vBuyingPressure.size()-2];
	}

	// display dynamic buying pressure marker for a TD Diff sell
	if(m_ShowSell() && vClose[vClose.size()-3] < vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] < vClose[vClose.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] > vBuyingPressure[vBuyingPressure.size()-1] 
//		&& vSellingPressure[vSellingPressure.size()-2] > vSellingPressure[vSellingPressure.size()-1]
		) {
		return vTrueLow[vTrueLow.size()-1] + vBuyingPressure[vBuyingPressure.size()-2];
	}

	return HOLEF;
}

// calculates the location of the dynamic selling pressure marker for a study
// if no marker should be displayed, this returns HOLEF
float CTDDifferential::m_CalcSellMark(BLPVector<float> &vOpen,
						     BLPVector<float> &vClose,
						     BLPVector<float> &vHigh,
						     BLPVector<float> &vLow,
						     BLPVector<float> &vTrueHigh,
							 BLPVector<float> &vTrueLow) {
	// check if we have enough data to make evaluation
	if(vOpen.size() < m_nDaysToConsider ||
	   vClose.size() < m_nDaysToConsider ||
	   vHigh.size() < m_nDaysToConsider ||
	   vLow.size() < m_nDaysToConsider ||
	   vTrueHigh.size() < m_nDaysToConsider ||
	   vTrueLow.size() < m_nDaysToConsider) {

		return HOLEF;
	}

	BLPVector<float> vBuyingPressure(2);
	BLPVector<float> vSellingPressure(2);

	// calculate buying and selling pressure
	for(int i=0;i<2;i++) {
		vBuyingPressure[vBuyingPressure.size()-i-1]=vClose[vClose.size()-i-1]-vTrueLow[vTrueLow.size()-i-1];
		vSellingPressure[vSellingPressure.size()-i-1]=vTrueHigh[vTrueHigh.size()-i-1]-vClose[vClose.size()-i-1];
	}

	// display dynamic selling pressure marker for a TD Diff buy
	if(m_ShowBuy() && vClose[vClose.size()-3] > vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] > vClose[vClose.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] < vBuyingPressure[vBuyingPressure.size()-1]
//		&& vSellingPressure[vSellingPressure.size()-2] < vSellingPressure[vSellingPressure.size()-1]
		) {
		return vTrueHigh[vTrueHigh.size()-1] - vSellingPressure[vSellingPressure.size()-2];
	}

	// display dynamic selling pressure marker for a TD Diff sell
	if(m_ShowSell() && vClose[vClose.size()-3] < vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] < vClose[vClose.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] > vBuyingPressure[vBuyingPressure.size()-1] 
//		&& vSellingPressure[vSellingPressure.size()-2] < vSellingPressure[vSellingPressure.size()-1]
		) {
		return vTrueHigh[vTrueHigh.size()-1] - vSellingPressure[vSellingPressure.size()-2];
	}

	return HOLEF;
}

// end of file
