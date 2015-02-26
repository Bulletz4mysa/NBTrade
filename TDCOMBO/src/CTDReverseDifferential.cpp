#include <stdafx.h>

#include "blpstudydlltd.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif
#include <math.h>

#include "compileropts.h"
#include "blpvector.h"


// boolean function looks at input data and determines whether to display a
// buy signal at the appropriate data point
bool CTDReverseDifferential::m_BuySignal(BLPVector<float> &vOpen,
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
		vSellingPressure[i] = vHigh[i] - vClose[i];
	}

	if( ! (vSellingPressure[vSellingPressure.size()-1] < vSellingPressure[vSellingPressure.size()-2])) {
		return false;
	}

	// if we have made it to this point, we have a valid point
	return true;
}

// boolean function looks at input data and determines whether to display a
// Sell signal at the appropriate data point
bool CTDReverseDifferential::m_SellSignal(BLPVector<float> &vOpen,
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

	// check that buying pressure is less for current bar than previous bar
	BLPVector<float> vBuyingPressure(m_nDaysToConsider);

	int i;
	for(i=0;i<m_nDaysToConsider;i++) {
		vBuyingPressure[i] = vClose[i] - vLow[i];
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
float CTDReverseDifferential::m_CalcBuyMark(BLPVector<float> &vOpen,
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

	// calculate buying/selling pressure
	for(int i=0;i<2;i++) {
		vBuyingPressure[vBuyingPressure.size()-i-1]=vClose[vClose.size()-i-1]-vTrueLow[vTrueLow.size()-i-1];
		vSellingPressure[vSellingPressure.size()-i-1]=vTrueHigh[vTrueHigh.size()-i-1]-vClose[vClose.size()-i-1];
	}

	// display dynamic selling pressure marker for a TD Reverse Diff buy
	if(m_ShowBuy() && vClose[vClose.size()-3] < vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] < vClose[vClose.size()-1]
//		&& vSellingPressure[vSellingPressure.size()-2] > vSellingPressure[vSellingPressure.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] < vBuyingPressure[vBuyingPressure.size()-1]
		) {
		return vTrueLow[vTrueLow.size()-1] + vBuyingPressure[vBuyingPressure.size()-2];
	}


	// display dynamic selling pressure marker for a TD Reverse Diff sell
	if(m_ShowSell() && vClose[vClose.size()-3] > vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] > vClose[vClose.size()-1] 
//		&& vSellingPressure[vSellingPressure.size()-2] < vSellingPressure[vSellingPressure.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] > vBuyingPressure[vBuyingPressure.size()-1]
		) {
		return vTrueLow[vTrueLow.size()-1] + vBuyingPressure[vBuyingPressure.size()-2];
	}

	return HOLEF;
}

// calculates the location of the dynamic selling pressure marker for a study
// if no marker should be displayed, this returns HOLEF
float CTDReverseDifferential::m_CalcSellMark(BLPVector<float> &vOpen,
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

	// calculate buying/selling pressure
	for(int i=0;i<2;i++) {
		vBuyingPressure[vBuyingPressure.size()-i-1]=vClose[vClose.size()-i-1]-vTrueLow[vTrueLow.size()-i-1];
		vSellingPressure[vSellingPressure.size()-i-1]=vTrueHigh[vTrueHigh.size()-i-1]-vClose[vClose.size()-i-1];
	}

	// display dynamic selling pressure marker for a TD Reverse Diff buy
	if(m_ShowBuy() && vClose[vClose.size()-3] < vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] < vClose[vClose.size()-1] 
//		&& vSellingPressure[vSellingPressure.size()-2] > vSellingPressure[vSellingPressure.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] < vBuyingPressure[vBuyingPressure.size()-1]
		) {
		return vTrueHigh[vTrueHigh.size()-1] - vSellingPressure[vSellingPressure.size()-2];
	}


	// display dynamic selling pressure marker for a TD Reverse Diff sell
	if(m_ShowSell() && vClose[vClose.size()-3] > vClose[vClose.size()-2] &&
		vClose[vClose.size()-2] > vClose[vClose.size()-1] 
//		&& vSellingPressure[vSellingPressure.size()-2] < vSellingPressure[vSellingPressure.size()-1] 
//		&& vBuyingPressure[vBuyingPressure.size()-2] > vBuyingPressure[vBuyingPressure.size()-1]
		) {
		return vTrueHigh[vTrueHigh.size()-1] - vSellingPressure[vSellingPressure.size()-2];
	}

	return HOLEF;
}

// end of file
