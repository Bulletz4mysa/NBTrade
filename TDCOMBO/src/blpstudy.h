#if !defined(AFX_BLPSTUDY_H__9AB36AA4_F046_11D3_BB9D_009027C31600__INCLUDED_)
#define AFX_BLPSTUDY_H__9AB36AA4_F046_11D3_BB9D_009027C31600__INCLUDED_

//
// Copyright (c) Bloomberg L.P. $Date: 2003/03/13 16:25:50 $
// $Header: /home/cvs2/external/blpstudy.h,v 1.47 2003/03/13 16:25:50 lmoser Exp $
//
#include "stdafx.h"
#ifndef _BLPSTUDY_H
#include <winsock2.h>
#define _BLPSTUDY_H
#include "compileropts.h"
#include "blpvector.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BLPStudy.h : header file
//
#include "blpinterface.h"
#include "graphdatatypes.h"
#include "fstring.h"
#ifdef _BLPSTUDY_

#ifdef BLPUNIX
#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
         struct _GUID name \
                 = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6, b7,  b8 } }
#else
//#include <initguid.h>
#endif // BLPUNIX

#endif //_BLPSTUDY_

#ifdef PURE
#undef PURE
#endif
#define PURE = 0

#define DELTA 1E-10

#include "guids.h"

class CBLPStudy2;

#ifndef BLPUNIX
class CString : public CFastString
{
};
#endif

enum /* Color types */
{
    blgBlack =  0x0,                            /* 0 */
    blgDarkBlue = 0x00410000,                   /* 1 */
    blgAmber = 0x000080FF,                      /* 2 */
    blgWhite = 0xFFFFFF,                        /* 4 */
    blgYellow = 0x0000FFFF,                     /* 6 */
    blgMagenta = 0x00FF00FF,                    /* 7 */
    blgGray =  0xC0C0C0,                        /* 8 */
    blgOrange = 0x000064F0,                     /* 9 */
    blgLightRed = 0x008080ff,                   /* 10 */
    blgPink = 0x008080ff,                       /* 10 */
    blgLime = 0x0000ff80,                       /* 11 */
    blgLightGreen = 0x0000ff80,                 /* 11 */
    blgLightBlue = 0x00ff8000,                  /* 12 */
    blgRed = 0x000000ff,                        /* 13 */
    blgGreen = 0x0000ff00,                      /* 14 */
    blgBlue = 0x00ff0000,                       /* 15 */
    blgCyan = 0x00ffff00,                       /* 17 */
    blgSteel = 0x00C0807D,                      /* 18 */
    blgDarkOlive = 0x00408080,                  /* 19 */
    blgOlive= 0x00008080,                       /* 20 */
    blgDarkRed = 0x00000056,                    /* 21 */
    blgAqua = 0x00808040                        /* 22 */

};

 
void ConvertChartColors(Chart_Color_Scheme *pParams);

class CGraphStudyFactoryStudyDll :	public IBIGObjectFactory,
									public CBLPCmdTarget
{
public:
	virtual HRESULT CreateInstance(unsigned long MsgId, IUnknown **piStudy);
	virtual LPUNKNOWN GetInterfaceHook(const void* piid);
	static CBLPStudy2* CreateCBLPStudy2(short MsgId);

	DECLARE_IUnknown()
};


