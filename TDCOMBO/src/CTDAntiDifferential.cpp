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

// as some days have invalid, or incomplete data, this function determines whether
// we have enough data to use for this specific study
bool CTDAntiDifferential::m_IsValidDay(float nOpen,
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

	return true;
}


// boolean function looks at input data and determines whether to display a
// buy signal at the appropriate data point
bool CTDAntiDifferential::m_BuySignal(BLPVector<float> &vOpen,
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

	// 2 consecutive bars where the close is less than the previous bar's close
	if(! (vClose[vClose.size()-5] > vClose[vClose.size()-4]) ) {
		return false;
	}

	if(! (vClose[vClose.size()-4] > vClose[vClose.size()-3]) ) {
		return false;
	}

	// bar which has a lower low than the previous bar's low
	// AND closes higher than the previous bars close

	if(! (vLow[vLow.size()-3] > vLow[vLow.size()-2]) ) {
		return false;
	}

	if(! (vClose[vClose.size()-3] < vClose[vClose.size()-2]) ) {
		return false;
	}

	// the final bar of the pattern closes lower than the bar 2 bars back
	if(! (vClose[vClose.size()-3] > vClose[vClose.size()-1]) ) {
		return false;
	}

	// if we have made it to this point, we have a valid point
	return true;
}

// boolean function looks at input data and determines whether to display a
// Sell signal at the appropriate data point
bool CTDAntiDifferential::m_SellSignal(BLPVector<float> &vOpen,
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


	// 2 consecutive bars where the close is greater than the previous bar's close
	if(! (vClose[vClose.size()-5] < vClose[vClose.size()-4]) ) {
		return false;
	}

	if(! (vClose[vClose.size()-4] < vClose[vClose.size()-3]) ) {
		return false;
	}

	// a bar which has a higher high than the previous bar's high
	// AND closes lower than the previous bar's close

	if(! (vHigh[vHigh.size()-3] < vHigh[vHigh.size()-2]) ) {
		return false;
	}

	if(! (vClose[vClose.size()-3] > vClose[vClose.size()-2]) ) {
		return false;
	}

	// the final bar of the pattern closes higher than the bar 2 bars back
	if(! (vClose[vClose.size()-3] < vClose[vClose.size()-1]) ) {
		return false;
	}

	// if we have made it to this point, we have a valid point
	return true;
}

// end of file
