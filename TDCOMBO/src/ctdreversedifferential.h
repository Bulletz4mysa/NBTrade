#ifndef __CTDREVERSEDIFFERENTIAL_H__
#define __CTDREVERSEDIFFERENTIAL_H__



#include "ctddifferential.h"

// CTDReverseDifferential overrides the protected member functions of CTDDifferential
class CTDReverseDifferential : public CTDDifferential
{
public:
		
protected:


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
							    BLPVector<float> &vTrueLow);

	// calculates the location of the dynamic selling pressure marker for a study
	// if no marker should be displayed, this returns HOLEF
	// pass by reference for efficiency
	virtual float m_CalcSellMark(BLPVector<float> &vOpen,
							     BLPVector<float> &vClose,
							     BLPVector<float> &vHigh,
							     BLPVector<float> &vLow,
							     BLPVector<float> &vTrueHigh,
							     BLPVector<float> &vTrueLow);

	virtual char *m_GetIDString() { return "TD Reverse Differential"; }
	
private:
};


#endif // __CTDREVERSEDIFFERENTIAL_H__

// end of file
