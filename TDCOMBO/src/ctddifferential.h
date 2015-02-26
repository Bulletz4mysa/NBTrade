
#ifndef __CTDDIFFERENTIAL_H__
#define __CTDDIFFERENTIAL_H__


#include "blpvector.h"
#include "blpstudydlltd.h"
#include "demarkstudies.h"

class CTDDifferential : public CStudyDllTD
{
public:
	CTDDifferential() {
		m_nDaysToConsider=3;
	}

	typedef struct tagStructStudyMessageBuf 
	{
		// Header
		short msg_id;   //	CTDDifferential ####
		short instance_id;
		short base_id;

		short version;

		// Message
		short show_buy;	
		short show_sell;
		long color[2]; // up, down
		long num_days;
	} StudyMessagePtr;

	virtual ~CTDDifferential(){}
	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:

	// m_FindRelevantData populates the vectors with the appropriate data for
	// the logical check for a signal.  e.g. if we are at data point 5, we
	// might want to fill the vectors with the last 4 data points so that m_ShowBuy and
	// m_ShowSell can base their logic off of those 4 data points,
	virtual void m_FindRelevantData(BLPVector<float> &vOpen,
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
									int i);


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

	virtual char *m_GetIDString() { return "TD Differential"; }

	// variables assigned in ConvertBuf
	short m_nShowBuy;	
	short m_nShowSell;

	short m_nVersion;

	bool m_ShowBuy() { return m_nShowBuy!=0; }
	bool m_ShowSell() { return m_nShowSell!=0; }

	long m_pnColor[2]; // up, down
	long m_nNumDays; // number days back to look, if m_nNumDays <= 0, we will display all days


 	// used by this implimentation of m_FindRelevantData, m_BuySignal, and m_SellSignal
	// to determine number of days to use for study
	short m_nDaysToConsider;
private:

};

#endif // __CTDDIFFERENTIAL_H__

// end of file
