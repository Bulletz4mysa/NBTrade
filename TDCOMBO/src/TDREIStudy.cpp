#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif
#include <math.h>


HRESULT CTDRei::ConvertBuf(void * pBuf)
{
    CBLPStudy::ConvertBuf(pBuf);
    
    StudyMessagePtr     pData  = (StudyMessagePtr)pBuf;

    //The following part is both in old and new message structure
    //Convert Buffer
    pData->rei_period          = ntohs(pData->rei_period);
    pData->duration            = ntohs(pData->duration);
    pData->period              = ntohs(pData->period);
    pData->high_lookback       = ntohs(pData->high_lookback);
    pData->low_lookback        = ntohs(pData->low_lookback);
    ConvertOscillatorParameters( &pData->zones );
    ConvertLineParameters(pData->line_params, 1);

    //Assign member variables
    m_nDuration                = pData->duration;
    m_nPeriod                  = pData->rei_period;
    m_zones.overbought         = pData->zones.overbought;
    m_zones.oversold           = pData->zones.oversold;
    m_nHighLookback            = pData->high_lookback;
    m_nLowLookback             = pData->low_lookback;
    m_nReiColor                = pData->line_params[0].line_color;
    m_nReiWidth                = pData->line_params[0].line_width;

    //Get the version information
    pData->version             = ntohs(pData->version);

    //If the message is from old Big, version = 0, stop convert buffer and set default value to member variables
    //Because the old message does not have the rest fields in messages
    if (pData->version <= 0)
    {
        m_nOverBoughtColor         = blgRed;
        m_nOverSoldColor           = blgGreen;
        m_bDuration                = TRUE;
        m_bTDREIAlternate          = FALSE;
        m_nThresholdDown           = -20;
        m_nThresholdUp             = 20;
        m_nAltDaysBack             = 1;
        m_nLowRiskDownColor        = blgRed;
        m_nLowRiskUpColor          = blgGreen;
        m_nPossExcColor            = blgMagenta;
#ifdef DEBUG
        m_bLowRiskDown             = TRUE;
        m_bLowRiskUp               = TRUE;
        m_bPossExcUp               = TRUE;
        m_bPossExcDown             = TRUE;
#else
        m_bLowRiskDown             = FALSE;
        m_bLowRiskUp               = FALSE;
        m_bPossExcUp               = FALSE;
        m_bPossExcDown             = FALSE;
#endif
        return NOERROR;
    }

    //If the message is from the new big, continute convert buffer
    //Convert Buffer
    pData->overbought_color    = ntohl(pData->overbought_color);
    pData->oversold_color      = ntohl(pData->oversold_color);
    pData->low_risk_up_color   = ntohl(pData->low_risk_up_color);
    pData->low_risk_down_color = ntohl(pData->low_risk_down_color);
    pData->poss_exc_color      = ntohl(pData->poss_exc_color);
    pData->threshold_up        = ntohs(pData->threshold_up);
    pData->threshold_down      = ntohs(pData->threshold_down);
    pData->alt_days_back       = ntohs(pData->alt_days_back);

    //Assign member vaiables
    m_nOverBoughtColor         = pData->overbought_color;
    m_nOverSoldColor           = pData->oversold_color;
    m_bDuration                = TDReiCharToBOOL(pData->duration_on);
    m_bTDREIAlternate          = TDReiCharToBOOL(pData->alternate_on);
    m_nAltDaysBack             = pData->alt_days_back;
    m_nThresholdDown           = pData->threshold_down;
    m_nThresholdUp             = pData->threshold_up;
    m_nLowRiskDownColor        = pData->low_risk_down_color;
    m_nLowRiskUpColor          = pData->low_risk_up_color;
    m_nPossExcColor            = pData->poss_exc_color;
    m_bLowRiskDown             = TDReiCharToBOOL(pData->low_risk_down_on);
    m_bLowRiskUp               = TDReiCharToBOOL(pData->low_risk_up_on);
    m_bPossExcUp               = TDReiCharToBOOL(pData->poss_exc_up_on);
    m_bPossExcDown             = TDReiCharToBOOL(pData->poss_exc_down_on);

	if (pData->version > 1)
	{
		pData->data_source = ntohs(pData->data_source);
		m_nPriceSource = (tagEnumSource)pData->data_source;
	}
    return NOERROR;
}


