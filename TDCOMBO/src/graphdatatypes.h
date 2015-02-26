#ifndef GRAPHDATATYPES__H
#define GRAPHDATATYPES__H

#include "graphdata.h"

// last changed:	01/07/99 DW added blgYield
//					01/14/99 DW added blgGIPPreOpen
//					02/12/99 DW added blgNWLastClosePrice
//					04/20/99 DW annotate struct ToolbarDefinitionHeader
//					04/22/99 DW add blgHostEcho, HostEcho
//					05/04/99 DW revised CustomStudyLegendLines
//					06/03/99 DW added blgSecurityName for new "NEWS" button
//					06/09/99 DW added blgUserClick message to be sent to Big
//					07/12/99 DW fix annonation on CustomStudyManagerLineNames
//					07/16/99 DW fix annonation on TrendLineCreationInformation
//					07/29/99 DW add blgGIPMarketClose
//					08/09/99 DW add calibration_scale to YAxisDisplayFormat
//					08/16/99 DW add blgPlotParameters and PlotParameters
//					08/17/99 DW add show_study... and show_legend to GlobalChartParameters
//					08/18/99 DW remove monid from PlotParameters, add new msg blgPlotMonids
//					08/23/99 DW add MultiLastClosePrice
//					08/26/99 DW add percent_change_graph to PlotParameters
//					09/16/99 DW add new HorizLine message
//					11/11/99 DW add new ShowHideToolbar message

enum /* Message Types */
{
	blgAddDataHistory = 1,
	blgSetChartStyle =2,
	blgSetBackGroundColor =3,
	blgSetBorderStyle=4,
	blgSetBorderWidth=5,
	blgSetChartArea=6,
	blgSetForeGroundColor=7,
	blgSetGrid=8,
	blgSetLineColor=9,
	blgSetLineType=10,
	blgSetLineWidth=11,
	blgSetPlotAreaLocation=12,
	blgSetPlotAreaLocationDefault=13,
	blgSetGridColor = 14,
	//blgSetStudyDefaults = 15, unused
	blgSetFieldIds = 16,
	blgViewRangeHistorical = 17,
	blgShowChart = 18,
    blgAddDataIntraday = 19,
	blgIntradayParameter = 20,
	blgHistoricalParameter = 21,
	blgViewRangeIntraday = 22,
    blgRemoveStudy = 23,
    blgGlobalChartParameters = 24,
    blgEnableAdvancedUI = 25,
    blgDisableAdvancedUI = 26,
    blgYAxisDisplayFormat = 27,
	blgFibonacciParameters = 28,
	blgLockChart = 29,
	blgUnLockChart = 30,
	blgSetTraceLevel = 31,
	blgUploadTraceFile = 32,
	blgLocalizedStringsUnused = 33,
	blgToolbarDefinition = 34,	// 1999/12/3 - LM - DW thinks this is deprecated
	blgLocalizedStringsWithCountAndEncoding = 35,
	blgToolbarDefinitionWithCountAndEncoding = 36,
    blgAdvancedUIFlags = 37,
	blgAddDataGIP = 38,
    blgAddLegendLine = 2004, //alias for blgCustomStudyLegendLines
    blgResetLegendLines = 2005, //alias for blgCustomStudyResetLegendLines
    blgGlobalChartParametersNew = 39,
    blgGIPParameter = 40,
    blgPassControlSize = 41,
    blgSetSplitRatio = 42,
    blgShowHideLegend = 43,
    blgAddTextLabel = 44,
    blgTrendLineChangeGUID = 45,
	blgGIPPreOpen = 46,
	blgNWLastClosePrice= 47,
	blgHostEcho= 48,
	blgSecurityName= 49,
	blgUserClick= 50,
	blgGIPMarketClose= 51,
	blgMultiLastClosePrice= 52,
	blgHorizLine= 53,
	blgShowHideToolbar= 54,
	blgShowValueTable = 55
};


enum //custom study managers
{
    blgCustomStudyParameters = 2000,
    blgCustomStudyName = 2001,
    blgCustomStudyLineNames = 2002,
    blgCustomStudyLineStyles = 2003,
    blgCustomStudyLegendLines = 2004,
    blgCustomStudyResetLegendLines = 2005,
    blgCustomStudySetChartParameters = 2006,
	blgPlotParameters = 2007,
	blgPlotMonids = 2008,
	blgPassthruStudy = 2009
};