struct CHARTDATA;
class CBLPStudy : public CBLPCmdTarget,
 				  public IBLPStudy
{
friend class CTDBreakoutQualifier;
public:
// enum of Price types used by a variety of studies
typedef enum tagEnumPrice {Open = 0, High = 1, Low = 2, Close = 3, TrueHigh = 4, TrueLow = 5, Midpoint = 6, HLCAverage = 7, LowHigh = 8, HighLow = 9, TrueLowTrueHigh = 10, TrueHighTrueLow = 11} enumPriceType;

public:
// Support classes for Prices with knowledge of HOLEF and INTERPOLATEF
	class CBLPPrice 
	{
	private:
		float m_fPrice;

	public:
		CBLPPrice() {m_fPrice = HOLEF;}
		CBLPPrice(float fPrice) {m_fPrice = fPrice;}
		operator float() {return m_fPrice;}
		bool operator ==(float fPrice) {return (m_fPrice == fPrice);}
		CBLPPrice &operator =(float fPrice)
		{
			m_fPrice = fPrice;
			return *this;
		}

		void operator *=(float fPrice)
		{
			*this = (*this * fPrice);
		}

		void operator +=(float fPrice)
		{
			*this = (*this + fPrice);
		}

		void operator -=(float fPrice)
		{
			*this = (*this - fPrice);
		}

		void operator /=(float fPrice)
		{
			*this = (*this / fPrice);
		}

		bool operator >(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF))
				return FALSE;
			else if ((fPrice == HOLEF) || (fPrice == INTERPOLATEF))
				return TRUE;
			else
				return m_fPrice > fPrice;
		}

		bool operator >(CBLPPrice blpPrice) {
			if ( (m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF) )
				return FALSE;
			else if ( (blpPrice.m_fPrice == HOLEF) || (blpPrice.m_fPrice == INTERPOLATEF ))
				return TRUE;
			else
				return m_fPrice > blpPrice.m_fPrice;
		}

		bool operator <(CBLPPrice blpPrice) {
			if ( (m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF) )
				return FALSE;
			else if ( (blpPrice.m_fPrice == HOLEF) || (blpPrice.m_fPrice == INTERPOLATEF ))
				return TRUE;
			else
				return m_fPrice < blpPrice.m_fPrice;
		}

		bool operator >=(CBLPPrice blpPrice) {
			return !(*this < blpPrice);
		}

		bool operator <=(CBLPPrice blpPrice) {
			return !(*this > blpPrice);
		}

		bool operator >=(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF))
				return FALSE;
			else if ((fPrice == HOLEF) || (fPrice == INTERPOLATEF))
				return TRUE;
			else
				return m_fPrice >= fPrice;
		}

		bool operator <(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF))
				return FALSE;
			else if ((fPrice == HOLEF) || (fPrice == INTERPOLATEF))
				return TRUE;
			else
				return m_fPrice < fPrice;
		}

		bool operator <=(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (m_fPrice == INTERPOLATEF))
				return FALSE;
			else if ((fPrice == HOLEF) || (fPrice == INTERPOLATEF))
				return TRUE;
			else
				return m_fPrice <= fPrice;
		}

		CBLPPrice operator *(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (fPrice == HOLEF))
				return HOLEF;
			else if ((m_fPrice == INTERPOLATEF) || (fPrice == INTERPOLATEF))
				return INTERPOLATEF;
			else
				return m_fPrice * fPrice;
		}

		CBLPPrice operator +(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (fPrice == HOLEF))
				return HOLEF;
			else if ((m_fPrice == INTERPOLATEF) || (fPrice == INTERPOLATEF))
				return INTERPOLATEF;
			else
				return m_fPrice + fPrice;
		}

		CBLPPrice operator -(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (fPrice == HOLEF))
				return HOLEF;
			else if ((m_fPrice == INTERPOLATEF) || (fPrice == INTERPOLATEF))
				return INTERPOLATEF;
			else
				return m_fPrice - fPrice;
		}

		CBLPPrice operator /(float fPrice) 
		{
			if ((m_fPrice == HOLEF) || (fPrice == HOLEF))
				return HOLEF;
			else if ((m_fPrice == INTERPOLATEF) || (fPrice == INTERPOLATEF))
				return INTERPOLATEF;
			else
				return m_fPrice / fPrice;
		}

	};

public:
	static float *GetStudyChartLine(IBLPStudy *iStudy, int nChartIndex, int nLineNum);
	virtual HRESULT CreateStudyLegend(){return S_OK;}