HRESULT CTDRei::CreateStudy(IBLPGraphStudyList * pGraph)
{
    //Set up Arrows parameters
    ChartLineParameters TDREIArrow_LineParms[4] = 
        {{1, XRT_LPAT_SOLID, m_nLowRiskUpColor},        // Low Risk Up 
         {1, XRT_LPAT_SOLID, m_nLowRiskDownColor},      // Low Risk Down
         {1, XRT_LPAT_SOLID, m_nPossExcColor},          // Poss Exc, OverBought
         {1, XRT_LPAT_SOLID, m_nPossExcColor}};         // Poss Exc, OverSold

    ChartPointParameters TDREIArrow_PointParms[4] = 
        {{XRT_POINT_UPARROW,0,0},
         {XRT_POINT_DOWNARROW,0,0},
         {XRT_POINT_UPARROW,0,0},
         {XRT_POINT_DOWNARROW,0,0}};

    //Set up Oscillator parameters
    ChartLineParameters TDREI_LineParms[3] = 
        {{m_nReiWidth, XRT_LPAT_SOLID, m_nReiColor},    // REI line
         {1, XRT_TOP_FILL, blgMagenta},                 // Upper duration
         {1, XRT_LPAT_SOLID, blgMagenta}};              // Lower duration

    ChartPointParameters TDREI_PointParms[3] = 
        {{XRT_POINT_NONE,0,0},
         {XRT_POINT_NONE,0,0},
         {XRT_POINT_NONE,0,0}};

    //Add study, 4 Arrow Lines, 3 Oscillator lines, total 7
    HRESULT rc = AddStudy(pGraph, 7, 4, TDREIArrow_LineParms, TDREIArrow_PointParms);
    //If there is no data for this security, it will fail at this point
	ASSERT(SUCCEEDED(rc));
    if (FAILED(rc))
        return rc;

    //Set up the OverboughtLine, OverSoldline color
    m_OverboughtLine.put_LineColor(m_nOverBoughtColor);
    m_OversoldLine.put_LineColor(m_nOverSoldColor);

    //Oscillator line
    CChartLine *pChartLine = new CChartLine;
    pChartLine->SetLineParams(TDREI_LineParms[0]);
    pChartLine->SetPointParams(TDREI_PointParms[0]);
    m_vChartLine.push_back(pChartLine);

    //Upper duration
    m_pDurationTopLine = new CChartLine;
    m_pDurationTopLine->SetLineParams(TDREI_LineParms[1]);
    m_pDurationTopLine->SetPointParams(TDREI_PointParms[1]);

    //Bottom duration
    m_pDurationBottomLine = new CChartLine;
    m_pDurationBottomLine->SetLineParams(TDREI_LineParms[2]);
    m_pDurationBottomLine->SetPointParams(TDREI_PointParms[2]);

    //Create the Oscillator.
    CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
    if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &m_CReiChart) == S_OK)
        AddChart(m_CReiChart);
	else 
		return S_FALSE;

	if(m_bLowRiskUp || m_bLowRiskDown || m_bPossExcUp || m_bPossExcDown)
	{
		if (pSD->QueryInterface(IID_IChart, (LPVOID *)&m_CArrowChart) == S_OK)
			AddChart(m_CArrowChart);
		else
			return S_FALSE;
	}

    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	iGraphToolbar->AddStudyButton(0, "TD REI", this);

    return S_OK;//CBLPStudy::CreateStudy(pGraph);
}


