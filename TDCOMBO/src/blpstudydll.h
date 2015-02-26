//#include "blpstudy.h"
//#ifdef _BLPSTUDYDLL_H_
//#define _BLPSTUDYDLL_H_

#include "blpstudydll2.h"

class CStudyDllExponentialMAVG : public CStudyDll
{

private:
	int m_nPeriod;

protected:

public:
	CStudyDllExponentialMAVG(){m_nPeriod = 0;}
	virtual ~CStudyDllExponentialMAVG(){}

	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	float *m_pfEMAVG;
};

//CStudyDllOBV no longer derive from CStudyDll
class CStudyDllOBV : public CBLPOscillatorStudy
{
 private:

 protected:
    typedef struct tagStructStudyMessageBuf {
	//--------version 0
	short msg_id;   //	OBV 1201
	short instance_id;
	short base_id;
	short numbars;
	short show_OBVZero; 
	ChartLineParameters line_params[1];
	BYTE version;
	//--------version 1
	BYTE skip[3];
	Chart_Color_Scheme chart_colors;
	ChartPointParameters point_params[1];
	//--------version 2
	short window_owner_id;
	char window_index;
	char rreserved;
    } *StudyMessagePtr;

 public:
    virtual ~CStudyDllOBV(){}
    virtual HRESULT ConvertBuf(void * pBuf);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
 protected:
    short m_nBarnum,m_showZero;
    ChartLineParameters m_pOBVLineParameters[1];
    ChartPointParameters m_pOBVPointParameters[1];
};


class CStudyDllSimpleMAVG : public CStudyDll
{

private:
	int m_nPeriod;

protected:

public:
	CStudyDllSimpleMAVG(){m_nPeriod = 0;}
	virtual ~CStudyDllSimpleMAVG(){}

	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	float *m_pfMAVG;
};


class CStudyDllRSI : public CStudyDll
{

private:
	int m_nPeriod;

protected:

public:
	CStudyDllRSI(){m_nPeriod = 0;}
	virtual ~CStudyDllRSI(){}

	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	float *m_pfRSI;
	short m_nStartBar;
	float *m_pfOverBought;
	float *m_pfOverSold;
	short m_nOverBought;
	short m_nOverSold; 
	char m_field_id[9];
	ChartLineParameters m_pLineParameters3[3];
	ChartPointParameters m_pPointParameters3[3];

};



class CStudyDllTAS : public CBLPOscillatorStudy //public CStudyDll
{

private:
	int m_nKPeriod;
	int m_nDPeriod;
	int m_nDSPeriod;
	int m_nDSSPeriod;
	short m_nOverBought;
	short m_nOverSold;

protected:
	typedef struct tagStructStudyMessageBuf {
		short msg_id;  // 1204
		short instance_id;
		short base_id;
		short period1;
		short period2;
		short period3;
		short period4;
		short show_tas_fast;
		short show_tas_slow;
		short show_tas_d_ds;
    	ChartLineParameters line_params[4];
		OscillatorParameters zone;
		BYTE version;
    	//--------version 0
    	BYTE skip[3];
		Chart_Color_Scheme chart_colors;
		Chart_Color_Scheme second_chart_colors;
		Chart_Color_Scheme third_chart_colors;
    	ChartPointParameters point_params[4];
    	//--------version 1
    	short window_owner_id;
    	char window_index;
    	char rreserved;
		//--------version 2
	  } *StudyMessagePtr;

	typedef struct tagOldStructStudyMessageBuf{
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
	} *OldStudyMessagePtr;

public:
	CStudyDllTAS()
	{
		m_fMax = 110;
		m_fMin = -10;
	}
	virtual ~CStudyDllTAS(){}

	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	float *m_pfK;
	float *m_pfD;
	float *m_pfDS;
	float *m_pfDSS;
	int m_show_k_d;
	int m_show_d_ds;
	int m_show_ds_dss;
	ChartLineParameters m_pK_D_LineParameters[4];
	CComPtr<IChart> m_pK_D_Chart;

	ChartLineParameters m_pD_DS_LineParameters[4];
	CComPtr<IChart> m_pD_DS_Chart;

	ChartLineParameters	m_pDS_DSS_LineParameters[4];
	CComPtr<IChart> m_pDS_DSS_Chart;

	ChartPointParameters m_pK_D_PointParameters[4];
	ChartPointParameters m_pD_DS_PointParameters[4];
	ChartPointParameters m_pDS_DSS_PointParameters[4];

	COLORREF m_crBackgroundKDchart;
	COLORREF m_crBackgroundDDSchart;
	COLORREF m_crBackgroundDSDSSchart;
};


//CStudyDllCHKO no longer derive from CStudyDll
class CStudyDllCHKO : public CBLPOscillatorStudy
{
 private:

 protected:
    typedef struct tagStructStudyMessageBuf {
	//--------version 0
	short msg_id;   //	OBV 1205
	short instance_id;
	short base_id;
	short show_CHKOZero; 
	ChartLineParameters line_params[1];
	BYTE version;
	//--------version 1
	BYTE skip[3];
	Chart_Color_Scheme chart_colors;
	ChartPointParameters point_params[1];
	//--------version 2
	short window_owner_id;
	char window_index;
	char rreserved;
    } *StudyMessagePtr;

 public:
    virtual ~CStudyDllCHKO(){}
    virtual HRESULT ConvertBuf(void * pBuf);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
 protected:
    short m_showZero;
    ChartLineParameters m_pCHKOLineParameters[1];
    ChartPointParameters m_pCHKOPointParameters[1];	
};


class CStudyDllROC : public CStudyDll
{
private:	
	short m_nPeriod1,m_nPeriod2,m_nPeriod3,m_nPeriod4;
protected:

public:
	virtual ~CStudyDllROC(){}
	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	float *m_pfROC1,*m_pfROC2,*m_pfROC3,*m_pfROC4;
	short m_lines_to_draw;
	char m_field_id[9];

	ChartLineParameters m_pROCLineParameters[4];
	ChartPointParameters m_pROCPointParameters[4];	
};



class CKeltnerBands : public CBLPStudy2{
 public:
	CKeltnerBands()
	{
		m_nVersion = 0;
		m_nBandPeriod = 10;
		m_nMiddlePeriod = 10;
		m_fUpperBandPercent = 100.0;
		m_fLowerBandPercent = 100.0;
	}
	virtual ~CKeltnerBands(){}

    virtual HRESULT ConvertBuf(void *pBuf);
    virtual HRESULT CreateStudy(IBLPGraphStudyList *pGraph);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
 private:
	short m_nVersion;
	short m_nBandPeriod;        // Band moving averag Period
    short m_nMiddlePeriod;      // Middle moving averag period
	float m_fUpperBandPercent;  // Upper band percent
	float m_fLowerBandPercent;  // Lower band percent
 protected:

    typedef struct tagStructStudyMessageBuf {
		short msg_id;
		short instance_id;
		short base_id;
		short version;

		short band_period;
		short middle_period;

		float upper_band_percent;
		float lower_band_percent;

		ChartLineParameters line_params[3];
		ChartPointParameters point_params[3];
    } *StudyMessagePtr;
   
};



//#endif
