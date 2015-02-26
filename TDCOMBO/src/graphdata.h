/* GraphData.h

	Data Structure specifying everything you need to know to draw a graph,
	including pointers to:
			1. An array of time values that calibrate the x-axis
			2. Arrays of data points to be plotted

	D. Willen

	March 3 1999

	Last Changed:	03/08/99
					03/09/99	add legend box data elements to CHARTDATA
					03/12/99	add multiple Intraday sessions, struct INTRADAY_SESSION
					03/31/99	add hint_high and hint_low to CHARTDATA
								add comments about data lines and calcrout fields
					04/20/99	add toolbar_ variables to main GRAPHDATA struct
					04/22/99	enhance specification for REPLACEINFO to include study lines
					04/26/99	enhance specification for CHARTDATA to include HORIZLINE
					04/27/99	add hint_present to CHARTDATA
					05/04/99	add Fibonacci data to GRAPHDATA struct
					05/05/99	change GRAPHDATA.chartdata from array of elements to array of ptrs
					05/11/99	add internal_study_data to CHARTDATA and to LEGENDLINE
					05/12/99	added more CHART_STYLEs and added study_line_style to CHARTDATA
					05/14/99	added chart_point_style to CHARTDATA, removed study_line_style from CHARTDATA
								removed the new new enum values from CHART_STYLE
					05/18/99	for trend lines: added TRENDLINE struct, CHARTDATA.trendlines,
									and CHARTDATA.chart_id
								add chart_ and axis_ background to CHARTDATA
					05/28/99	add offset to INTRADAY_SESSION struct
					06/01/99	add type=-3 to REPLACEINFO struct
					06/03/99	trendline changes, new toolbar_id value of 11 for "NEWS" button
					06/08/99	revised spec for NotifyTrendLine (changed chart_id to chart_index)
					06/10/99	redefine GRAPHDATA.user_font_size
					06/25/99	add CHARTDATA.draw_shading
					07/02/99	add GRAPHDATA.new_toolbar_flag
					07/06/99	increased MAX_CHARTS from 6 to 10
		
					07/12/99	add CHARTDATA.yaxis_logarithmic, a BOOL flag
								which determines whether the graph should use a 
								log Y axis or a regular one - VS.
					07/19/99	add GRAPHDATA.isLocked - see comments in graphctrl.cpp
					07/20/99	add	points_allocated, was m_points_allocated in graphpump.h
					07/29/99	increase MAX_SESSIONS from 30 to 60, current BIG max in G is 30
					08/09/99	add chart_line to CHARTDATA
					08/17/99	add GRAPHDATA.ShowLegend, CHARTDATA.ShowMktClosed
					08/16/99	major re-design to handle multiple securities
					08/20/99	add security_id to REPLACEINFO struct
					08/26/99	add percent_change_graph to CHARTDATA;
								introduce two new REPLACEINFO.montype values: 6, 7
					09/01/99	annotation change
					09/02/99	new REPLACEINFO.montype values 8, 9, also added REPLACEINFO.dataline
					09/09/99	added CHARTDATA.barColor
					09/17/99	added new sec_id field to HORIZLINE struct, new const MAX_HORIZLINES
					09/30/99	increase MAX_TRENDLINE from 12 to 24
								increase MAX_SESSIONS from 60 to 70
                    10/06/99    #define EXTENDED_DATES by default for extrapolated x-axes
                    10/12/99    Remove ifdefs as well so that extended date features are standard
					10/13/99	Add GRAPHDATA.ForceNewCharts
					10/21/99	Introduce CHARTDATA.number_of_study_points to enable studies to extend
								into the future. Note this field is init'ed to -1 and normally left as
								-1 which is interpreted in chartwnd.cpp as using the main GRAPHDATA
								count of points; this allows all the existing study code to remain
								unchanged and unaware of this new feature.
					11/02/99	Introduce GRAPHDATA.number_of_future_points to be set to the largest
								CHARTDATA.number_of_study_points value in the system, or to -1, if no
								studies have extended into the future.  This will be used in chartwnd::
								ScaleX... so that all charts scale using the future points.  This new
								variable must be set and maintained in graphpump by any study that
								extends into the future.
					11/15/99	Add GRAPHDATA.ShowToolbar to indicate whether or not to show the toolbar
					11/16/99	Add gipstrip data storage to GRAPHDATA
*/

#ifndef _GRAPHDATA_H
#define _GRAPHDATA_H

typedef enum {								// point styles (from Olectra)
	XRT_POINT_HATCH_FILL = -1,
    XRT_POINT_NONE = 1,
    XRT_POINT_DOT,
    XRT_POINT_BOX,
    XRT_POINT_TRI,
    XRT_POINT_DIAMOND,
    XRT_POINT_STAR,
    XRT_POINT_VERT_LINE,
    XRT_POINT_HORIZ_LINE,
    XRT_POINT_CROSS,
    XRT_POINT_CIRCLE,
    XRT_POINT_SQUARE,
	XRT_DEMARK = 12
} XrtPoint;

typedef enum {
	DEMARK_MAGNET_QUALIFIED = -1,
	DEMARK_MAGNET_DISQUALIFIED = -2,
	// line styles (from Olectra)
    XRT_LPAT_NONE = 1,
    XRT_LPAT_SOLID,
    XRT_LPAT_LONG_DASH,
    XRT_LPAT_DOTTED,
    XRT_LPAT_SHORT_DASH,
    XRT_LPAT_LSL_DASH,
    XRT_LPAT_DASH_DOT,
	XRT_LPAT_DASH_DASH_DOT,
	XRT_TOP_FILL,
	XRT_BOTTOM_FILL
} XrtLinePattern;

// Different ways to connect the dots with studies.
enum STUDY_STYLE {
	STUDY_STYLE_PW_LINEAR,	// Default: connects adjacent points with a 
							// straight line (Piece-Wise Linear)
	STUDY_STYLE_STEP,	// Used by Elliot Refson's swing chart -
						// points are connected by moving horizontally, then
						// vertically, with no diagonal lines.
	STUDY_STYLE_OPEN_STEP	// Used by Tom DeMark Range Projections. Points
							// are represented as horizontal lines, like the
							// open/close bars on an OHLC graph, but they are
							// not connected with vertical lines.
};

typedef enum {
	line_type_placeholder = -1,
	line_type_segment,						// line segment (endpoints at points specified)
	line_type_extended_ray,					// 2extended ray (extends for entire range of chart)
	line_type_extended_forward,				// extended only forward in time 
	line_type_fibonacci_retracement,		// Fibonacci line plot
	line_type_fibonacci_projection,			// Fibonacci projection (reverse sense Fibonacci). 
// study generated trendline types
	line_type_study_fibonacci_projection,	// Fibonacci with fixed ratios for DeMark price projections
	line_type_study_retracement,
	line_type_study_price_projection
} trendline_types;

// maximum number of trendlines per chart:
#define MAX_TRENDLINES 300

#endif