HRESULT CTDRei::DoStudy(IBLPGraphData *pGraph)
{
	if (m_vCharts.empty())
		return E_HANDLE;

    float       *pfOpen, *pfHigh, *pfLow, *pfClose, *pfTrueHigh, *pfTrueLow;

    // Input
    pGraph->get_NumPoints(&m_nNumPoints);
    pGraph->get_HighData(m_nNumPoints, &pfHigh);
    pGraph->get_LowData(m_nNumPoints, &pfLow);
    pGraph->get_CloseData(m_nNumPoints, &pfClose);
    pGraph->get_OpenData(m_nNumPoints, &pfOpen);
    pGraph->get_TrueHighData(m_nNumPoints, &pfTrueHigh);
    pGraph->get_TrueLowData(m_nNumPoints, &pfTrueLow);

    //Make copy of the Input
    BLPVector<float> vOpen(m_nNumPoints, 0.0f);
    BLPVector<float> vClose(m_nNumPoints, 0.0f);
    BLPVector<float> vHigh(m_nNumPoints, 0.0f);
    BLPVector<float> vLow(m_nNumPoints, 0.0f);

    //Set the study line
    BLPVector<float> vRei(m_nNumPoints, 0.0f);
    BLPVector<float> vReiDurationInside(m_nNumPoints, HOLEF);
    BLPVector<float> vReiDurationOutside(m_nNumPoints, HOLEF);
    BLPVector<float> vLowRiskUp(m_nNumPoints, HOLEF);
    BLPVector<float> vLowRiskDown(m_nNumPoints, HOLEF);
    BLPVector<float> vPossExcOverBought(m_nNumPoints, HOLEF);
    BLPVector<float> vPossExcOverSold(m_nNumPoints, HOLEF);
    BLPVector<float> vEmpty(m_nNumPoints, HOLEF);
    BLPVector<long> vDuration(m_nNumPoints, 0);
        
    float       *REI                 = &vRei[0];
    float       *ReiDurationInside   = &vReiDurationInside[0];
    float       *ReiDurationOutside  = &vReiDurationOutside[0];
    float       *pfLowRiskUp         = &vLowRiskUp[0];
    float       *pfLowRiskDown       = &vLowRiskDown[0];
    float       *pfPossExcOverBought = &vPossExcOverBought[0];
    float       *pfPossExcOverSold   = &vPossExcOverSold[0];
    
    float       *OPEN                = &vOpen[0];
    float       *CLOSE               = &vClose[0];
    float       *HIGH                = &vHigh[0];
    float       *LOW                 = &vLow[0];
        
    CopyData(pfOpen, OPEN, 0, m_nNumPoints - 1);
    CopyData(pfClose, CLOSE, 0, m_nNumPoints - 1);
    CopyData(pfHigh, HIGH, 0, m_nNumPoints - 1);
    CopyData(pfLow, LOW, 0, m_nNumPoints - 1);

    //If of the data is invalid, change all the data on that bar invalid
    ValidateData(OPEN, CLOSE, HIGH, LOW, 0, m_nNumPoints - 1);
    
    //Calculate the TDRei
    TDReiCalc(
        OPEN,
        HIGH,
        LOW,
        CLOSE,
        OPT_HIGH,       // long  TDReiPrice1,           // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev highs
        OPT_LOW,        // long  TDReiPrice2,           // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev lows
        m_nHighLookback,// long  TDReiBarsBack1,        // bars back for highs offset comparison
        m_nLowLookback, // long  TDReiBarsBack2,        // bars back for lows offset comparison
        TRUE,           // BOOL  TDReiUseEqual,         // TRUE for use equal when comparing
        m_nPeriod,
        REI,
        0,                      
        m_nNumPoints -1);

    //Calculate TDRei Alternate if necessary
    if (m_bTDREIAlternate == TRUE)
    {
        TDReiAltCalc(
            m_nAltDaysBack,
            REI,
            0,
            m_nNumPoints - 1);
    }

    //Calculate Duration if necessary
    if (m_bDuration == TRUE)
    {
        DurationCalc(
            REI,
            ReiDurationInside,
            ReiDurationOutside,
            &vDuration[0],
            m_nDuration,
            m_nPeriod,
            m_zones.overbought,
            m_zones.oversold,
            m_nThresholdUp,
            m_nThresholdDown,
            0,
            m_nNumPoints - 1);
    }

    //Copy the REI study line onto the Oscillator Chart
    //Copy the Duration Line onto the Osciallator Chart
    //Query the Oscillator Chart
    CComPtr<IStudyData> iSD;
    HRESULT hr = (m_CReiChart)->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return hr;

    iSD->put_StudyDataLine(0, m_nNumPoints, REI);
    iSD->put_StudyDataLine(1, m_nNumPoints, ReiDurationInside);
    iSD->put_StudyDataLine(2, m_nNumPoints, ReiDurationOutside);

    //Calculate Arrows if necessary
    if (m_bLowRiskUp || m_bLowRiskDown || m_bPossExcUp || m_bPossExcDown)
    {
        ArrowsCalc(
            OPEN, 
            HIGH,
            LOW,
            CLOSE,
            pfTrueHigh,
            pfTrueLow,
            REI,
            m_zones.overbought,
            m_zones.oversold,
            m_nThresholdUp,
            m_nThresholdDown,
            pfLowRiskUp,
            pfLowRiskDown,
            pfPossExcOverBought,
            pfPossExcOverSold,
            0,
            m_nNumPoints - 1);

        //Filler out those Arrows in the Mildly Overbought or Mildly Oversold area
        fillter(pfLowRiskUp,
                pfLowRiskDown,
                &vDuration[0],
                0,
                m_nNumPoints - 1);
        
        //always monitor the last bar
        TDReiMonitorLastBar(pfPossExcOverBought,
                            pfPossExcOverSold,
                            LOW,
                            HIGH,
                            CLOSE,
                            m_nNumPoints - 1);
        
        //Hide Low Risk Up
        if (m_bLowRiskUp == FALSE)
        {
            pfLowRiskUp = &vEmpty[0];
        }

        //Hide Low Risk Down
        if (m_bLowRiskDown == FALSE)
        {
            pfLowRiskDown = &vEmpty[0];
        }

        //Hide Poss Exc Up Arrow
        if (m_bPossExcUp == FALSE)
        {
            pfPossExcOverBought = &vEmpty[0];
        }

        //Hide Poss Exc Down Arrow
        if (m_bPossExcDown == FALSE)
        {
            pfPossExcOverSold = &vEmpty[0];
        }

	    CComPtr<IStudyData> iSD;
		HRESULT hr = (m_CArrowChart)->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
		ASSERT(SUCCEEDED(hr));
		if (FAILED(hr))
			return hr;

		// Copy the Arrow lines onto the graph
		iSD->put_StudyDataLine(0, m_nNumPoints, pfLowRiskUp);
		iSD->put_StudyDataLine(1, m_nNumPoints, pfLowRiskDown);
		iSD->put_StudyDataLine(2, m_nNumPoints, pfPossExcOverBought);
		iSD->put_StudyDataLine(3, m_nNumPoints, pfPossExcOverSold);
    }
    

    return NOERROR;//CBLPStudy::DoStudy(pGraph);
}


