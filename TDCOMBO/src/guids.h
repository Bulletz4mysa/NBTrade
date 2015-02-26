//////////////////////////////////////////////////////////////////////////////////
// jfk: Graph Interfaces
//		IBLPGraphData provides access to High, Low, Open, Close and Volume data;
//					  methods are provided to take Moving and Exponential Averages
// {5C264BF6-0EDE-11D4-BB9F-009027C31600}
//BLPINTERFACE("5c264bf6-0ede-11d4-bb9f-009027c31600")
//IBLPGraphData : public IUnknown
// {899A83E6-963F-11D4-BBD1-00D0B76AB717}
#ifndef _GUIDS_H_
#define _GUIDS_H_

#include "graphdata.h"
#include "graphdatatypes.h"

DEFINE_GUID(IID_IBIGObjectFactory, 
0x899a83e6, 0x963f, 0x11d4, 0xbb, 0xd1, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBIGObjectFactory : public IUnknown
{
public:
	virtual HRESULT CreateInstance(unsigned long MsgId, IUnknown **ppUnk) = 0;
};

// {4D4597D8-D915-11D5-9BBA-00D0B76AB717}
DEFINE_GUID(IID_IStudyData, 
0x4d4597d8, 0xd915, 0x11d5, 0x9b, 0xba, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IStudyData : public IUnknown
{
public:
	virtual HRESULT get_StudyDataPoints(float ***ppfDataPoints) = 0; //pure
	virtual HRESULT get_StudyButtonId(int *pButtonId) = 0; //pure
	virtual HRESULT AddRetracementButton(LPCTSTR ButtonText) = 0; //pure
	virtual HRESULT put_StudyStyle(short StudyLineIdx, STUDY_STYLE style) = 0; //pure
	virtual HRESULT put_NumStudyPoints(long NumPoints) = 0; //pure
	virtual HRESULT put_StudyDataLine(short LineNum, long NumPoints, float *pfDataPoints) = 0; //pure
	virtual HRESULT put_ChartHint(float hint_high, float hint_low) = 0; //pure
};

class IChart;
// {70001FFF-6C7C-425A-8C76-707ECE4BB02E}
DEFINE_GUID(IID_IChartList, 
0x70001fff, 0x6c7c, 0x425a, 0x8c, 0x76, 0x70, 0x7e, 0xce, 0x4b, 0xb0, 0x2e);
class IChartList : public IUnknown
{
public:
	virtual HRESULT get_ChartCount(short *nCount) = 0;
	virtual HRESULT get_Chart(short Index, IChart **piChart) = 0;

	virtual HRESULT AddChart(IChart *iChart) = 0;
	virtual HRESULT RemoveChart(IChart *iChart) = 0;
	virtual HRESULT RemoveAllCharts() = 0;
};

class IBLPGraphData;
class IBLPGraphStudyList;
DEFINE_GUID(IID_IBLPStudy,
0xC75F4A82,0xC766,0x11D3, 0xBB,0x99,0x00,0x90,0x27,0xC3,0x16,0x00);
/////////////////////////////////////////////////////////////////////////////
// CBLPStudy command target
class IBLPStudy : public IUnknown
{
public:
// Study specific ConvertBuf initializes data members during object creation
	virtual HRESULT ConvertBuf(void *pBuf) = 0; //pure
	// Interface - PURE (= 0) virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph) = 0; //pure
	virtual HRESULT CreateStudy(IBLPGraphStudyList *pGraph) = 0; //pure
	virtual HRESULT RemoveStudy(IBLPGraphStudyList *pGraph) = 0; //pure
};

// {FF9BAF09-B3BD-4552-8978-6E3E9AE2499F}
DEFINE_GUID(IID_IGraphLegendLine, 
0xff9baf09, 0xb3bd, 0x4552, 0x89, 0x78, 0x6e, 0x3e, 0x9a, 0xe2, 0x49, 0x9f);
class IGraphLegendLine : public IUnknown
{
public:
	virtual HRESULT get_MonitoringString(short LineNum, char *LegendString[]) = 0;
	virtual HRESULT get_TrackingString(short LineNum, long BarNum, char *LegendString[]) = 0;
	virtual HRESULT get_MonitoringValue(short LineNum, float *LegendValue) = 0;
	virtual HRESULT get_TrackingValue(short LineNum, long BarNum, float *LegendString) = 0;
};

class IBLPOscillator;
// {C80E4E45-E4FB-11D5-9BC0-00D0B76AB717}
DEFINE_GUID(IID_IGraphCharts, 
0xc80e4e45, 0xe4fb, 0x11d5, 0x9b, 0xc0, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IGraphCharts : public IChartList
//					 public IBLPGraphStudyList// causes ambiguous conversion to IUnknown
{
public:
	virtual HRESULT CreateLineChart(IBLPStudy *iStudy, IChart **piChart) = 0;
	virtual HRESULT CreateOHLCChart(IBLPStudy *iStudy, IChart **piChart) = 0;
	virtual HRESULT CreateOscillatorChart(short InstanceId, short ChartId, IBLPOscillator *iOscillatorValues, IChart **piChart) = 0;

#if 1
//IBLPGraphStudyList repeated here to prevent ambiguous multiple inheritance
	virtual HRESULT AddStudy(short instance_id, short base_id, char * field_id, int lineCount, int showCount,
					ChartLineParameters *lineparms,
					ChartPointParameters*pointparms, IStudyData ** ppSD, IBLPStudy *pStudy) = 0; //pure // generic create a study func

	virtual HRESULT AddStudyExtra(short instance_id, int dataIdx, int lineCount,
							ChartLineParameters *lineparm,
							ChartPointParameters *pointparm, IStudyData **ppSD) = 0; //pure // create an extra window for a study
	virtual HRESULT DeleteStudy(IStudyData *pSD) = 0;

	virtual HRESULT RemoveAllStudies() = 0;
#endif
};

class IChartLine;
// {7CD74133-E5CF-11D5-9BC0-00D0B76AB717}
DEFINE_GUID(IID_IBLPOscillator, 
0x7cd74133, 0xe5cf, 0x11d5, 0x9b, 0xc0, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBLPOscillator : public IUnknown
{
public:
	virtual HRESULT get_Max(float *fRetVal) = 0;
	virtual HRESULT put_Max(float fNewVal) = 0;
	virtual HRESULT get_Overbought(float *fRetVal, IChartLine **piLineParameters) = 0;
	virtual HRESULT put_Overbought(float fNewVal, IChartLine *iLineParameters) = 0;
	virtual HRESULT get_Oversold(float *fRetVal, IChartLine **piLineParameters) = 0;
	virtual HRESULT put_Oversold(float fNewVal, IChartLine *iLineParameters) = 0;
	virtual HRESULT get_Min(float *fRetVal) = 0;
	virtual HRESULT put_Min(float fNewVal) = 0;

	virtual HRESULT get_LineCount(short *pnCount) = 0;
	virtual HRESULT get_OscillatorLine(short nLineNum, IChartLine **piLine) = 0;
	virtual HRESULT put_OscillatorLine(short nLineNum, IChartLine *iLine) = 0;

	virtual HRESULT get_DurationArea(IChartLine **piTopLine, IChartLine **piBottomLine) = 0;
	virtual HRESULT put_DurationArea(IChartLine *iTopLine, IChartLine *iBottomLine) = 0;
};

// {8C574728-DDC8-11D5-9BBA-00D0B76AB717}
DEFINE_GUID(IID_IChartLine, 
0x8c574728, 0xddc8, 0x11d5, 0x9b, 0xba, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IChartLine : public IUnknown
{
public:
	virtual HRESULT get_LineColor(long *pnColorRef) = 0;
	virtual HRESULT put_LineColor(long nColorRef) = 0;
	virtual HRESULT get_LineStyle(short *pnStyle) = 0;
	virtual HRESULT put_LineStyle(short nStyle) = 0;
	virtual HRESULT get_LineWidth(short *pnWidth) = 0;
	virtual HRESULT put_LineWidth(short nWidth) = 0;
	virtual HRESULT get_PointColor(long *pnColorRef) = 0;
	virtual HRESULT put_PointColor(long nColorRef) = 0;
	virtual HRESULT get_PointStyle(short *pnStyle) = 0;
	virtual HRESULT put_PointStyle(short nStyle) = 0;
	virtual HRESULT get_PointWidth(short *pnWidth) = 0;
	virtual HRESULT put_PointWidth(short nWidth) = 0;

	virtual HRESULT get_DataPoints(long nFirst, long nLast, float *pDataPoints[/*1 + nLast - nFisrt*/]) = 0;
	virtual HRESULT put_DataPoints(long nCount, float DataPoints[/*nCount*/]) = 0;
};

// {BF2F5EF1-E371-11D5-9BBD-00D0B76AB717}
DEFINE_GUID(IID_IyAxisData, 
0xbf2f5ef1, 0xe371, 0x11d5, 0x9b, 0xbd, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IyAxisData : public IUnknown
{
public:
	virtual HRESULT get_yaxis_D(long *RetVal) = 0;
	virtual HRESULT put_yaxis_D(long newVal) = 0;
	virtual HRESULT get_yaxis_LZS(long *RetVal) = 0;
	virtual HRESULT put_yaxis_LZS(long newVal) = 0;
	virtual HRESULT yaxis_shortm(long *RetVal) = 0;
	virtual HRESULT yaxis_shortm(long newVal) = 0;
	virtual HRESULT get_AxisBackgroundColor(long *ColorRef) = 0;
	virtual HRESULT put_AxisBackgroundColor(long newColorRef) = 0;
	virtual HRESULT get_AxisForegroundColor(long *ColorRef) = 0;
	virtual HRESULT put_AxisForegroundColor(long newColorRef) = 0;

	virtual HRESULT AddChartLine(IChartLine *iChart) = 0;
	virtual HRESULT RemoveChartLine(IChartLine *iChart) = 0;
};

// {4D4597D7-D915-11D5-9BBA-00D0B76AB717}
DEFINE_GUID(IID_IChart, 
0x4d4597d7, 0xd915, 0x11d5, 0x9b, 0xba, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IChart : public IUnknown
{
public:
	virtual HRESULT get_BackgroundColor(long *ColorRef) = 0;
	virtual HRESULT put_BackgroundColor(long newColorRef) = 0;
	virtual HRESULT get_BarColor(long *ColorRef) = 0;
	virtual HRESULT put_BarColor(long newColorRef) = 0;
	virtual HRESULT get_ChartStyle(long *ChartStyle) = 0;
	virtual HRESULT put_ChartStyle(long newChartStyle) = 0;
	virtual HRESULT get_Visible(BOOL *pbVisible) = 0;
	virtual HRESULT put_Visible(BOOL bVisible) = 0;

	virtual HRESULT get_YAxisData(short nIndex, IyAxisData **ppYAxis) = 0;
	virtual HRESULT put_YAxisData(short nIndex, IyAxisData *pYAxis) = 0;
};

DEFINE_GUID(IID_IBLPGraphData, 
0x5c264bf6, 0xede, 0x11d4, 0xbb, 0x9f, 0x0, 0x90, 0x27, 0xc3, 0x16, 0x0);

class IBLPGraphData : public IUnknown
{
public:
	// Data Array properties
    virtual HRESULT get_NumPoints(long *pVal) = 0;    
    virtual HRESULT get_NumFuturePoints(long *pVal) = 0;    
    virtual HRESULT put_NumFuturePoints(long newVal) = 0;    

    virtual HRESULT get_HighData(long nNumPoints, float *pVal[  ]) = 0;
    virtual HRESULT get_TrueHighData(long nNumPoints, float *pVal[  ]) = 0;
    virtual HRESULT get_LowData(long nNumPoints, float *pVal[  ]) = 0;    
    virtual HRESULT get_TrueLowData(long nNumPoints, float *pVal[  ]) = 0;    
    virtual HRESULT get_OpenData(long nNumPoints, float *pVal[  ]) = 0;    
    virtual HRESULT get_CloseData(long nNumPoints, float *pVal[  ]) = 0; 
    virtual HRESULT get_VolumeData(long nNumPoints, float *pVal[  ]) = 0;     
    virtual HRESULT get_CalcRouteData(LPCTSTR zCalcRouteId, long nNumPoints, float *pVal[  ]) = 0; 

	// Study generated Trend Lines
	virtual HRESULT AddStudyTrendLine(long x1, float y1, long x2, float y2, ChartLineParameters *pLineParameters, short type, /*[out, retval]*/ long *id) = 0;
	virtual HRESULT DeleteStudyTrendLine(long id) = 0;

	virtual HRESULT MovingAverage(long Period, long NumPoints,
					float Input[  ], float *Output[  ]) = 0;
    
    virtual HRESULT ExponentialAverage(int Period, long nNumPoints,
					float Input[  ], float *Output[  ]) = 0;

};


// {A8D4DFA6-665F-11D5-9B90-00D0B76AB717}
DEFINE_GUID(IID_ITrendlineManager, 
0xa8d4dfa6, 0x665f, 0x11d5, 0x9b, 0x90, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class ITrendlineManager : public IUnknown
{
public:
	virtual HRESULT AddStudyTrendLine(long x1, float y1, long x2, float y2, 
									  ChartLineParameters *pLineParameters, 
									  trendline_types type, /*[out, retval]*/ 
									  long *id) = 0; //pure
	virtual HRESULT DeleteStudyTrendLine(long id) = 0; //pure
	virtual HRESULT UpdateTrendLines() = 0; //pure
};

DEFINE_GUID(IID_IDatePickerEventHandler, 
0x8ecc89f3, 0x9005, 0x11d4, 0xbb, 0xce, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IDatePickerEventHandler : public IUnknown
{
public:
	virtual HRESULT Abort() = 0; //pure
	virtual HRESULT DatePicked(long nDate) = 0;//pure
};

class IBLPStudy;

DEFINE_GUID(IID_IBLPGraphStudyList, 
0x4ad5ec04, 0x4b63, 0x11d4, 0xbb, 0xc0, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBLPGraphStudyList : public IUnknown
{
public:   
	virtual HRESULT AddStudy(short instance_id, short base_id, char * field_id, int lineCount, int showCount,
					ChartLineParameters *lineparms,
					ChartPointParameters*pointparms, IStudyData ** ppSD, IBLPStudy *pStudy) = 0; //pure // generic create a study func

	virtual HRESULT AddStudyExtra(short instance_id, int dataIdx, int lineCount,
							ChartLineParameters *lineparm,
							ChartPointParameters *pointparm, IStudyData **ppSD) = 0; //pure // create an extra window for a study
	virtual HRESULT DeleteStudy(IStudyData *pSD) = 0;

	virtual HRESULT RemoveAllStudies() = 0;
};      

class IBLPRetracement;
typedef IBLPRetracement **RetracementList;
// {8E246863-79C8-11D4-BBC7-00D0B76AB717}
DEFINE_GUID(IID_IBLPGraphRetracementList, 
0x8e246863, 0x79c8, 0x11d4, 0xbb, 0xc7, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBLPGraphRetracementList : public IUnknown
{
public:
	virtual HRESULT AddRetracement(IBLPRetracement *pRetracement) = 0;
	virtual HRESULT DeleteRetracementList() = 0;
	virtual HRESULT GetRetracementList(RetracementList *pRetracementList) = 0;
	virtual HRESULT GetRetracementCount(short *pnRetracementCount) = 0;
	virtual HRESULT SetRetracementButtonState(int nIndex, int bState) = 0;
};


// {272CB473-7387-11D4-BBC7-00D0B76AB717}
DEFINE_GUID(IID_IBLPRetracement, 
0x272cb473, 0x7387, 0x11d4, 0xbb, 0xc7, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);

// {DA937F47-98AC-11D5-9B35-00D0B76AC6A7}
DEFINE_GUID(IID_IBLPGraphMenuButton, 
0xda937f47, 0x98ac, 0x11d5, 0x9b, 0x35, 0x0, 0xd0, 0xb7, 0x6a, 0xc6, 0xa7);

class IBLPGraphMenu;
class CGraphCtrl;

class IBLPGraphMenuButton : public IUnknown
{
 public:
  virtual HRESULT get_Text(BYTE *pnEncoding, LPSTR *pText) = 0; //pure
  virtual HRESULT put_Text(BYTE nEncoding, LPCTSTR Text) = 0; //pure

  virtual HRESULT get_Flags(UINT *pFlags) = 0; //pure
  virtual HRESULT put_Flags(UINT Flags) = 0; //pure
  virtual HRESULT CheckButton( UINT nCheck ) = 0; //pure
  virtual HRESULT DisableButton(UINT nDisable ) = 0; //pure

  virtual HRESULT get_Id(short *pnId) = 0; //pure
  virtual HRESULT put_Id(short nId) = 0; //pure

  virtual HRESULT get_Popup(IBLPGraphMenu ** ppPopup) = 0;
  virtual HRESULT put_Popup(IBLPGraphMenu * pPopup) = 0;

  virtual HRESULT InvokeCommand(BOOL bEnable) = 0; //pure
};

// {A8D4DFA7-665F-11D5-9B90-00D0B76AB717}
DEFINE_GUID(IID_IBLPGraphMenu, 
0xa8d4dfa7, 0x665f, 0x11d5, 0x9b, 0x90, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBLPGraphMenu : public IUnknown
{
public:
    virtual HRESULT CreateMenuButton(IBLPGraphMenuButton **ppButton) = 0; //pure
	virtual HRESULT CreatePopupMenu(IBLPGraphMenu **pMenu) = 0; //pure

	virtual HRESULT InsertMenu(UINT nPosition, UINT nFlags, IBLPGraphMenuButton *pButton) = 0; //pure
	virtual HRESULT AppendMenu(IBLPGraphMenuButton *pButton) = 0; //pure
	virtual HRESULT RemoveMenu( UINT nPosition, UINT nFlags ) = 0; //pure

	virtual HRESULT get_SubMenu( UINT nPosition, IBLPGraphMenu **pMenu,
                                 UINT nFlags) = 0; //pure
	virtual HRESULT get_Button(UINT Position, IBLPGraphMenuButton **ppButton) = 0; //pure
    virtual HRESULT get_MenuItemCount(UINT *pCount) = 0; //pure
};

// {A411937C-EA31-4E4F-89C0-480719A049B3}
DEFINE_GUID(IID_IStudyTrendlines, 
0xa411937c, 0xea31, 0x4e4f, 0x89, 0xc0, 0x48, 0x7, 0x19, 0xa0, 0x49, 0xb3);
class IStudyTrendlines : public IUnknown
{
public:
	virtual HRESULT get_TrendlineCount(short *nCount) = 0;
	virtual HRESULT get_TrendlineId(short Index, long *pnId) = 0;
};

// {DD6C1EF0-CC8C-11d5-9B31-00D0B74F39C6}
DEFINE_GUID(IID_IBLPLogo, 
0xdd6c1ef0, 0xcc8c, 0x11d5, 0x9b, 0x31, 0x0, 0xd0, 0xb7, 0x4f, 0x39, 0xc6);

// {4D4597D9-D915-11D5-9BBA-00D0B76AB717}
DEFINE_GUID(IID_IBLPGraphToolbar, 
0x4d4597d9, 0xd915, 0x11d5, 0x9b, 0xba, 0x0, 0xd0, 0xb7, 0x6a, 0xb7, 0x17);
class IBLPGraphToolbar : public IBLPGraphMenu
{
public:
	virtual HRESULT InvokeCommand(short nCmdId, BOOL bEnable) = 0;

	virtual HRESULT AddRetracementButton(BYTE encoding, const char *button_text, IBLPStudy *pStudy) = 0;
	virtual HRESULT DeleteRetracementButton(short nId) = 0;

	virtual HRESULT AddStudyButton(BYTE encoding, const char *button_text, IChartList *iChartList) = 0;
	virtual HRESULT DeleteStudyButton(short nId) = 0;
};

// {E056423B-B0F0-48B4-B9E0-E1244A5035AD}
DEFINE_GUID(IID_ITDCountdown, 
0xe056423b, 0xb0f0, 0x48b4, 0xb9, 0xe0, 0xe1, 0x24, 0x4a, 0x50, 0x35, 0xad);
class ITDCountdown : public IUnknown
{
public:
	virtual HRESULT get_CountdownDisplay(short nIndex, int **ppCountdownArray) = 0;
};

// {6F4DA955-AD22-48C2-85D8-B900D81A2100}
DEFINE_GUID(IID_IBLPPropertyPage, 
0x6f4da955, 0xad22, 0x48c2, 0x85, 0xd8, 0xb9, 0x0, 0xd8, 0x1a, 0x21, 0x0);
class IBLPPropertyPage : public IUnknown
{
public:
	virtual HRESULT Activate(HWND hWndParent, LPCRECT pRect) = 0;
	virtual HRESULT Move(LPCRECT prc) = 0;
	virtual HRESULT Deactivate() = 0;
	virtual HRESULT IsPageDirty() = 0;
	virtual HRESULT SetPropertySheet(LPUNKNOWN pUnkPropertySheet) = 0;
};

// {91FBE922-778A-4850-BEA1-6366D7EC15A0}
DEFINE_GUID(IID_IPropertySheetEvents, 
0x91fbe922, 0x778a, 0x4850, 0xbe, 0xa1, 0x63, 0x66, 0xd7, 0xec, 0x15, 0xa0);
class IPropertySheetEvents : public IUnknown
{
public:
	virtual HRESULT OnOK() = 0;
	virtual HRESULT OnCancel() = 0;
};

// {849B136E-6C7D-4725-BEA3-7BB096C1CEEA}
DEFINE_GUID(IID_IBLPPropertySheet, 
0x849b136e, 0x6c7d, 0x4725, 0xbe, 0xa3, 0x7b, 0xb0, 0x96, 0xc1, 0xce, 0xea);
class IBLPPropertySheet : public IUnknown
{
public:
	virtual HRESULT get_ActiveIndex(short *pnIndex) = 0;
	virtual HRESULT put_ActiveIndex(short nIndex) = 0;

	virtual HRESULT get_Title(LPSTR pTitle, short nMaxLength) = 0;
	virtual HRESULT put_Title(LPSTR pTitle) = 0;

	virtual HRESULT get_PageCount(short *pnCount) = 0;
	virtual HRESULT get_Page(IBLPPropertyPage **piPropertyPage) = 0;

	virtual HRESULT DoProperties(IPropertySheetEvents *iEventHandler) = 0;
	virtual HRESULT AddPage(IBLPPropertyPage *iPropertyPage) = 0;
	virtual HRESULT RemovePage(IBLPPropertyPage *iPropertyPage) = 0;
};
	
#endif

