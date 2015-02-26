// legacystudies.h: interface for the CBollingerBands class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEGACYSTUDIES_H__2239355A_2ACD_4CF0_98CD_84305570DAF4__INCLUDED_)
#define AFX_LEGACYSTUDIES_H__2239355A_2ACD_4CF0_98CD_84305570DAF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "blpstudy.h"

class CBollingerBands : public CBLPStudy2  
{
public:
	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short period;
		float percent_band1;
		float percent_band2;
		char field_id[FIELD_LEN];
		ChartLineParameters line_params[5];
		short show_boll_bandwidth;
		short show_boll_percentb;
		short boll_lead_lag;
		short dummy_field;
		BYTE version;
		//--------version 0
		BYTE skip[3];
		Chart_Color_Scheme bandwidth_chart_colors;
		Chart_Color_Scheme percent_chart_colors;
		ChartPointParameters point_params[5];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
			//--------version 2
		} *StudyMessagePtr;

	CBollingerBands();
	virtual ~CBollingerBands();

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	int m_nPeriod;				// the MAVG period
	float m_fUpperBandFactor;				// upper band constant
	float m_fLowerBandFactor;				// lower band constant
	CFastString m_strCalcRtId;			// dataline to use;
    ChartLineParameters m_LineParams[5];
    ChartPointParameters m_PointParams[5];
	BOOL m_bShowBollBandwidth;
	BOOL m_bShowBollPercentb;
	COLORREF m_nBandwidthBackground;
	COLORREF m_nPercentBBackground;
};

class CTE : public CBollingerBands
{
// IBLPStudy overrides
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

};

class CDirectionalMovementIndicator : public CBLPOscillatorStudy  
{
public:
	CDirectionalMovementIndicator(){}
	virtual ~CDirectionalMovementIndicator(){}

	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;

		short period;  
		char unused[FIELD_LEN];
		ChartLineParameters line_params[4];
		OscillatorParameters unused_too;
		short show_adx;
		short show_adxr;
		BYTE version;
		//--------version 0
		BYTE skip[3];
		Chart_Color_Scheme chart_colors;
		ChartPointParameters point_params[4];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
		} *StudyMessagePtr;

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	BOOL m_bShowADX;
	BOOL m_bShowADXR;
	COLORREF m_crBackground;
    ChartLineParameters m_LineParams[4];
    ChartPointParameters m_PointParams[4];
};

class CParabolicTimePrice : public CBLPStudy2  
{
public:
	CParabolicTimePrice(){}
	virtual ~CParabolicTimePrice(){}

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short dummy_field;
		float accel_factor;  
		float start;
		float max;
		ChartLineParameters line_params[1];
		BYTE version;
		//--------version 0
		BYTE skip[3];
		ChartPointParameters point_params[1];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
		} *StudyMessagePtr;

protected:
	float m_fAccelerationFactor;
	float m_fStart;
	float m_fMax;
};

class CMAvgConvDiv : public CBLPStudy2  
{
public:
	virtual ~CMAvgConvDiv(){}
	CMAvgConvDiv()
	{
		memset(&m_PointParams, 0, sizeof(m_PointParams));
	}

	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short period1;
		short period2;
		short period3;
		char field_id[FIELD_LEN];
		ChartLineParameters line_params[3];
		short show_macd_histogram;
		short show_macd2;
		BYTE version;
		//--------version 0
		BYTE skip[3];
		Chart_Color_Scheme chart_colors;
		ChartPointParameters point_params[3];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
		} *StudyMessagePtr;

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod1;
	short m_nPeriod2;
	short m_nSignalPeriod;
	BOOL m_bShowHistogram;
	BOOL m_bShowMACD2;
	COLORREF m_crBackground;
    ChartLineParameters m_LineParams[3];
    ChartPointParameters m_PointParams[3];
	float *m_pfDataIn;
};