void CTDRei::TDReiCalc(
    float *OPEN,
    float *HIGH,
    float *LOW,
    float *CLOSE,
    long  TDReiPrice1,          // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev highs
    long  TDReiPrice2,          // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev lows
    long  TDReiBarsBack1,       // bars back for highs offset comparison
    long  TDReiBarsBack2,       // bars back for lows offset comparison
    BOOL  TDReiUseEqual,        // TRUE for use equal when comparing
    short nPeriod,
    float *Rei,
    long  first_bar,            // calc only this many bars from end (speedup)          
    long  last_bar)     
{
    if (Rei == NULL)
        return;

    int nNumPoints = last_bar + 1;

    // work arrays for REI
    BLPVector<float>    vReiHi(nNumPoints, 0.0f);
    BLPVector<float>    vReiLow(nNumPoints, 0.0f);
    BLPVector<float>    vHiTbl(nNumPoints, 0.0f);
    BLPVector<float>    vLowTbl(nNumPoints, 0.0f);
    BLPVector<float>    vDiffTbl(nNumPoints, HOLEF);

    float       *ReiHi   = &vReiHi[0];
    float       *ReiLow  = &vReiLow[0];
    float       *HiTbl   = &vHiTbl[0];
    float       *LowTbl  = &vLowTbl[0];
    float       *DiffTbl = &vDiffTbl[0];
    
    // load the work arrays
    long i;
    for (i = first_bar; i < nNumPoints; ++i)
    {
        float   op = OPEN[(i)];
        float   hi = HIGH[(i)];
        float   lo = LOW[(i)];  
        float   cl = CLOSE[(i)];

        switch (TDReiPrice1)
        {
        case OPT_OPEN:  ReiHi[i] = op;  break;
        case OPT_HIGH:  ReiHi[i] = hi;  break;
        case OPT_LOW:   ReiHi[i] = lo;  break;
        case OPT_CLOSE: ReiHi[i] = cl;  break;
        case OPT_MIDPT: ReiHi[i] = (hi + lo) / 2.0f;      break;
        case OPT_PIVOT: ReiHi[i] = (hi + lo + cl) / 3.0f; break;
        default: break;
        }
        
        switch (TDReiPrice2)
        {
        case OPT_OPEN:  ReiLow[i] = op; break;
        case OPT_HIGH:  ReiLow[i] = hi; break;
        case OPT_LOW:   ReiLow[i] = lo; break;
        case OPT_CLOSE: ReiLow[i] = cl; break;
        case OPT_MIDPT: ReiLow[i] = (hi + lo) / 2.0f;           break;
        case OPT_PIVOT: ReiLow[i] = (hi + lo + cl) / 3.0f;      break;
        default: break;
        }
    }   

    // calc REI
    long x;

    //check for Qualified bar, calculate DiffTbl
    for (x = 0; x < (last_bar + 1); ++x)
    {
        //If current Day's data is invalid, set REI to HOLEF;
        if (IsInvalid(OPEN[x]))
        {
            Rei[x] = HOLEF;
            continue;
        }
        
        //How many days minumum should look back
        long nMaxDays = Max(Max(TDReiBarsBack1, TDReiBarsBack2), 6);

        if (GetnBarsBack(OPEN, first_bar, x, nMaxDays) < first_bar)
        {
            Rei[x] = HOLEF;
            continue;
        }

        long    PrvHiX  = GetnBarsBack(OPEN, first_bar, x, TDReiBarsBack1);
        long    PrvLowX = GetnBarsBack(OPEN, first_bar, x, TDReiBarsBack2);
        long    Day5    = GetnBarsBack(OPEN, first_bar, x, 5);
        long    Day6    = GetnBarsBack(OPEN, first_bar, x, 6);
        
        //check for qualify
        BOOL Qualify = FALSE;
        //Check for valid
 
        //The current day's high must be => the low 5 or 6 days earlier to qualify
        //The current day's low must be =< the high 5 or 6 days earlier to qualify
        if (TDReiUseEqual) {
            if ((ReiHi[x] >= ReiLow[Day5] || ReiHi[x] >= ReiLow[Day6]) && 
                (ReiLow[x] <= ReiHi[Day5] || ReiLow[x] <= ReiHi[Day6]))
                Qualify = TRUE;
        }
        else {
            if ((ReiHi[x] > ReiLow[Day5] || ReiHi[x] > ReiLow[Day6]) && 
                (ReiLow[x] < ReiHi[Day5] || ReiLow[x] < ReiHi[Day6]))
                Qualify = TRUE;
        }
        //Set DiffTbl to be valid as there are valid data

        //If we do can look back nMaxDays days
                nMaxDays = Max(nMaxDays, 8);
                if (GetnBarsBack(OPEN, first_bar, x, nMaxDays) >= first_bar)
        {
            
            long        Day7  = GetnBarsBack(OPEN, first_bar, x, 7);
            long        Day8  = GetnBarsBack(OPEN, first_bar, x, 8);
            float       Day7C = CLOSE[(Day7)];
            float       Day8C = CLOSE[(Day8)];

            // The high of 2 days ago must be = > the close 7 or 8 days earlier to qualify
            // The low of 2 days ago must be  = < the close 7 or 8 days earlier to qualify            
            if (TDReiUseEqual) {
                if ((ReiHi[PrvHiX] >= Day7C || ReiHi[PrvHiX] >= Day8C) && 
                    (ReiLow[PrvLowX] <= Day7C || ReiLow[PrvLowX] <= Day8C))
                    Qualify = TRUE;
            }
            else {
                if ((ReiHi[PrvHiX] > Day7C || ReiHi[PrvHiX] > Day8C) && 
                    (ReiLow[PrvLowX] < Day7C || ReiLow[PrvLowX] < Day8C))
                    Qualify = TRUE;
            }
        }

        HiTbl[x]  = (CBLPPrice)ReiHi[x] - ReiHi[PrvHiX];
        LowTbl[x] = (CBLPPrice)ReiLow[x] - ReiLow[PrvLowX];
            
        //If not qualify, set DiffTbl to 0
        if (Qualify)
            DiffTbl[x] = (CBLPPrice)HiTbl[x] + LowTbl[x];
        else
            DiffTbl[x] = 0;
    }

    //Calcualte the Rei value
    for (x = nPeriod - 1; x < nNumPoints; x++)
    {
        float Sums; 
        float AbsSums; 

        Sums = AbsSums = 0;

        //If invalid data, don't draw anything
        if (IsInvalid(Rei[x]))
            continue;

        if (GetnBarsBack(DiffTbl, first_bar, x, nPeriod) < first_bar)
        {
            Rei[x] = HOLEF;
            continue;
        }

        long nn;
        for (nn = 0; nn < nPeriod; ++nn) {
            long nnDays = GetnBarsBack(DiffTbl, first_bar, x, nn);
            
            Sums    += (CBLPPrice)DiffTbl[nnDays];
            AbsSums += (float)(fabs(HiTbl[nnDays]) + fabs(LowTbl[nnDays]));
        }
        
        if (AbsSums != 0) {
            Rei[x] = (Sums * 100.0f) / AbsSums; 
        }
        else
        {       
            Rei[x] = 0.0f;
        }
    }   
}

