#ifndef __CTDANTIDIFFERENTIAL_H__
#define __CTDANTIDIFFERENTIAL_H__



#include "ctddifferential.h"

// CTDReverseDifferential overrides the protected member functions of CTDDifferential
class CTDAntiDifferential : public CTDDifferential
{
public:
	CTDAntiDifferential() {
		m_nDaysToConsider=5;
	}
protected:

	// as some days have invalid, or incomplete data, this function determines whether
	// we have enough data to use for this specific study
	virtual bool m_IsValidDay(float nOpen,
							 float nClose,
							 float nHigh,
							 float nLow,
							 float nTrueHigh,
							 float nTrueLow);

	// boolean function looks at input data and determines whether to display a
	// buy signal at the appropriate data point
	// pass by reference for efficiency
	virtual bool m_BuySignal(BLPVector<float> &vOpen,
							 BLPVector<float> &vClose,
							 BLPVector<float> &vHigh,
							 BLPVector<float> &vLow,
							 BLPVector<float> &vTrueHigh,
							 BLPVector<float> &vTrueLow);

	// boolean function looks at input data and determines whether to display a
	// sell signal at the appropriate data point
	// pass by reference for efficiency
	virtual bool m_SellSignal(BLPVector<float> &vOpen,
							  BLPVector<float> &vClose,
							  BLPVector<float> &vHigh,
							  BLPVector<float> &vLow,
							  BLPVector<float> &vTrueHigh,
							  BLPVector<float> &vTrueLow);


	// calculates the location of the dynamic buying pressure marker for a study
	// if no marker should be displayed, this returns HOLEF
	// pass by reference for efficiency
	virtual float m_CalcBuyMark(BLPVector<float> &vOpen,
							    BLPVector<float> &vClose,
							    BLPVector<float> &vHigh,
							    BLPVector<float> &vLow,
							    BLPVector<float> &vTrueHigh,
								BLPVector<float> &vTrueLow) {
		return HOLEF;
	}

	// calculates the location of the dynamic selling pressure marker for a study
	// if no marker should be displayed, this returns HOLEF
	// pass by reference for efficiency
	virtual float m_CalcSellMark(BLPVector<float> &vOpen,
							     BLPVector<float> &vClose,
							     BLPVector<float> &vHigh,
							     BLPVector<float> &vLow,
							     BLPVector<float> &vTrueHigh,
								 BLPVector<float> &vTrueLow) {
		return HOLEF;
	}


	virtual char *m_GetIDString() { return "TD Anti-Differential"; }
private:
};


#endif // __CTDANTIDIFFERENTIAL_H__

// end of file