protected:
	void ConvertOscillatorParameters(OscillatorParameters *pParams)
		{
		  pParams->overbought = ntohs(pParams->overbought);
		  pParams->oversold = ntohs(pParams->oversold);
		}

	void ConvertLineParameters(ChartLineParameters *pParams, int nParamCount)
		{
		  for (int i = 0; i < nParamCount; i++)
			{
			  pParams[i].line_width = ntohs(pParams[i].line_width);
			  pParams[i].line_style = ntohs(pParams[i].line_style);
			  pParams[i].line_color = ntohl(pParams[i].line_color);
			}

		}

	void ConvertPointParameters(ChartPointParameters *pParams, int nParamCount)
		{
		  for (int i = 0; i < nParamCount; i++)
			{
			  pParams[i].point_width = ntohs(pParams[i].point_width);
			  pParams[i].point_style = ntohs(pParams[i].point_style);
			  pParams[i].point_color = ntohl(pParams[i].point_color);
			}
		}

	float *CBLPStudy::GetStudyDataLine(IBLPGraphData *pGraph, enumPriceType type)
		{
			float *pData;
			long nNumPoints;
			pGraph->get_NumPoints(&nNumPoints);
			switch (type) {
			  case High :
				  if ( pGraph->get_HighData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  case Low :
				  if ( pGraph->get_LowData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  case Open :
				  if ( pGraph->get_OpenData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  case Close :
				  if ( pGraph->get_CloseData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  case TrueHigh :
				  if ( pGraph->get_TrueHighData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  case TrueLow :
				  if ( pGraph->get_TrueLowData(nNumPoints, &pData) != NOERROR) {
					pData = NULL;
					}
				break;

			  default:
					pData = NULL;
			}

			return pData;		
		}
    long mapidtocolor(short nColorVal)
	{
		long color;

		switch (nColorVal) 
		{
		case 0:
			color = blgBlack;
			break;
		case 1:
			color = blgDarkBlue;
			break;
		case 2 :
			color = blgAmber;
			break;
		case 4:
			color = blgWhite;
			break;
		case 6 :
			color = blgYellow;
			break;
		case 7 :
			color = blgMagenta;
			break;
		case 8 :
			color = blgGray;
			break;
		case 9 :
			color = blgOrange;
			break;
		case 10 :
			color = blgLightRed;
			break;
		case 11:
			color = blgLightGreen;
			break;
		case 12 :
			color = blgLightBlue;
			break;
		case 13 :
			color = blgRed;
			break;
		case 14 :
			color = blgGreen;
			break;
		case 15 :
			color = blgBlue;
			break;
		case 17 :
			color = blgCyan;
			break;
		case 18 :
			color = blgSteel;
			break;
		case 19 :
			color = blgDarkOlive;
			break;
		case 20 :
			color = blgOlive;
			break;
		case 21 :
			color = blgDarkRed;
			break;
		case 22 :
			color = blgAqua;
			break;
		default :
			color = blgRed;
			break;
		}

		return color;
	}

	float *GetStudyCalcRoute(IBLPGraphData *pGraph, LPCTSTR zCalcRouteId)
		{
			float *pData;
			HRESULT hr;
			pGraph->get_NumPoints(&m_nNumPoints);
			if (strcmp("TD007", zCalcRouteId) == 0)
				hr = pGraph->get_TrueHighData(m_nNumPoints, &pData);
			else if (strcmp("TD008", zCalcRouteId) == 0)
				hr = pGraph->get_TrueLowData(m_nNumPoints, &pData);
			else
				hr = pGraph->get_CalcRouteData(zCalcRouteId, m_nNumPoints, &pData);
				
			if (hr != NOERROR) {
				pData = NULL;
				}

			return pData;
		}

	BOOL IsInvalid(float);
	BOOL IsInvalid(float,float,float,float);
	BOOL IsInvalid(float[], float[], float[], float[], long index); 
	void GetDataRange(int nNumpoints, float pfData[], float * pfMin, float * pfMax);

	//These two functions will go through pfData from nStart to nEnd to get
	//the Bar with maximum/minimum value.
	//nStart and nEnd must be valid index for pfData
	//nStart >=0, nEnd >= nStart, nEnd <= m_nNumPoints - 1
	//It will return - 1 if from nStart to nEnd, there is no valid data
	//If there are Bars of the same max/min value, it will return the first
	//one.
	//Both nStart and nEnd Bar are counted.
	long GetBarOfMax(float pfData[], long nStart, long nEnd);
	long GetBarOfMin(float pfData[], long nStart, long nEnd);

	void ScreenForMissing(float **ppfData,
			      long nLines,
			      long nNumPoints,
			      long *pnBadIndex,
			      long *nBadNumPoints);

	void MergeGoodValuesWithMissing(float **pComputedData,
					long nLines,
					long nNumPoints,
					long *pnBadIndex,
					long nBadNumPoints);

	void MergeGoodValuesWithMissing(float *pfComputedData,
					long nNumPoints,
					long *pnBadIndex,
					long nBadNumPoints);
	
	void MergeGoodValuesWithMissing(int *pnComputedData,
					long nNumPoints,
					long *pnBadIndex,
					long nBadNumPoints);
	
	long GetMapedIndex(long nNumPoints,
					long *pnBadIdex,
				    long nBadNumPoints,
					long nFilteredIndex);
	    	
public:
/////////////////////////////////////////////////////////////////////////////
//MFC-like IUnknown implementation

	ULONG STDMETHODCALLTYPE AddRef() {
		return CBLPCmdTarget::AddRef();
		}

	ULONG STDMETHODCALLTYPE Release() {
		return CBLPCmdTarget::Release();
		}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppvRet) {
		return CBLPCmdTarget::QueryInterface(iid, ppvRet);		\
		}

	LPUNKNOWN GetInterfaceHook(const void* piid)
	{
		IID riid = *((IID*)piid);
		if (riid == IID_IUnknown || riid == IID_IBLPStudy)
			{
			return (IBLPStudy*)this;
			}
		return NULL;
	}
/////////////////////////////////////////////////////////////////////////////

protected:
// Attributes

// Operations
public:
//	static void Normalize(int nCount, float pfData[], float fFactor = 100.0f);
//	static BOOL IsDeMarkStudy(short nMessageId)
//	{
//		if ( (nMessageId >= blgTDFirst) && (nMessageId <= blgTDLast) )
//			return TRUE;
//		else
//			return FALSE;
//	}
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBLPStudy)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	CBLPStudy()
	{
		memset(m_zCalcRouteId, 0, sizeof(m_zCalcRouteId));
		strcpy(m_zCalcRouteId, "#0");
		m_nBaseId = 0;
		m_nInstanceId = 0;
		m_nNumPoints = 0;
		m_pLineParameters = NULL;
		m_pPointParameters = NULL;
		pSD = NULL;
	}


	virtual CBLPStudy::~CBLPStudy()
	{
	}

	// IBLPStudy 
	virtual HRESULT CBLPStudy::ConvertBuf(void * pBuf)
	{
		struct tagStudyHeader{
			short msg_id;   
			short instance_id;
			short base_id;
		  } *pHead = (struct tagStudyHeader *)pBuf;

 		pHead->instance_id = ntohs( pHead->instance_id );
		pHead->base_id = ntohs( pHead->base_id );
		m_nInstanceId = pHead->instance_id;
		m_nBaseId = pHead->base_id;
		return S_OK;
	}

protected:
	CComPtr<IStudyData> pSD;
	short m_nInstanceId;
	short m_nBaseId;
	long m_nNumPoints;
	ChartLineParameters *m_pLineParameters;
	ChartPointParameters *m_pPointParameters;
	char m_zCalcRouteId[ FIELD_LEN + 1 ];
	
	int GetStudyButtonId();
	void CBLPStudy::SetStudyDataLine(short LineNum, float *DataPoints)
	{
		ASSERT(pSD != NULL);
		HRESULT hr = pSD->put_StudyDataLine(LineNum, m_nNumPoints, DataPoints);
		ASSERT(SUCCEEDED(hr));
	}

	float *GetStudyDataPoints(unsigned short index);

public:
	virtual void Set_BaseId(short nBaseId) 
	{
		m_nBaseId = nBaseId;
	};

	virtual void Set_InstanceId(short nInstanceId)
	{
		m_nInstanceId = nInstanceId;
	}

};

class CTDBreakoutQualifier
{
public:
	CTDBreakoutQualifier()
	{
		m_bQualifier1 = TRUE;
		m_bQualifier2 = TRUE;
		m_bQualifier3 = TRUE;
	}

	virtual BOOL QualifyBreakout(IBLPGraphData *pGraphData, CBLPStudy *pStudy, BOOL bUp, long nBreakoutIndex, float fBreakoutPrice);

protected:
	BOOL  m_bQualifier1;
	BOOL  m_bQualifier2;
	BOOL  m_bQualifier3;

};

class CBLPStudy2 : public CBLPStudy,
					public IChartList
{
protected:
	BLPVector< CComPtr<IChart> > m_vCharts;
	BOOL m_bVisible;

//Title for Study Button
	CFastString m_strButtonText;

public:
	CBLPStudy2() {m_bVisible = TRUE;}
	virtual ~CBLPStudy2(){}
	typedef struct tagStructStudyMessageBuf {
	  union {	  
		  struct {
			short msg_id;   
			short instance_id;
			short base_id;
		  } StudyHeader;

		};
	} *StudyMessagePtr;

	IMPLEMENT_IUnknown(CBLPStudy2)

	BEGIN_BLPINTERFACE_MAP(CBLPStudy2, CBLPStudy)
		SUPPORT_GEN_INTERFACE(IChartList)
	END_BLPINTERFACE_MAP()

	HRESULT CreateInvisibleStudy(IBLPGraphStudyList *pGraph)
	{
		m_bVisible = FALSE;
		return CreateStudy(pGraph);
	}

	virtual HRESULT RemoveStudy(IBLPGraphStudyList *pGraph)
	{
		m_vCharts.clear();
		return 0;
	}

	virtual HRESULT get_ChartCount(short *nCount)
	{
		*nCount = (short) m_vCharts.size();
		return S_OK;
	}

	virtual HRESULT get_Chart(short Index, IChart **piChart)
	{
		ASSERT(piChart != NULL);
		if (piChart == NULL)
			return E_POINTER;

		if (m_vCharts.empty())
			return E_NOTIMPL;

		if ((Index >= 0) && (Index < (short)m_vCharts.size())) {
			*piChart = m_vCharts[Index];
			DWORD dwRef = (*piChart)->AddRef();
			return S_OK;
			}

		*piChart = NULL;
		return E_INVALIDARG;
	}

	virtual HRESULT AddChart(IChart *iChart)
	{
		m_vCharts.push_back(iChart);
		if (pSD == NULL)
			iChart->QueryInterface(IID_IStudyData, (LPVOID *)&pSD);
		if (!m_bVisible) {
			iChart->put_Visible(FALSE);
			}
		return S_OK;
	}

	virtual HRESULT RemoveChart(IChart *iChart)
	{
		BLPVector<CComPtr<IChart> >::iterator piChart;
		for (piChart = m_vCharts.begin(); piChart < m_vCharts.end(); piChart++)
			if (*piChart == iChart) {
				m_vCharts.erase(piChart);
				return S_OK;
				}

		return E_INVALIDARG;
	}

	virtual HRESULT RemoveAllCharts()
	{
		m_vCharts.clear();
		return S_OK;
	}

	virtual HRESULT AddStudy(IBLPGraphStudyList *pGraph, int lineCount = -1, int showCount = -1, ChartLineParameters *LineParms = NULL, ChartPointParameters *PointParms = NULL)
	{
		if ( (lineCount == -1)	||
			 (showCount == -1)	||
			 (LineParms == NULL) ||
			 (PointParms == NULL) )
		// new implementation will allow dynamic adding of study lines
			 return E_NOTIMPL;

		CComPtr<IGraphCharts> iGraphCharts;
		if (SUCCEEDED(pGraph->QueryInterface(IID_IGraphCharts, (LPVOID *)&iGraphCharts)))
			return iGraphCharts->AddStudy(m_nInstanceId, m_nBaseId, m_zCalcRouteId, lineCount, showCount,
						LineParms, PointParms, &pSD, this);
		else
			return pGraph->AddStudy(m_nInstanceId, m_nBaseId, m_zCalcRouteId, lineCount, showCount,
						LineParms, PointParms, &pSD, this); 
	}

	virtual HRESULT AddStudyExtra(IBLPGraphStudyList *pGraph, short nChartIndex, int nFirstLineIndex, int lineCount, ChartLineParameters *LineParms, ChartPointParameters *PointParms, IStudyData **ppSD)
	{
		CComPtr<IGraphCharts> iGraphCharts;
		if (SUCCEEDED(pGraph->QueryInterface(IID_IGraphCharts, (LPVOID *)&iGraphCharts)))
			return iGraphCharts->AddStudyExtra(m_nInstanceId * 100 + nChartIndex, nFirstLineIndex, lineCount,
						LineParms, PointParms, ppSD);
		else
			return pGraph->AddStudyExtra(m_nInstanceId * 100 + nChartIndex, nFirstLineIndex, lineCount,
						LineParms, PointParms, ppSD);
		return S_OK;
	}

};

class CChartLine : public CBLPCmdTarget,
					public IChartLine
{
protected:
	ChartLineParameters m_LineParams;
	ChartPointParameters m_PointParams;

public:
	IMPLEMENT_IUnknown(CChartLine);
	BEGIN_BLPINTERFACE_MAP(CChartLine, CBLPCmdTarget)
		SUPPORT_GEN_INTERFACE(IChartLine)
	END_BLPINTERFACE_MAP()

	CChartLine()
	{
		CBLPCmdTarget::m_dwRef = 1;
		ChartPointParameters DefaultPointParameters = {XRT_POINT_NONE};
		memcpy(&m_PointParams, &DefaultPointParameters, sizeof(DefaultPointParameters));
	}

	virtual HRESULT SetLineParams(ChartLineParameters &LineParams)
	{
		memcpy(&m_LineParams, &LineParams, sizeof(ChartLineParameters));
		return S_OK;
	}

	virtual HRESULT SetPointParams(ChartPointParameters &PointParams)
	{
		memcpy(&m_PointParams, &PointParams, sizeof(ChartPointParameters));
		return S_OK;
	}

	virtual HRESULT get_LineColor(long *pnColorRef)
	{
		ASSERT(pnColorRef != NULL);
		if (pnColorRef == NULL)
			return E_INVALIDARG;

		*pnColorRef = m_LineParams.line_color;
		return S_OK;
	}

	virtual HRESULT put_LineColor(long nColorRef)
	{
		m_LineParams.line_color = nColorRef;
		return S_OK;
	}

	virtual HRESULT get_LineStyle(short *pnStyle)
	{
		ASSERT(pnStyle != NULL);
		if (pnStyle == NULL)
			return E_INVALIDARG;

		*pnStyle = m_LineParams.line_style;
		return S_OK;
	}

	virtual HRESULT put_LineStyle(short nStyle)
	{
		m_LineParams.line_style = nStyle;
		return S_OK;
	}

	virtual HRESULT get_LineWidth(short *pnWidth)
	{
		ASSERT(pnWidth != NULL);
		if (pnWidth == NULL)
			return E_INVALIDARG;

		*pnWidth = m_LineParams.line_width;
		return S_OK;
	}

	virtual HRESULT put_LineWidth(short nWidth)
	{
		m_LineParams.line_width = nWidth;
		return S_OK;
	}

	virtual HRESULT get_PointColor(long *pnColorRef)
	{
		ASSERT(pnColorRef != NULL);
		if (pnColorRef == NULL)
			return E_INVALIDARG;

		*pnColorRef = m_PointParams.point_color;
		return S_OK;
	}

	virtual HRESULT put_PointColor(long nColorRef)
	{
		m_PointParams.point_color = nColorRef;
		return S_OK;
	}

	virtual HRESULT get_PointStyle(short *pnStyle)
	{
		ASSERT(pnStyle != NULL);
		if (pnStyle == NULL)
			return E_INVALIDARG;

		*pnStyle = m_PointParams.point_style;
		return S_OK;
	}

	virtual HRESULT put_PointStyle(short nStyle)
	{
		m_PointParams.point_style = nStyle;
		return S_OK;
	}

	virtual HRESULT get_PointWidth(short *pnWidth)
	{
		ASSERT(pnWidth != NULL);
		if (pnWidth == NULL)
			return E_INVALIDARG;

		*pnWidth = m_PointParams.point_width;
		return S_OK;
	}

	virtual HRESULT put_PointWidth(short nWidth)
	{
		m_PointParams.point_width = nWidth;
		return S_OK;
	}

	virtual HRESULT get_DataPoints(long nFirst, long nLast, float *pDataPoints[/*1 + nLast - nFisrt*/]){return E_NOTIMPL;}
	virtual HRESULT put_DataPoints(long nCount, float DataPoints[/*nCount*/]){return E_NOTIMPL;}
};

class CBLPOscillatorStudy : public CBLPStudy2,
							public IBLPOscillator 
{
protected:
	BLPVector< CComPtr<IChartLine> > m_vChartLine;
	CChartLine *m_pDurationTopLine;
	CChartLine *m_pDurationBottomLine;
	CChartLine m_OverboughtLine;
	CChartLine m_OversoldLine;
	struct _tagOverboughtOversold {
		float overbought;		
		float oversold;
		} m_zones;
	float m_fMax;
	float m_fMin;

public:
	IMPLEMENT_IUnknown(CBLPOscillatorStudy);
	BEGIN_BLPINTERFACE_MAP(CBLPOscillatorStudy, CBLPStudy2)
		SUPPORT_GEN_INTERFACE(IBLPOscillator)
	END_BLPINTERFACE_MAP()

	virtual ~CBLPOscillatorStudy(){}
protected:
	CBLPOscillatorStudy()
	{
		m_pDurationTopLine = NULL;
		m_pDurationBottomLine = NULL;

		m_OverboughtLine.put_LineColor(RGB(255, 0, 0));
		m_OverboughtLine.put_LineStyle(XRT_LPAT_SOLID);
		m_OverboughtLine.put_LineWidth(1);

		m_OversoldLine.put_LineColor(RGB(0, 255, 0));
		m_OversoldLine.put_LineStyle(XRT_LPAT_SOLID);
		m_OversoldLine.put_LineWidth(1);

		m_fMax = HOLEF;
		m_fMin = HOLEF;

		m_zones.overbought = HOLEF;
		m_zones.oversold = HOLEF;
	}

public:
	CBLPOscillatorStudy(CBLPOscillatorStudy &StudyRef)
	{
		m_pDurationTopLine = StudyRef.m_pDurationTopLine;
		m_pDurationBottomLine = StudyRef.m_pDurationBottomLine;

		m_OverboughtLine = StudyRef.m_OverboughtLine;
		m_OversoldLine = StudyRef.m_OversoldLine;

		m_fMax = StudyRef.m_fMax;
		m_fMin = StudyRef.m_fMin;

		m_zones = StudyRef.m_zones;

		ULONG nRef = AddRef();
	}

	virtual HRESULT put_Max(float fNewVal)
	{
		if ((m_fMax == HOLEF) || (m_fMin == HOLEF))
			return E_NOTIMPL;

		m_fMax = fNewVal;
		return S_OK;
	}

	virtual HRESULT get_Max(float *fRetVal)
	{
		if ((m_fMax == HOLEF) || (m_fMin == HOLEF))
			return E_NOTIMPL;

		*fRetVal = m_fMax;
		return S_OK;
	}

	virtual HRESULT get_Min(float *fRetVal)
	{
		if ((m_fMax == HOLEF) || (m_fMin == HOLEF))
			return E_NOTIMPL;

		*fRetVal = m_fMin;
		return S_OK;
	}

	virtual HRESULT put_Min(float fNewVal)
	{
		m_fMin = fNewVal;
		return S_OK;
	}

	virtual HRESULT put_Overbought(float fNewVal, IChartLine *iLineParameters)
	{
		m_zones.overbought = fNewVal;
		return S_OK;
	}

	virtual HRESULT get_Overbought(float *fRetVal, IChartLine **piLineParameters)
	{
		*fRetVal = m_zones.overbought;
		m_OverboughtLine.QueryInterface(IID_IChartLine, (LPVOID *)piLineParameters);
		return S_OK;
	}

	virtual HRESULT put_Oversold(float fNewVal, IChartLine *iLineParameters)
	{
		m_zones.oversold = fNewVal;
		return S_OK;
	}

	virtual HRESULT get_Oversold(float *fRetVal, IChartLine **piLineParameters)
	{
		*fRetVal = m_zones.oversold;
		m_OversoldLine.QueryInterface(IID_IChartLine, (LPVOID *)piLineParameters);
		return S_OK;
	}

	virtual HRESULT get_LineCount(short *pnCount)
	{
		*pnCount = m_vChartLine.size();
		return S_OK;
	}

	virtual HRESULT get_OscillatorLine(short nLineNum, IChartLine **piLine)
	{
		ASSERT(!m_vChartLine.empty());
		if (m_vChartLine.empty())
			return E_HANDLE;

		ASSERT((nLineNum < m_vChartLine.size()) && (piLine != NULL));
		if ((nLineNum >= m_vChartLine.size()) || (piLine == NULL))
			return E_INVALIDARG;

		m_vChartLine[nLineNum]->QueryInterface(IID_IChartLine, (LPVOID *)piLine);
		return S_OK;
	}

	virtual HRESULT put_OscillatorLine(short nLineNum, IChartLine *iLine){return E_NOTIMPL;}

	virtual HRESULT get_DurationArea(IChartLine **piTopLine, IChartLine **piBottomLine)
	{
		ASSERT((piTopLine != NULL) && (piBottomLine != NULL));
		if ((piTopLine == NULL) || (piBottomLine == NULL))
			return E_INVALIDARG;

		if ((m_pDurationTopLine == NULL) || (m_pDurationBottomLine == NULL))
			return E_NOTIMPL;

		*piTopLine = m_pDurationTopLine;
		*piBottomLine = m_pDurationBottomLine;
		return S_OK;
	}
		
	virtual HRESULT put_DurationArea(IChartLine *iTopLine, IChartLine *iBottomLine){return E_NOTIMPL;}

};

class IBLPRetracement : public IUnknown
{
public:
	virtual HRESULT put_Direction(BOOL bDirection) PURE;
	virtual HRESULT get_Direction(BOOL *bDirection) PURE;
	virtual HRESULT put_Date(long nDate) PURE;
	virtual HRESULT get_Date(long *nDate) PURE;
	virtual HRESULT DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph) PURE;
};

class CBLPRetracement : public CBLPStudy2,
					   public IDatePickerEventHandler,
					   public IBLPRetracement,
					   public CTDBreakoutQualifier
{
public:
/////////////////////////////////////////////////////////////////////////////
// IUnknown Implementation
	IMPLEMENT_IUnknown(CBLPRetracement)
	BEGIN_BLPINTERFACE_MAP(CBLPRetracement, CBLPStudy)
		SUPPORT_GEN_INTERFACE(IBLPRetracement)
		SUPPORT_GEN_INTERFACE(IDatePickerEventHandler)
	END_BLPINTERFACE_MAP()
/////////////////////////////////////////////////////////////////////////////

protected:
// parameters passed by the BIG for all retracement types 
	long m_nDate;
	BOOL m_bDirection;				// TRUE means Up retracement
	enumPriceType m_nProjectionPrice; // default High for downside retracement; Low for upside
	BOOL m_bCancel1;
	BOOL m_bCancel2;
	BOOL m_bCancel3;
	BOOL m_bDisplayProjections;

// internal variables used by derived Retracement classes
protected:
	BLPVector<long> m_nRetracementIds; // sutdy trendlines
	short m_nNumStudyLines; // for lines other than study trendlines
	long m_nMagnetId;

	BOOL m_bSaveTrendline;

	CComPtr<IBLPGraphData> m_pGraph;
	CFastString m_strErrorText;

// IDatePickerEventHandler
	virtual HRESULT Abort(){return NOERROR;}
	virtual HRESULT DatePicked(long nDate)
	{
		m_nDate = nDate;
		if (m_pGraph) {
			CComPtr<IBLPGraphRetracementList> pRetracementList;
			HRESULT rc = m_pGraph->QueryInterface(IID_IBLPGraphRetracementList, (void **)&pRetracementList);
			ASSERT(SUCCEEDED(rc));
			DoRetracement(m_nDate, m_bDirection, m_pGraph);
			pRetracementList->SetRetracementButtonState(GetStudyButtonId(), FALSE);
			}

		return NOERROR;
	}

public:
	virtual ~CBLPRetracement(){};

	CBLPRetracement()
	{
		m_nRetracementIds = BLPVector<long>(10);
		m_nNumStudyLines = 0;
		m_nMagnetId = 0;
		m_nDate = 0;
		m_pGraph = NULL;
		m_bDirection = TRUE;
		m_bQualifier1 = TRUE;
		m_bQualifier2 = FALSE;
		m_bQualifier3 = FALSE;
		m_bCancel1 = TRUE;
		m_bCancel2 = TRUE;
		m_bCancel3 = TRUE;
		m_bDisplayProjections = TRUE;
		m_bSaveTrendline = FALSE;
	}

//IBLPStudy Overrides
	virtual HRESULT ConvertBuf(void * pBigBuf)
	{
		CBLPStudy::ConvertBuf(pBigBuf);
		m_nDate = 0;
		return NOERROR;
	}	

	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph)
	{
		m_pGraph = pGraph;
		return NOERROR;
	}

	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph)
	{
		TRACE("CBLPRetracement::CreateStudy - Entering\n");
		int rc;
		if (m_nNumStudyLines == 0) {
			ChartLineParameters lp[1] = {{0}};
			ChartPointParameters pp[1] = {{0}};
			rc = pGraph->AddStudy(m_nInstanceId, m_nBaseId, m_zCalcRouteId, 1, 0,
							lp, pp, &pSD, this);
			}
		else
			rc = pGraph->AddStudy(m_nInstanceId, m_nBaseId, m_zCalcRouteId, m_nNumStudyLines, m_nNumStudyLines,
							m_pLineParameters, m_pPointParameters, &pSD, this);

		ASSERT(rc == 0);
		if (rc < 0)
			return rc;

		CComPtr<IBLPGraphRetracementList> pRetracementList;
		rc = pGraph->QueryInterface(IID_IBLPGraphRetracementList, (void **)&pRetracementList);
		ASSERT(SUCCEEDED(rc));

		pSD->AddRetracementButton(m_strButtonText.GetBuffer(0));	
		pRetracementList->SetRetracementButtonState(GetStudyButtonId(), FALSE);
		TRACE("CBLPRetracement::CreateStudy - Exiting\n");

		return NOERROR;
	}

	virtual HRESULT RemoveStudy(IBLPGraphStudyList *pGraph)
	{
		m_pGraph = NULL;
		return NOERROR;
	}