//Calculate the Duration based on the Rei value, overbought, oversold and Duration
//pnDuration should be initialized properly. It will contain the information to filter out the arrows.
void CTDRei::DurationCalc(
    float *Rei,
    float *ReiDurationInside,
    float *ReiDurationOutside,
    long *pnDuration,
    short nDuration,
    short nPeriod,
    short nOverBought,
    short nOverSold,
    short nUpThreshold,
    short nDownThreshold,
    long first_bar,
    long last_bar)
{
    long UpperDurationCount = 0;
    long LowerDurationCount = 0;
    long x;

    ASSERT(nPeriod >= 1);
    for (x = nPeriod - 1; x <= last_bar; x++)
    {
        //We stop counting once we encounter invalid Rei value
        if (IsInvalid(Rei[x]))
        {
            UpperDurationCount = 0;
            LowerDurationCount = 0;
            continue;
        }
        
        if (Rei[x] >= nOverBought)
        {
            ++UpperDurationCount;
            if (UpperDurationCount >= nDuration)
            {
                ReiDurationOutside[x] = 100.0f;
                ReiDurationInside[x]  = nOverBought;
            }
        }
        else {
            UpperDurationCount = 0;
        }

        if (Rei[x] <= nOverSold)
        {
            ++LowerDurationCount;
            if (LowerDurationCount >= nDuration)
            {
                ReiDurationOutside[x] = nOverSold;
                ReiDurationInside[x]  = -100.0f;
            }
        }
        else {
            LowerDurationCount = 0;
        }

        ASSERT(x >= 1);
        if (pnDuration[x - 1] != 0)
        {
            if ((pnDuration[x - 1] == MildlyOverBought && Rei[x] > nDownThreshold) ||
                (pnDuration[x - 1] == MildlyOverSold   && Rei[x] < nUpThreshold))
                pnDuration[x] = pnDuration[x - 1];
            else
                pnDuration[x] = 0;
            
        }
        else if (!IsInvalid(ReiDurationOutside[x]))
        {
            if (UpperDurationCount > 0)
                pnDuration[x] =  MildlyOverBought;
            else
                pnDuration[x] = MildlyOverSold;
        }
        else
            pnDuration [x] = 0;
            
    }
}