#if 0
class CTAStochastics : public CBLPOscillatorStudy
{
public:
	virtual ~CTAStochastics(){}
	CTAStochastics()
	{
		m_fMax = 110;
		m_fMin = -10;
	}

	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short period1;
		short period2;
		short period3;
		short period4;
		short show_tas_fast;
		short show_tas_slow;
		short dummy_field;
		ChartLineParameters line_params[4];
		OscillatorParameters zone;
		BYTE version;
		//--------version 0
		BYTE skip[3];
		Chart_Color_Scheme chart_colors;
		Chart_Color_Scheme second_chart_colors;
		ChartPointParameters point_params[4];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
	} *StudyMessagePtr;

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nKPeriod;
	short m_nDPeriod;
	short m_nDSPeriod;
	short m_nDSSPeriod;

	BOOL m_bShowFast;
	BOOL m_bShowSlow;

	COLORREF m_crBackgroundFastchart;
	COLORREF m_crBackgroundSlowchart;

};
#endif

class CGOCChart : public CBLPStudy2
				  ,public IGraphLegendLine
{
public:
	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short period1;
		short period2;
		short period3;
		ChartLineParameters line_params[5];
		BYTE version;
		//--------version 0
		BYTE skip[3];
		ChartPointParameters point_params[5];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
		} *StudyMessagePtr;

	CGOCChart()
	{
		m_nConversionPeriod = 9;	
		m_nBasePeriod = 26;	
		m_nLeadLagPeriod = 26;
	}

	virtual ~CGOCChart(){}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

// IGraphLegendLine
	virtual HRESULT get_MonitoringString(short LineNum, char *LegendString[])
	{
		return E_NOTIMPL;
	}
	virtual HRESULT get_TrackingString(short LineNum, long BarNum, char *LegendString[])
	{
		return E_NOTIMPL;
	}

	virtual HRESULT get_MonitoringValue(short LineNum, float *LegendValue)
	{
		float *fStudyDataPoints = GetStudyDataPoints(LineNum);
		*LegendValue = HOLEF;
		long nCurrentBarNum = m_nNumGraphDataPoints - 1;
		switch (LineNum) {
		  case 2:// lagging span
			*LegendValue = fStudyDataPoints[nCurrentBarNum - m_nLeadLagPeriod];
			break;
		
		  case 3:
		  case 4:
			*LegendValue = fStudyDataPoints[nCurrentBarNum + m_nLeadLagPeriod - 1];
			break;

		  default:
			*LegendValue = fStudyDataPoints[nCurrentBarNum];
			break;
		}

		return S_OK;
	}

	virtual HRESULT get_TrackingValue(short LineNum, long BarNum, float *LegendString)
	{
		return E_NOTIMPL;
	}

	IMPLEMENT_IUnknown(CGOCChart)

	BEGIN_BLPINTERFACE_MAP(CGOCChart, CBLPStudy2)
		SUPPORT_GEN_INTERFACE(IGraphLegendLine)
	END_BLPINTERFACE_MAP()

protected:
	short m_nConversionPeriod;
	short m_nBasePeriod;
	short m_nLeadLagPeriod;
	long m_nNumGraphDataPoints;
};

class CRateOfChange : public CBLPOscillatorStudy  
{
public:
	typedef struct tagStructStudyMessageBuf{
		short msg_id;
		short instance_id;
		short base_id;
		short period; 
		short mode;
		short lead_lag;
		char field_id[FIELD_LEN];
		short window_id;
		short dummy;
		ChartLineParameters line_params[1];
		BYTE version;
		//--------version 0
		BYTE skip[3];
		Chart_Color_Scheme chart_colors;
		ChartPointParameters point_params[1];
		//--------version 1
		short window_owner_id;
		char window_index;
		char rreserved;
		//--------version 2
		} *StudyMessagePtr;

	CRateOfChange()
	{
	}

	virtual ~CRateOfChange(){}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	BOOL m_bModeIsPercent;
	COLORREF m_crChartBackground;
};

class CVolumeChart : public CBLPStudy2
{
public:
	CVolumeChart()
	{
	}

typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
    char  count_ticks;
    char  dummy_field;
	long color;
    BYTE version;
    //--------version 0
    BYTE skip[3];
	Chart_Color_Scheme chart_colors;
    //--------version 1
    short window_owner_id;
    char window_index;
    char reserved;
    //--------version 2
	char VolumeId[6]; //NULL terminated CalcRt Id
    //--------version 3
}  *StudyMessagePtr;

	virtual ~CVolumeChart(){}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	COLORREF m_crHistogramBar;
	COLORREF m_crBackground;

};