//IBLPRetracement
	virtual HRESULT put_Direction(BOOL bDirection)
	{
		m_bDirection = bDirection;
		m_nProjectionPrice = m_bDirection ? High : Low;
		return NOERROR;
	}

	virtual HRESULT get_Direction(BOOL *bDirection)
	{
		ASSERT(bDirection != NULL);
		if (bDirection == NULL)
			return E_POINTER;

		*bDirection = m_bDirection;
		return NOERROR;
	}

	virtual HRESULT put_Date(long nDate)
	{
		m_nDate = nDate;
		if ((m_nDate < 0) && (m_pGraph != NULL)) {
			CComPtr<ITrendlineManager> iTrendlineManager;
			m_pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineManager);
			BLPVector<long>::iterator pTrendlineId;
			for (pTrendlineId = m_nRetracementIds.begin(); pTrendlineId < m_nRetracementIds.end(); pTrendlineId++) {
				if (*pTrendlineId == 0)
					continue;
				iTrendlineManager->DeleteStudyTrendLine(*pTrendlineId);
				}
			if (m_nMagnetId != 0)
				iTrendlineManager->DeleteStudyTrendLine(m_nMagnetId);
			iTrendlineManager->UpdateTrendLines();

			m_pGraph->get_NumPoints(&m_nNumPoints);
			BLPVector<float> fAllHoles(m_nNumPoints, HOLEF);
			int nLineNum;
			for (nLineNum = 0; nLineNum < m_nNumStudyLines; nLineNum++)
				SetStudyDataLine(nLineNum, &fAllHoles[0]);

			}

		return NOERROR;
	}

	virtual HRESULT get_Date(long *pnDate)
	{
		ASSERT(pnDate != NULL);
		if (pnDate == NULL)
			return E_POINTER;

		*pnDate = m_nDate;
		return NOERROR;
	}
};

#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLPSTUDY_H__9AB36AA4_F046_11D3_BB9D_009027C31600__INCLUDED_)