//Calculate TDRei Alternate according to how many days back
void CTDRei::TDReiAltCalc(
    short nAltDaysBack,   //How many days back
    float *Rei,
    long first_bar,
    long last_bar)
{
    if(Rei == NULL) return;

    long nNumPoints = last_bar + 1, i;
    //According to nAltDaysBack, reset Rei values.
    for (i = nAltDaysBack; i < nNumPoints; ++i)
    {
        if (Rei[i] == 0.0f)
        {
            long j;
            for (j = 1; j <= nAltDaysBack; ++j)
            {
                long nDaysBack = GetnBarsBack(Rei, first_bar, i, j);
                if ( nDaysBack >= first_bar)
                    Rei[nDaysBack] = 0;
            }
            
        }
    }
}

//Calculate the Arrows
void CTDRei::ArrowsCalc(
    float *OPEN,
    float *HIGH,
    float *LOW,
    float *CLOSE,
    float *TRUEHIGH,
    float *TRUELOW,
    float *Rei,
    short nOverBought,
    short nOverSold,
    short nThresholdUp,
    short nThresholdDown,
    float *LowRiskUp,
    float *LowRiskDown,
    float *PossExcOverBought,
    float *PossExcOverSold,
    long first_bar,
    long last_bar)
{

    if (Rei == NULL) return;

    long nNumPoints = last_bar + 1, i;
    float low_offset, high_offset, max, min;

    GetDataRange(nNumPoints, HIGH, &high_offset, &max);
    GetDataRange(nNumPoints, LOW, &min, &low_offset);
    low_offset = (min - max) * 0.03f;
    high_offset = (max - min) * 0.03f;

    //Temp variables
    BLPVector<BOOL>     vOverBought(nNumPoints, FALSE);
    BLPVector<BOOL>     vOverSold(nNumPoints, FALSE);
    BLPVector<BOOL>     vOverBoughtSetup(nNumPoints, FALSE);
    BLPVector<BOOL>     vOverSoldSetup(nNumPoints, FALSE);

    //Overbought yet not Oversold.
    //Oversold yet not Overbought
    //The first bar is special
    if (!IsInvalid(Rei[0]))
    {
        if (Rei[0] >= nOverBought)
        {
            vOverBought[0] = TRUE;
            vOverSold[0]   = FALSE;
        }
        else if (Rei[0] <= nOverSold)
        {
            vOverBought[0] = FALSE;
            vOverSold[0]   = TRUE;
        }
    }
    
    //If overbought, can not be oversold
    //If oversold, can not be overbought
    //If not both, the same as before
    for (i = 1; i < nNumPoints; ++i)
    {
        if (IsInvalid(Rei[i]))
        {
            vOverBought[i] = FALSE;
            vOverSold[i] = FALSE;
            continue;
        }

        if (Rei[i] >= nOverBought)
        {
            vOverBought[i] = TRUE;
            vOverSold[i]   = FALSE;
        }
        else if (Rei[i] <= nOverSold)
        {
            vOverBought[i] = FALSE;
            vOverSold[i]   = TRUE;
        }
        else
        {
            long Day1 = GetnBarsBack(Rei, first_bar, i, 1);
                        if (Day1 < first_bar)
                                continue;
            vOverBought[i] = vOverBought[Day1];
            vOverSold[i]   = vOverSold[Day1];
        }
    }

	//Day1High and Day1Low will only be used in the comparation with Day1 in low risk up/down
	float *Day1High, *Day1Low;
	switch (m_nPriceSource)
	{
	case UseBarValue:
		Day1High = HIGH;
		Day1Low  = LOW;
		break;
	case UseTrueValue:
	default:
		Day1High = TRUEHIGH;
		Day1Low  = TRUELOW;
		break;
	}
    //Overbought Setup
    //Oversold Setup
    for (i = 3; i < nNumPoints; ++i)
    {
        int     Day3 = GetnBarsBack(Rei, first_bar, i, 3);
        int     Day2 = GetnBarsBack(Rei, first_bar, i, 2);
        int     Day1 = GetnBarsBack(Rei, first_bar, i, 1);

        if (Day3 < first_bar || IsInvalid(Rei[i]))
            continue;
        
        float Day2Day3Close =   CLOSE[Day2] - CLOSE[Day3];
        float Day1Day2Close =   CLOSE[Day1] - CLOSE[Day2];

        //over bought setup 1
        //coincidently or subsequently overbought yet not oversold
        //upclose follow by downclose
        //rei reading greater than bottom of threshold
        if ((vOverBought[Day2] || vOverBought[Day3]) 
            && (Day2Day3Close > 0)
            && (Day1Day2Close < 0)
            && (Rei[Day1] > nThresholdDown))
            vOverBoughtSetup[i] = TRUE;
        //over bought setup 2
        //coincidently or subsequently overbought yet not oversold
	//up close and close below open
        //rei reading greater than bottom of threshold
        else if ((vOverBought[Day1] || vOverBought[Day2])
                 && (Day1Day2Close > 0 && (CLOSE[Day1] - OPEN[Day1]) < 0)
                 && (Rei[Day1] > nThresholdDown))
            vOverBoughtSetup[i] = TRUE;
        //over sold setup 1
        //coincidently or subsequently oversold yet not overbought
        //downclose follow by upclose
        //rei reading smaller than top of threshold
        else if ((vOverSold[Day2] || vOverSold[Day3])
                 && (Day2Day3Close < 0)
                 && (Day1Day2Close > 0)
                 && (Rei[Day1] < nThresholdUp))
            vOverSoldSetup[i] = TRUE;
        //over sold setup 2
        //coincidently or subsequently oversold yet not overbought
	//down close and close above open
        //rei reading smaller than top of threshold
        else if ((vOverSold[Day1] || vOverSold[Day2])
                 && (Day1Day2Close < 0 && (CLOSE[Day1] - OPEN[Day1]) > 0)
                 && (Rei[Day1] < nThresholdUp))
            vOverSoldSetup[i] = TRUE;
        
        if (vOverBoughtSetup[i])
        {
            //Open below two bars Low
            //Poss Exc Arrow Up
            if ((OPEN[i] < LOW[Day1]) && (OPEN[i] < LOW[Day2]))
            {
                PossExcOverBought[i] = LOW[i] + low_offset;
            }
            //open above yesterday's low, then trades below yesterday's low
            //open between the two days' low
            //Low Risk Down
            else if (((OPEN[i] > Day1Low[Day1]) && (LOW[i] < Day1Low[Day1]))
                     || ((OPEN[i] < Day1Low[Day1]) && (OPEN[i] > LOW[Day2]) && (OPEN[i] > LOW[i])))
            {
                LowRiskDown[i] = HIGH[i] + high_offset;
            }
        }

        else if (vOverSoldSetup[i])
        {
            //Open above two day's High
            //Poss Exc Arrow Down
            if((OPEN[i] > HIGH[Day1]) && (OPEN[i] > HIGH[Day2]))
            {
                PossExcOverSold[i] = HIGH[i] + high_offset;
            }
            //Open below yesterday's high, then trades above yesterday's high
            //OPen between the two days' high
            //Low Risk Up
            else if (((OPEN[i] < Day1High[Day1]) && (HIGH[i] > Day1High[Day1]))
                     || ((OPEN[i] > Day1High[Day1]) && (OPEN[i] < HIGH[Day2]) && (OPEN[i] < HIGH[i])))
            {
                LowRiskUp[i] = LOW[i] + low_offset;
            }
        }
    }
}