enum /*Study types*/
{
    blgPrice = 1000,
	blgSimpleMAVG = 1001,
	blgExponentialMAVG = 1002,
	blgRSI = 1003,
	blgPTPS = 1004,
	blgROC = 1005,
	blgWilliam = 1006,
	blgChaikin = 1007,
	blgOnBalanceVolume = 1008,
	blgDMI = 1009,
	blgMACD = 1010,
	blgTAS = 1011,
	blgMAE = 1012,
	blgBOLL = 1013,
	blgGOC = 1014,
	blgCMCI = 1015,
	blgVolume = 1016,
	blgHURST = 1017,
	blgCreateTrendLine = 1018,
	blgGannLines = 1019,
	blgFibbonacci = 1020,
	blgTE = 1021,
    blgPriceGraph = 1022,
	blgYield= 1023,
	blgTriangularMAVG = 1024,
	blgVariableMAVG = 1025,
	blgWeightedMAVG = 1026,
	blgSwingChart = 1027,
/////////////////////////////////////////////////////////////
// Start of DeMark Message Id's passed down from the BIG
	blgTDFirst = 1028,
// DeMark Studies prior to Fortunato contract
	blgTDREI = 1028,
	blgTDMAVG1 = 1029,
	blgTDRangeProjections = 1030,
// jfk: Tom DeMark Study Agreement per Ben Fortunato 
//		April 30, 2000 Studies (in Production July 7, 2000)
	blgTDChannel1 = 1031,
	blgTDChannel2 = 1032,
	blgTDDeMarker1 = 1033,
//		September 30, 2000 Studies
//               (in production August 7, 2000)
	blgTDPressure = 1035,
	blgTDSequential = 1036,
	blgTDCombo = 1037, 
	blgTDLines = 1038,
//               (in production September 7, 2000?????)
	blgTDDeMarker2 = 1034,
//
	blgTDRelativeRetracement = 1039,
	blgTDAbsoluteRetracement = 1040,
	blgTDTrendFactor = 1041,
	blgTDRelativeRetracementDown = 1039,
	blgTDAbsoluteRetracementDown = 1040,
	blgTDTrendFactorDown = 1041,
	blgTDRelativeRetracementUp = 1042,
	blgTDAbsoluteRetracementUp = 1043,
	blgTDTrendFactorUp = 1044,
// Study added after Fortunato contract
	blgTDWave = 1045,
// Extra TDLines message to accomadate request by Tom Demark to allow
// confirmation of TD Points with closing price instead of high/low
	blgTDLinesEx = 1046,

	blgTDLast = 1100,
/////////////////////////////////////////////////////////////
	blgPolynomialFit = 1101,
////////////////////////////////////////////////////////////
	blgStudyDllFirst = 1200,
	blgStudyDllLast  = 1400,
////////////////////////////////////////////////////////////
// Message sent back to the BIG when a date is picked via mouse
	blgDateCallback = 1028
};


enum /*StructuredMessage types*/
{
	blgControllerTick = 3
};



#define FIELD_LEN 8

#define HOLE -999999.0
#define HOLEF -999999.0f
#define INTERPOLATE -999998.0
#define INTERPOLATEF -999998.0f

#define INVALID_STUDY_ID_FROM_BIG ((short)0xFFFF)
#define INVALID_STUDY_ID_FROM_BIG_PRICE_WINDOW ((short)0xFFFE)
#define INVALID_STUDY_ID_FROM_BIG_NEW_WINDOW ((short)0xFFFD)

#define VOLUME_CHART ((short)0xFFFC)

#define LATEST_CONTROL_HEADER_VERSION 2

///////////////////////////////////////////////
//Globally used structures
///////////////////////////////////////////////

typedef struct
{
	short line_width;
	short line_style;
	long line_color;
} ChartLineParameters;

typedef struct
{
	short point_style;
    short point_width;
	long point_color;
} ChartPointParameters;


typedef struct
{
	short overbought;
	short oversold;
} OscillatorParameters;

typedef struct
{
  ChartLineParameters major_grid;
  ChartLineParameters minor_grid;
  long chart_background;
  long chart_foreground; //not used now, may use later for chart text
  long yaxis_background;
  long yaxis_foreground;
  long legend_background;
  long legend_foreground; //not used now, may use later for legend text not linked to a line
  long legend_border;
} Chart_Color_Scheme;

typedef struct
{
    short msg_id;
    short instance_id;
    short base_id;
} ChildStudyHeader;

#endif