class CMavg : public CBLPStudy2
{
public:
	CMavg()
	{
	}

typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
	short period;
	char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
    ChartLineParameters line_params[1];
    BYTE version;
    //--------version 0
    BYTE skip[3];
    ChartPointParameters point_params[1];
    //--------version 1
    short window_owner_id;
    char window_index;
    char rreserved;
    //--------version 2
	}  *StudyMessagePtr;

	virtual ~CMavg(){}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
//	virtual HRESULT DoStudy(IBLPGraphData *pGraph); override in derived classes
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	CFastString m_strCalcRtId;
//	enum {Simple, Exponential} MovingAverageType;
};

class CExponentialMavg : public CMavg
{
public:
	CExponentialMavg() {
		m_strButtonText = "Exp Mavg";
		}
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
};

class CSimpleMavg : public CMavg
{
public:
	CSimpleMavg() {
		m_strButtonText = "Simple Mavg";
		}

	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
};

class CVariableMavg : public CMavg
{
public:
	CVariableMavg() {
		m_strButtonText = "Var Mavg";
		}

	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
};

class CWeightedMavg : public CMavg
{
protected:
	BLPVector<float> m_vfWeights;

public:
	CWeightedMavg() {
		m_strButtonText = "Weighted Mavg";
		}

	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
};

class CTriangularMavg : public CWeightedMavg
{
public:
	CTriangularMavg() {
		m_strButtonText = "Tri Mavg";
		}

	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};


class CRelativeStrengthIndex : public CBLPOscillatorStudy
{
public:
	CRelativeStrengthIndex()
	{
		m_fMax = 100;
		m_fMin = 0;
	}

typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
	short period;
	char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
    ChartLineParameters line_params[1];
	OscillatorParameters zone;
    BYTE version;
    //--------version 0
    BYTE skip[3];
	Chart_Color_Scheme chart_colors;
    ChartPointParameters point_params[1];
    //--------version 1
    short window_owner_id;
    char window_index;
    char rreserved;
    //--------version 2
	}  *StudyMessagePtr;

// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	COLORREF m_crBackground;
};

class CCMCI : public CBLPOscillatorStudy
{
public:
typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
	short period;  
	char unused[FIELD_LEN];
    ChartLineParameters line_params[1];
	OscillatorParameters zone;
    BYTE version;
    //--------version 0
    BYTE skip[3];
	Chart_Color_Scheme chart_colors;
    ChartPointParameters point_params[1];
    //--------version 1
    short window_owner_id;
    char window_index;
    char rreserved;
    //--------version 2
}  *StudyMessagePtr;

CCMCI()
{
	m_fMax = 210;
	m_fMin = -210;
}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	COLORREF m_crBackground;
};

class CWilliam : public CBLPOscillatorStudy
{
public:
typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
	short period;  
	char unused[FIELD_LEN];
    ChartLineParameters line_params[1];
	OscillatorParameters zone;
    BYTE version;
    //--------version 0
    BYTE skip[3];
	Chart_Color_Scheme chart_colors;
    ChartPointParameters point_params[1];
    //--------version 1
    short window_owner_id;
    char window_index;
    char rreserved;
    //--------version 2
}  *StudyMessagePtr;

CWilliam()
{
	m_fMax = 0;
	m_fMin = -100;
}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	COLORREF m_crBackground;
};

class CHurst : public CBLPOscillatorStudy
{
public:
typedef struct tagStructStudyMessageBuf {
	short msg_id;
	short instance_id;
	short base_id;
	short period;
	char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
    ChartLineParameters line_params[1];
    BYTE version;
    //--------version 0
    BYTE skip[3];
	Chart_Color_Scheme chart_colors;
    ChartPointParameters point_params[1];
    //--------version 1
    short window_owner_id;
    char window_index;
    char rreserved;
    //--------version 2
}  *StudyMessagePtr;

CHurst()
{
	m_fMax = 1;
	m_fMin = 0;
}
// IBLPStudy overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

protected:
	short m_nPeriod;
	COLORREF m_crBackground;

private:
	void HurstExponents(short LookBack, long NumberOfInputPoints, float* InputData, float* OutputData);
};

#endif // !defined(AFX_LEGACYSTUDIES_H__2239355A_2ACD_4CF0_98CD_84305570DAF4__INCLUDED_)