void CTDRei::TDReiMonitorLastBar(float *PossExcOverBought,
                                 float *PossExcOverSold,
                                 float *LOW,
                                 float *HIGH,
                                 float *CLOSE,
                                 long last_bar)
{
    if (last_bar <= 0) return;
    
    if (!IsInvalid(PossExcOverBought[last_bar]))
    {
        int Day1 = GetnBarsBack(CLOSE, 0, last_bar, 1);
        if (Day1 >= 0 && CLOSE[Day1] < LOW[last_bar])
            PossExcOverBought[last_bar] = HOLEF;
    }
    else if (!IsInvalid(PossExcOverSold[last_bar]))
    {
        int Day1 = GetnBarsBack(CLOSE, 0, last_bar, 1);
        if (Day1 >= 0 && CLOSE[Day1] > HIGH[last_bar])
            PossExcOverSold[last_bar] = HOLEF;
    }
}


//return the correct BOOL value according to the flag
BOOL CTDRei::TDReiCharToBOOL(char flag)
{
    if (flag == 'X') 
        return TRUE;
    else
        return FALSE;
}

//Check OPEN, CLOSE, HIGH, LOW are valid or not.
//If not, set everything to HOLEF
long CTDRei::ValidateData(float *OPEN,
                          float *CLOSE,
                          float *HIGH,
                          float *LOW,
                          long first_bar,
                          long last_bar)
{
    long        i;
    long        counter = 0;
    
    for (i = first_bar;i < last_bar; ++i)
    {
        BOOL    invalid = FALSE;
        if (IsInvalid(OPEN[i], CLOSE[i], HIGH[i], LOW[i]))
        {
            invalid = TRUE;
        }
 
        if (invalid)
        {
            OPEN[i]  = HOLEF;
            CLOSE[i] = HOLEF;
            HIGH[i]  = HOLEF;
            LOW[i]   = HOLEF;
            ++counter;
        }
    }

    return counter;
}

//return the Bar number that is nBarsBack from nStartBar in DATA array
//return -1 if it fails to find it before nFirstBar(including).
long CTDRei::GetnBarsBack(float *DATA,
                          long nFirstBar,
                          long nStartBar,
                          long nBarsBack)
{
    long        nCounter    = 0;
    long        nCurrentBar = nStartBar;
    
    if (nBarsBack < 0 || nStartBar == nFirstBar)
        return -1;
    
    while ((nCounter < nBarsBack) && (nCurrentBar > nFirstBar))
    {
        --nCurrentBar;
        if (!IsInvalid(DATA[nCurrentBar]))
            ++nCounter;
    }

    if (nCounter == nBarsBack)
        return nCurrentBar;
    else
        return -1;
}

//Get the larger value of x, y
long CTDRei::Max(long x, long y)
{
    if (x < y)
        return y;
    else
        return x;
}

//Copy data from pfSrc to pfDes
void CTDRei::CopyData(float *pfSrc,
                      float *pfDes,
                      long first_bar,
                      long last_bar)
{
    long i;

    for (i = first_bar; i <= last_bar; ++i)
        pfDes[i] = pfSrc[i];

}

void CTDRei::fillter(float *pfBuyArrows,
                     float *pfSellArrows,
                     long  *pnDuration,
                     long first_bar,
                     long last_bar)
{
    long nBar;

    for (nBar = first_bar; nBar <= last_bar; ++nBar)
    {
        if (pnDuration[nBar] != 0)
            pfBuyArrows[nBar] = pfSellArrows[nBar] = HOLEF;
    }
}
