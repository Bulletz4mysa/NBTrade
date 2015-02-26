
#ifndef __DEMARKSTUDIES_H__
#define __DEMARKSTUDIES_H__

//#include "graphdata.h"
#include "compileropts.h"
#include "blpstudy.h"
#include "ctddifferential.h"
#include "ctdreversedifferential.h"
#include "ctdantidifferential.h"

#define DWAVE_MAX_LINES 12
#define DWAVE_MAX_WAVES 9
#define DWAVE_NUM_WAVES 17

#define DWAVE_UP 0
#define DWAVE_DOWN 1

#define DWAVE_EOD -1

#define DOPEN		0
#define DHIGH		1
#define DLOW		2
#define DCLOSE		3
#define DTHIGH		4
#define DTLOW		5


class CGraphStudyFactory : public IBIGObjectFactory,
                           public CBLPCmdTarget
{
 public:
    virtual HRESULT CreateInstance(unsigned long MsgId, IUnknown **piStudy);
/////////////////////////////////////////////////////////////////////////////
//MFC-like IUnknown implementation

    ULONG STDMETHODCALLTYPE AddRef() {
        return CBLPCmdTarget::AddRef();
    }

    ULONG STDMETHODCALLTYPE Release() {
        return CBLPCmdTarget::Release();
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppvRet) {
        return CBLPCmdTarget::QueryInterface(iid, ppvRet);              \
                                                                            }

    virtual LPUNKNOWN GetInterfaceHook(const void* piid);
/////////////////////////////////////////////////////////////////////////////
};

typedef enum {
    XRT_DEMARK_FIRST = 12,
    XRT_POINT_UPARROW = XRT_DEMARK_FIRST,
    XRT_POINT_DOWNARROW = 13,
    XRT_POINT_INDEX_BELOW = 14,
    XRT_POINT_INDEX_ABOVE = 15,
    XRT_POINT_SEQUENTIAL_UP = 16,
    XRT_POINT_SEQUENTIAL_DOWN = 17,
    XRT_BACKGROUND_RECTANGLE = 18,
    XRT_PT_TO_PT_SOLID = 19,
    XRT_PT_TO_PT_SOLID_WITH_VALUE = 19,
    XRT_PT_TO_PT_DASHED = 20,
    XRT_BACKGROUND_NONE = 21,
    XRT_BACKGROUND_SHADE = 22,
    XRT_POINT_INDEX_BELOW_DROPPED = 23,
    XRT_POINT_INDEX_ABOVE_RAISED = 24,
    XRT_DEMARK_LAST = XRT_POINT_INDEX_ABOVE_RAISED
} DeMarkSpecialStyles;
    

class CTDStudy : public CBLPOscillatorStudy
{
 public:
    static BOOL IsDeMarkStudy(unsigned long nMessageId)
        {
            if ( (nMessageId >= blgTDFirst) && (nMessageId <= blgTDLast) )
                return TRUE;
            else
                return FALSE;
        }


///////////////////////////////////////////////////////////////////////
//
//      Marshalling Interface to the BIG
//
// union of all Study Message struct's
    typedef struct tagStructStudyMessageBuf {
        union {   
            struct {
                short msg_id;   
                short instance_id;
                short base_id;
            } StudyHeader;

            struct {
                short msg_id;   
                short instance_id;
                short base_id;

                // CTDChannel properties
                short nPeriod;
                short nMAType;
                BOOL bCenter;
                short nSmooth;
                short nPriceHigh, nPriceLow;
                float fHighFactor, fLowFactor;

                // Line parameters
                ChartLineParameters line_params[2]; /* line1 = high, line2 = low */
            } TDChannel;

            struct {
                short msg_id;   
                short instance_id;
                short base_id;

                char type;  //type of the moving average 0=simple 
                char center; // 0=no, 1=yes will move average to right period/2 

                char higher_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
                char lower_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
                short period;       /* Length of the moving average */
                short smoothing; /* number of bars to smooth the average */  
                short high_factor;  
                short low_factor;  
                short lookback;  /* number of bars to lookback for new highs/lows */
                short spare;  
                ChartLineParameters line_params[2]; /* line1 = high, line2 = low */
                ChartPointParameters point_params[2];
            } TDMavg1;

            struct {
                short msg_id;   
                short instance_id;
                short base_id;

                char use_true_prices;
                char use_every_bar;
                ChartLineParameters line_params[2];
            } TDRangeProjections;


            struct {
                short msg_id;   //      blgTDSequential = 1036, blgTDCombo = 1037 
                short instance_id;
                short base_id;

                float fProjectionPercent;
                short nProjections; // Number of projections per trendline
                short nNumTicks;  // # of Ticks required for breakout
                short nLookback;  // # bars to look back
                short nMaxUsePoint; // # times a ppoint can be reused
                short nLevel;
                BOOL  bQualifier1;
                BOOL  bQualifier2;
                BOOL  bQualifier3;
                BOOL  bUseTrueHighLow;
                BOOL  bDisplayDemand;
                BOOL  bDisplaySupply;
                BOOL  bDisplayQualified;
                BOOL  bDisplayDisqualified;
                BOOL  bDisplayTDPoints;
                BOOL  bCancel1;
                BOOL  bCancel2;
                BOOL  bCancel3;
                BOOL  bDisplayCancel;

                // Line parameters
                ChartLineParameters line_params[4]; // Line 0 = Qualified Demand; Line 1 = Qualified Supply;
                // Line 2 = Disqualified; Line 4 = Cancel
            }   TDLines;

            struct {
                short msg_id;   //      blgTDLinesEx  = 1046
                short instance_id;
                short base_id;

                float fProjectionPercent;
                short nProjections; // Number of projections per trendline
                short nNumTicks;  // # of Ticks required for breakout
                short nLookback;  // # bars to look back
                short nMaxUsePoint; // # times a point can be reused
                short nPointPrice; // enum tagEnumPrice {High = 1, Close = 3}
                short nLevel;
                BOOL  bQualifier1;
                BOOL  bQualifier2;
                BOOL  bQualifier3;
                BOOL  bUseTrueHighLow;
                BOOL  bDisplayDemand;
                BOOL  bDisplaySupply;
                BOOL  bDisplayQualified;
                BOOL  bDisplayDisqualified;
                BOOL  bDisplayTDPoints;
                BOOL  bCancel1;
                BOOL  bCancel2;
                BOOL  bCancel3;
                BOOL  bDisplayCancel;

                // Line parameters
                ChartLineParameters line_params[4]; // Line 0 = Qualified Demand; Line 1 = Qualified Supply;
                // Line 2 = Disqualified; Line 4 = Cancel
            }   TDLinesEx;

            struct {
                short msg_id;  // blgDateCallback
                short instance_id;
                short base_id;

                long date;
            } DateCallback;


        }; // end of study specific union

        long reserved[4]; //reserved tail for all study messages
    } *StudyMessagePtr;
// end of Study Message structs

// Instantiation
 public:
    static CBLPStudy2 *CreateStudyInstance(unsigned long nMsgId);
};

class CTDStudyWithCountdown : public CTDStudy,
                              public ITDCountdown
{
 public:
    static BLPVector<int **> m_ppnCountdownDisplay;
    CTDStudyWithCountdown()
    {
        m_pnCountdownDisplay = NULL;

        for (m_nCountdownDisplayIndex = 0; m_nCountdownDisplayIndex < (short) m_ppnCountdownDisplay.size(); m_nCountdownDisplayIndex++) {
            if (m_ppnCountdownDisplay[m_nCountdownDisplayIndex] == NULL) {
                m_ppnCountdownDisplay[m_nCountdownDisplayIndex] = &m_pnCountdownDisplay;
                break;
            }
        }

        if (m_nCountdownDisplayIndex == (short) m_ppnCountdownDisplay.size()) {
            m_ppnCountdownDisplay.push_back(&m_pnCountdownDisplay);
        }

    }

	~CTDStudyWithCountdown() {
		delete [] m_pnCountdownDisplay;
		m_ppnCountdownDisplay[m_nCountdownDisplayIndex] = NULL;
	}

 protected:
// Internal array of integers used for displaying characters
    int *m_pnCountdownDisplay;
    int m_nCountdownDisplayIndex;

 public:
    IMPLEMENT_IUnknown(CTDStudyWithCountdown)
        BEGIN_BLPINTERFACE_MAP(CTDStudyWithCountdown, CTDStudy)
        SUPPORT_GEN_INTERFACE(ITDCountdown)
        END_BLPINTERFACE_MAP()

//ITDCountdown
        virtual HRESULT get_CountdownDisplay(short nIndex, int **ppCountdownArray)
        {
            ASSERT ((nIndex >= 0) && (nIndex < (short) m_ppnCountdownDisplay.size()));
            if ((nIndex >= 0) && (nIndex < (short) m_ppnCountdownDisplay.size())) 
            {
                *ppCountdownArray = *(m_ppnCountdownDisplay[nIndex]);
                return S_OK;
            }
            else {
                *ppCountdownArray = NULL;
                return E_INVALIDARG;
            }
        }


};

class CTDSequential : public CTDStudyWithCountdown,
                      public CTDBreakoutQualifier,
                      public IStudyTrendlines

{
#if (defined __SUNPRO_CC)
public:
#else
 protected:
#endif
// enum's to support TD Sequential and TD Combo parameters
    typedef enum {noIntersection = 0, anytime = 1, during = 2} enumIntersectionRule;
    typedef enum {noCancel = 0, LowVsHigh = 1, HighVsHigh = 2, CloseVsHigh = 3,
                  LowVsTrueHigh = 4, HighVsTrueHigh = 5, CloseVsTrueHigh = 6, TrueLowVsTrueHigh = 7, HighVsClose = 8
                  } enumCancelRule;
    typedef enum {RecycleAlways = 0, RecycleOnOrBefore = 1, RecycleBefore = 2, RecyclenoRecycling = 3} enumRecycleRule;
    typedef enum {GT = 0, GTE = 1, LT = 2, LTE = 3} enumQualifierCondition;
    typedef enum {noIgnore = 0, IgnoreSmaller = 1, IgnoreLarger = 2, IgnoreBoth =3} enumIgnoreSetupRule;

// Qualifier structure used by TD Sequential and TD Combo
    typedef struct tagQualifier {
        BOOL bUse;
        enumPriceType nPrice1;
        short nBar1; 
        BOOL bSetupPhase1;
        enumQualifierCondition nCondition; 
        enumPriceType nPrice2;
        short nBar2;
        BOOL bSetupPhase2;
    } structQualifier; 
    
    typedef struct  tagStructStudyMessageBuf{
        short msg_id;   /* blgTDSequential = 1036, blgTDCombo = 1037 */
        short instance_id;
        short base_id;
  
        /* Setup parameters */
        short nSetupLookback;
        short nMinSetupBars;
        long  bSetupUsesEqual; /* =0. dummy */
        BOOL  bShowFullSetup;
        CBLPStudy::enumPriceType  nSetupPrice;
  
        /* Cancel and Recycle Rules */
        long nCancelRule;
        long  nRecycleRule;
        long  bReverseSetupCancelsCountdown; /* =1. dummy */
        long  bIgnoreSetupInPriorRange; /* =1 dummy */

        /* Intersection Rule */
        short nStartIntersection;/* =9 for seq. */
        enumIntersectionRule  nIntersectionRule;/* =0, no intersection. dummy */
  
        /* Countdown parameters */
        short nCountdownLookback;
        short nMinCountdown;
        long  bCountdownUsesEqual;/* =1. dummy */
        CBLPStudy::enumPriceType  nCountdownPrice;
  
        /* Countdown qualifiers */
        long nIgnoreSetupRule;
        float fLargerSetupFactor; /* include recycle count */
        // This used to be struct Qualifier. Since we no longer uses Setup Qualifier, 
        // we are taking this variable out, and use it for Combo and Seq.
        long  nVersion; /* */
        long  nComboVersion; /* Combo Version */
        short nComboCountdownUsesEqual; /* 4 Options*/
        float fSmallRecycleLevel; /* Recycle Level, Bottom Line */
        float fLargeRecycleLevel; /* Recycle Level, Top Line */
        short nSetupWithinSetupOptions[2]; /* Option 1: Use Max/Min Setup. Option 2: Use Close/True Range */
        short nStopLossLevelOption; /* Option: Numbered Bars/All Bars */
        short nRecycleCount; /* Recycle Count */
        short dummy; /* in order to make the size to the same with stuctQualifier */

        structQualifier CountdownQualifier1; /* */
        structQualifier CountdownQualifier2; /* */
        long bAndCountdownQualifiers;

        /*
         * Line Parameters
         * Line 0 = Sell Setup Shading, Line 1 = Buy Setup Shading
         * Line 2 = Setup numbering, Line 4 = Countdown numbering
         * Line 0 or 1 's line_stylle < 0 means TDST off.
         * Both of them to be positive to mean TDST on.
         */
        ChartLineParameters line_params[4];

        /*
         * new variables start from here
         */
        char  cShowFullCountdown;
        char  cShowSetup;
        char  cShowCountdown;
        char  cShowSetupQualifier;
        char  cShowCountdownCompletion;
        char  cShowStopLossLevel;
        char  cShowShade;
        char  cShowTDST;
        char  cExtendTDST;
        char  cRecycleConfirm;
        char  cRecycleCount;
        char  cLastCountdownQualifier;
		
		//Added for TREQ 36665 by Arun - Combo variables    
		char  cCntCondCombo;

		//Added for TREQ 36665 by Arun - Sequential variables*/    
		char  cEnableSWithinS;
		char  cReverseSetupCancel;
		char  cShowSetupTo;

    } *StudyMessagePtr;
    
 protected:
    virtual HRESULT ConvertQualifier(structQualifier *pQualifier);

 public:
    IMPLEMENT_IUnknown(CTDSequential);
    BEGIN_BLPINTERFACE_MAP(CTDSequential, CTDStudyWithCountdown)
        SUPPORT_GEN_INTERFACE(IStudyTrendlines)
    END_BLPINTERFACE_MAP()

        virtual HRESULT ConvertBuf(void * pBuf) ;
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

// IStudyTrendlines
    virtual HRESULT get_TrendlineCount(short *nCount)
        {
            *nCount = MAX_TRENDLINES;
            return S_OK;
        }

    virtual HRESULT get_TrendlineId(short Index, long *pnId)
        {
            ASSERT ((Index >= 0) && (Index < MAX_TRENDLINES));
            if ((Index >= 0) && (Index < MAX_TRENDLINES)) {
                *pnId = m_pTrendLineIds[Index];
                return S_OK;
            }
            else {
                *pnId = 0;
                return E_INVALIDARG;
            }
        }

    CTDSequential() {
        m_nSetupLookback = 4;
        m_nCountdownLookback = 2;
        m_nMinSetupBars = 9;
        m_bShowFullSetup = FALSE;
        m_nMinCountdownBars = 13;
        m_nIntersectionRule = noIntersection;//  Sequential default; none for Combo
        m_nStartIntersection = 8; // Sequential default; 0 for Combo
        m_nCancelRule = LowVsTrueHigh;
        m_nRecycleRule = RecycleAlways;
        m_nRecycleCount = 13;
        m_nSetupPrice = Close;
        m_nCountdownPrice = Close;
        m_nTerminationCountPrice = Open;

        m_bQualifier1 = TRUE;
        m_bQualifier2 = FALSE;
        m_bQualifier3 = FALSE;

        // initialize default Countdown qualifier
        m_CountdownQualifier2.bUse = FALSE;
        m_CountdownQualifier1.bUse = TRUE;
        m_CountdownQualifier1.nPrice1 = Low;
        m_CountdownQualifier1.nBar1 = 13; 
        m_CountdownQualifier1.bSetupPhase1 = FALSE;
        m_CountdownQualifier1.nCondition = LTE; 
        m_CountdownQualifier1.nPrice2 = Close;
        m_CountdownQualifier1.nBar2 = 8;
        m_CountdownQualifier1.bSetupPhase2 = FALSE;

        m_CountdownQualifier2.bUse = FALSE;
        m_CountdownQualifier2.nPrice1 = High;
        m_CountdownQualifier2.nBar1 = 13; 
        m_CountdownQualifier2.bSetupPhase1 = FALSE;
        m_CountdownQualifier2.nCondition = LT; 
        m_CountdownQualifier2.nPrice2 = Low;
        m_CountdownQualifier2.nBar2 = 1;
        m_CountdownQualifier2.bSetupPhase2 = FALSE;

        m_bAndCountdownQualifiers = TRUE;

        //Init all the new variables
        m_fSmallRecycleLevel = (float)1.000;
        m_fLargeRecycleLevel = (float)1.618;
        m_bShowSetup = TRUE;
        m_bShowSetupQualifier = FALSE;
        m_bShowCountdown = TRUE;
        m_bShowFullCountdown = FALSE;
        m_bShowCountdownCompletion = FALSE;
        m_bShowStopLossLevel = FALSE;
        m_bRecycleConfirm = FALSE;
//        m_nRecycleCount = 12;
        m_bRecycleCount = TRUE;
        m_nSetupWithinSetupOptions[0] = UseMaxSetup;
        m_nSetupWithinSetupOptions[1] = UseMinSetup;
        m_nStopLossLevelOption = NumberedCountdownBars;
        m_bShowShade = FALSE;
        m_bShowTDST = FALSE;
        m_bExtendTDST = FALSE;
        m_bLastCountdownQualifier = TRUE; //Always FALSE for COMBO
        
		//Variables added for TDSequential for TREQ 36665 by Arun.
		m_bReverseSetupCancel = TRUE;
		m_bEnableSWithinS = FALSE;
		m_nShowSetupTo = 1;
		
        
		//Combo Variables
        m_nComboCountdownUsesEqual = 4;
        int i;
        for (i = 0; i < MAX_TRENDLINES; i++)
            m_pTrendLineIds[i] = 0;
		
		// Combo Variables added for TREQ 36665 by Arun.
		m_bEnableSWSCombo = TRUE;
		m_bRevCancelCombo = TRUE;
		m_nShowSetupToCombo = 1;
		m_nSellCntCond1Combo = 1;
		m_nSellCntCond2Combo = 1;
		m_nBuyCntCond1Combo = 1;
		m_nBuyCntCond2Combo = 1;
        
		m_strButtonText = "TD Sequential";
    }

 protected:
    BOOL TestCancelCriteria(IBLPGraphData *pGraph, long nRule, BOOL bSellSetup, int nIndex, float fComparePrice);
    float GetComparePrice(enumPriceType nPrice, BOOL bSellSetup, int nIndex);

    //This function will start from nStart to nEnd(including both bars),
    //according to the Price type and number of bars to lookback, looking
    //for certain type of price flip(Sell/Buy)
    //nStart >= 0, nEnd >= nStart, nEnd <= m_nNumPoints - 1
    //nLookback > 0;
    //Return nEnd + 1 if there is no such a pattern.
    //If there are more patterns in the range, it will return the very
    //first one.
    long GetBarOfPriceFlip(enumPriceType nPrice, long nStart, long nEnd, short nLookback, BOOL bSell);
    BOOL LastCountdownQualifier(IBLPGraphData *pGraph, BOOL bSellCountdown, int nCurrentBar, BLPVector<long> m_vnCountdownBars); 

    class XDeMarkCountdown {
    public:
        XDeMarkCountdown() {
            m_bSellCountdown=0;
            m_fMinSetup=0;
            m_fMaxSetup=0;
            m_bCompletedSetup = FALSE;
            m_bCompletedCountdown = FALSE;
			m_bDisplayR = FALSE;
        }

        XDeMarkCountdown(long nNumBars, BOOL bSellCountdown, float fMinSetup, float fMaxSetup) :
            m_bSellCountdown(bSellCountdown),
            m_vpriceCountdown(nNumBars, HOLEF),
            m_fMinSetup(fMinSetup),
            m_fMaxSetup(fMaxSetup),
            m_bCompletedSetup(FALSE),
            m_bCompletedCountdown(FALSE){}

        BLPVector<float> m_vpriceCountdown;
        BLPVector<long> m_vnCountdownBars;

        BOOL m_bSellCountdown;
        float m_fMinSetup;
        float m_fMaxSetup;
        long m_nFirstSetupBar;
        long m_nLastSetupBar;
        BOOL m_bCompletedSetup;
        BOOL m_bCompletedCountdown;
        long m_nCompleteCountdownBar;
		BOOL  m_bDisplayR;
    };

    XDeMarkCountdown* DoCountdown(IBLPGraphData *pGraph, BOOL bSellCountdown, int nCountdownStart, float fMinSetup, float fMaxSetup);
    virtual BOOL TestTerminationCount(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount);
    virtual BOOL TestCountdownRule(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount, CTDStudy::enumPriceType nComparePrice = Close);
    XDeMarkCountdown* GetFirstReverseCountdown(XDeMarkCountdown* pCountdown);
	XDeMarkCountdown* GetFirstReverseCountdownBk(XDeMarkCountdown* pCountdown);
    XDeMarkCountdown* GetFirstSimilarCountdown(XDeMarkCountdown* pCountdown);
    int DrawStopLossLevel(XDeMarkCountdown* pCountdown, IBLPGraphData* pGraph);
    int DrawTDSTLine(XDeMarkCountdown* pCountdown, IBLPGraphData *pGraph);
    BOOL SetupWithinSetup(XDeMarkCountdown* pCountdown);
    void DrawRecycleLevel(XDeMarkCountdown* pCountdown, IBLPGraphData *pGraph);
        
 protected:
    enum {NumberedCountdownBars = 2, AllCountdownBars = 1}; //Risk Level Option
    enum {UseMaxSetup = 1, UseMinSetup = 2 }; //SetupWithinSetup Option 1
    enum {UseClosePrice = 1, UseTrueRange = 2}; //SetupWithinSetup Option 2
    enum {IgnoreNone = 0, IgnoreSmallAndLarge, IgnoreCurrent, IgnoreAll}; //RecycleRules
            
    //Member variables
 protected:    
    BLPVector<XDeMarkCountdown> m_vDeMarkCountdowns;

    // Setup parameters
    short m_nSetupLookback;
    short m_nMinSetupBars;
    BOOL m_bShowSetup;
    BOOL m_bShowSetupQualifier;
    BOOL m_bShowFullSetup;
    BOOL m_bShowShade;
    enumPriceType m_nSetupPrice;

    // Cancel and Recycle rules
    short m_nRecycleCount;
    long m_nCancelRule;
    long m_nRecycleRule;
    float m_fLargeRecycleLevel;
    float m_fSmallRecycleLevel;
    BOOL m_bReverseSetupCancelsCountdown;
    BOOL m_bRecycleCount;
    BOOL m_bRecycleConfirm;
    BOOL m_bCountdownUsesEqual;
    short m_nSetupWithinSetupOptions[2]; // Option 1:Max/Min Setup, Option 2:Close/TrueRange

    // Intersection rule
    short m_nStartIntersection;
    enumIntersectionRule m_nIntersectionRule;

    // Countdown parameters
    short m_nCountdownLookback;
    short m_nMinCountdownBars;
    short m_nStopLossLevelOption;
    BOOL m_bShowCountdown;
    BOOL m_bShowCountdownCompletion;
    BOOL m_bShowStopLossLevel;
    BOOL m_bLastCountdownQualifier;
    BOOL m_bShowFullCountdown;
    enumPriceType m_nCountdownPrice;
    enumPriceType m_nTerminationCountPrice;
        
    // Countdown qualifiers
    structQualifier m_SetupQualifier, m_CountdownQualifier2, m_CountdownQualifier1;
    BOOL m_bAndCountdownQualifiers;

    //TDST
    BOOL m_bShowTDST;
    BOOL m_bExtendTDST;

    //Combo Variables
    short m_nComboVersion;
    short m_nComboCountdownUsesEqual;

	//Combo Variables added for TREQ 36665 by Arun
	BOOL  m_bEnableSWSCombo;
	BOOL  m_bRevCancelCombo;
	
	short m_nRPosition;
	short m_nShowSetupToCombo;
	short m_nSellCntCond1Combo;
	short m_nSellCntCond2Combo;
	short m_nBuyCntCond1Combo;
	short m_nBuyCntCond2Combo;
	
	// Sequential Variables for TREQ 36665 by Arun
	BOOL m_bReverseSetupCancel;
	BOOL m_bEnableSWithinS;
	short m_nShowSetupTo;
    
	//Study Button
    CFastString m_strButtonText;

 private:
    float *m_pfHigh;
    float *m_pfLow;
    float *m_pfClose;
    float *m_pfOpen;
    float *m_pfTrueHigh;
    float *m_pfTrueLow;
    long m_nTrendLineIndex;
    long m_pTrendLineIds[MAX_TRENDLINES];
};

class CTDCombo : public CTDSequential
{
 public:
    CTDCombo() : CTDSequential()
        {
            m_nIntersectionRule = noIntersection;
            m_nStartIntersection = 0;
            m_strButtonText = "TD Combo";

            m_nComboVersion = 3;
			m_nComboCountdownUsesEqual = 4 ;
			
			// Variables added for TREQ 36665 by Arun. 
			m_bEnableSWSCombo = TRUE;
			m_bRevCancelCombo = TRUE;
			m_nShowSetupToCombo = 1;
			m_nSellCntCond1Combo = 1;
			m_nSellCntCond2Combo = 1;
			m_nBuyCntCond1Combo = 1;
			m_nBuyCntCond2Combo = 1;

        }

 protected:
    enum {
        ComboCountdownUsesEqualAllDay= 1,
        ComboCountdownUsesEqualDayOneToTen = 2,
        ComboCountdownUsesEqualDayGreatThanTen = 3,
        ComboCountdownUsesEqualTurnOff = 4
    };

    virtual BOOL TestTerminationCount(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount);
    virtual BOOL TestCountdownRule(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount, CTDStudy::enumPriceType nComparePrice = Close);
    CBLPPrice m_fPreviousCountdownClose;
	CBLPPrice m_fPreviousCountdownHighOrLow;

};

class CTDChannel : public CTDStudy
{
//      DECLARE_DYNCREATE(CTDChannel)
 protected:
    short m_nPeriod;
    enum tagEnumMAType {Simple, Exponential} m_nMAType;
    BOOL m_bCenter;
    short m_nSmooth;
    enumPriceType m_nPriceHigh, m_nPriceLow;
//      enum tagEnumPrice {Open, High = 1, Low = 2, Close, TrueHigh, TrueLow} m_nPriceHigh, m_nPriceLow;
    float m_fHighFactor, m_fLowFactor;

 public:
    virtual ~CTDChannel(){};

    CTDChannel(short nChannelType = 1) : CTDStudy() {
        m_nPeriod = 3;
        m_nMAType = Simple;
        m_bCenter = FALSE;
        m_nSmooth = 0;
        switch (nChannelType) {
        case 1:
            m_nPriceHigh = Low;
            m_nPriceLow = High;
            m_fHighFactor = 1.03f;
            m_fLowFactor = .97f;
            break;

        case 2:
            m_nPriceHigh = High;
            m_nPriceLow = Low;
            m_fHighFactor = 1.005f;
            m_fLowFactor = .995f;
            break;

        default:
            break;
        }
    }

//IBLPStudy Overrides
    virtual HRESULT ConvertBuf(void * pBuf) ;

    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};

class CTDLines : public CTDStudy,
                 public CTDBreakoutQualifier,
                 public IStudyTrendlines
{
//      DECLARE_DYNCREATE(CTDLines)
 private:
    class XProjectionLine 
        {
        public:
            float       fPrice;
            long        nFirstBar;
            long        nLastBar;
            ChartLineParameters LineParms;

            XProjectionLine(){
                fPrice = 0.0f;
                nFirstBar = 0;
                nLastBar = 0;
                LineParms.line_width = 1;
                LineParms.line_style = XRT_LPAT_SOLID;
                LineParms.line_color = RGB(255,255,255);
            }

            XProjectionLine(const XProjectionLine &SourceProjectionLine)
                {
                    fPrice = SourceProjectionLine.fPrice;
                    nFirstBar = SourceProjectionLine.nFirstBar;
                    nLastBar = SourceProjectionLine.nLastBar;
                }

            inline BOOL operator <(const XProjectionLine ProjectionLine2) const
                {
                    return nFirstBar < ProjectionLine2.nFirstBar;
                }

            XProjectionLine operator = (const XProjectionLine &SourceProjectionLine)
                {
                    fPrice = SourceProjectionLine.fPrice;
                    nFirstBar = SourceProjectionLine.nFirstBar;
                    nLastBar = SourceProjectionLine.nLastBar;

                    return SourceProjectionLine;
                }

        };
    long m_pTrendlineIds[MAX_TRENDLINES];
    ChartLineParameters m_QualifiedDemandLineParms, m_QualifiedSupplyLineParms,
        m_DisqualifiedLineParms, m_CancelLineParms;
 protected:
    float m_fProjectionPercent;
    enum tagEnumShowProjections {noProjections = 0, LastOnly = 2, LastWithFibonacci = 3, allProjections = 1} m_nProjections; // Number of projections per trendline
    short m_nNumTicks;  // # of Ticks required for breakout
    short m_nLookback;  // # bars to look back
    enumPriceType m_nPointPrice;
    short m_nMaxUsePoint; // # times a point can be reused
    short m_nLevel;
    BOOL  m_bUseTrueHighLow;
    BOOL  m_bDisplayDemand;
    BOOL  m_bDisplaySupply;
    BOOL  m_bDisplayQualified;
    BOOL  m_bDisplayDisqualified;
    BOOL  m_bDisplayTDPoints;
    BOOL  m_bCancel1;
    BOOL  m_bCancel2;
    BOOL  m_bCancel3;
    BOOL  m_bDisplayCancel;

 public:
    CTDLines() : CTDStudy() {
        m_nLevel = 1;
        m_nMaxUsePoint = 10;
        m_nLookback = 60;
        m_nNumTicks = 1;
        m_nProjections = allProjections;//LastWithFibonacci;
        m_fProjectionPercent = 100.0;
        m_bCancel1 = TRUE;
        m_bCancel2 = TRUE;
        m_bCancel3 = TRUE;
        m_bDisplayDemand = TRUE;
        m_bDisplaySupply = TRUE;
        m_bDisplayQualified = TRUE;
        m_bDisplayDisqualified = TRUE;
        m_bUseTrueHighLow = FALSE;
        m_bDisplayTDPoints = FALSE;
        m_bDisplayCancel = TRUE;
                        
        m_nPointPrice = CBLPStudy::High;
        ChartLineParameters line_params[4] = {
            {1, XRT_LPAT_SOLID, RGB(0, 255, 0)},                        // Demand                                       ChartLineParameters LineParms = {1, XRT_LPAT_SOLID, RGB(0, 255, 0)};    // Demand
            {1, XRT_LPAT_SOLID, RGB(255, 0, 0)},                        // Supply                                       ChartLineParameters LineParms = {1, XRT_LPAT_SOLID, RGB(0, 255, 0)};    // Demand
            {1, XRT_LPAT_LONG_DASH, RGB(0xF0, 0x64, 0)},        // Disqualified
            {1, XRT_LPAT_SOLID, RGB(192, 192, 192)}                     // Cancelled
        };
        memcpy(&m_QualifiedDemandLineParms, &(line_params[0]), sizeof(ChartLineParameters));
        memcpy(&m_QualifiedSupplyLineParms, &(line_params[1]), sizeof(ChartLineParameters));
        memcpy(&m_DisqualifiedLineParms, &(line_params[2]), sizeof(ChartLineParameters));
        memcpy(&m_CancelLineParms, &(line_params[3]), sizeof(ChartLineParameters));

        int i;
        for (i = 0; i < MAX_TRENDLINES; i++)
            m_pTrendlineIds[i] = 0;
    }

    IMPLEMENT_IUnknown(CTDLines);
    BEGIN_BLPINTERFACE_MAP(CTDLines, CBLPStudy2)
        SUPPORT_GEN_INTERFACE(IStudyTrendlines)
        END_BLPINTERFACE_MAP()

// IBLPStudy overrides
        // Interface - PURE virtual functions must be overrridden by derived classes
        virtual HRESULT DoStudy(IBLPGraphData *pGraph) ;
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph) ;
    virtual HRESULT ConvertBuf(void * pBuf) ;
    virtual HRESULT ConvertBufEx(StudyMessagePtr pBuf);

// IStudyTrendlines
    virtual HRESULT get_TrendlineCount(short *nCount)
        {
            *nCount = MAX_TRENDLINES;
            return S_OK;
        }

    virtual HRESULT get_TrendlineId(short Index, long *pnId)
        {
            ASSERT ((Index >= 0) && (Index < MAX_TRENDLINES));
            if ((Index >= 0) && (Index < MAX_TRENDLINES)) {
                *pnId = m_pTrendlineIds[Index];
                return S_OK;
            }
            else {
                *pnId = 0;
                return E_INVALIDARG;
            }
        }

protected:
	//This function will look at at both sides of fPrice, if this nBar's price
	//is higher or lower(decided by bHigh) than the nLevel bars adjacent
	//(those bars that are equal will not be counted). If not, it will return FALSE.
	//If we do not have enough bars to compare, it will return FALSE.
	BOOL IsTDPoint(float* fPrice, long nBar, long nLevel, BOOL bHigh);

};


class CTDDeMarker1 : public CBLPOscillatorStudy
{
 protected:
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   
        short instance_id;
        short base_id;

        short DeMarkerPeriod;  /* Period for DeMarker calculation */
        short duration; /* number of bars DeMarker needs to stay overbought or oversold */  
        short pad;
        OscillatorParameters zones;
        char higher_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        char lower_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        short high_lookback;  /* number of bars to lookback for new highs */
        short low_lookback;  /* number of bars to lookback for new lows */
        char use_equal;  /* Use = when comparing prices 0=no, 1=yes */
        char use_adjust; /* 0=no 1=yes should adjustments be made to calcs when period < 8 */
        char use_mavg; /* determines the moving average to apply 0=none, 1=simple, 2=exponential */
        char center; /* 0=no, 1=yes */    
        short MavgPeriod; /* moving average period */
        short smoothing; /* Number of bars for the smoothing */
        ChartLineParameters line_params[1]; 
        ChartPointParameters point_params;              
		ChartLineParameters ma_line_params; // Added for MA by Lu Sun
		short version;						// added by Lu Sun for version switch
		char  duration_on;		// If duration is on or off - added by Lu Sun
		char  show_maline;		// show moving average line or not - added by Lu Sun
		short ma_length;		// Moving average length - added by Lu Sun		
    } *StudyMessagePtr;

    BOOL m_bCenterMavg;
    short m_nSmooth;
    short m_nMavgPeriod;
    short m_nLowLookback;
    short m_nHighLookback;
//      enum tagEnumPrice {Open, High, Low, Close, HighLow, HighLowClose} m_nPriceHigh, m_nPriceLow;
    enumPriceType m_nPriceHigh, m_nPriceLow;
    short m_nDuration;
    short m_nPeriod;
	ChartLineParameters *m_pMALineParameter;	// class member of ma_line_params
												// added by Lu Sun
	short m_nVersion;		// version switch added by Lu Sun
	char  m_nDurationOn;	// class member of duration_on - added by Lu Sun
	char  m_nShowMALine;	// class member of show_ma - added by Lu Sun
	short m_nMALength;		// class member of ma_length - added by Lu Sun
        
    virtual float *GetStudyDataLine(IBLPGraphData *pGraph, enumPriceType type); 
    float *GetExtStudyDataLine(IBLPGraphData *pGraph, enumPriceType type);	//added by Lu Sun

 public:
    virtual ~CTDDeMarker1(){}

    CTDDeMarker1() {
        m_nPeriod = 8;
        m_nDuration = 16;
        m_zones.overbought = 60;
        m_zones.oversold = 40;
        m_nPriceHigh = High;
        m_nPriceLow = Low;
        m_nHighLookback = m_nLowLookback = 1;
        m_nMavgPeriod = 3;
        m_nSmooth = 0;
        m_bCenterMavg = FALSE;
        m_strButtonText = "TD DeMarker I";
        m_fMax = 101;
        m_fMin = -1;
    }

    // IBLPStudy overrides
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph) ;
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph) ;
    virtual HRESULT ConvertBuf(void * pBuf) ;
//IBLPOscillator        
};

class CTDDeMarker2 : public CTDDeMarker1
{
 protected:
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   
        short instance_id;
        short base_id;

        short rei_period;  /* Period for REI calculation */
        short duration; /* number of bars to REI needs to stay overbought or oversold */  
        short spare;
        OscillatorParameters zones;
        char higher_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        char lower_id[FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        short high_lookback;  /* number of bars to lookback for new highs */
        short low_lookback;  /* number of bars to lookback for new lows */
        char use_equal;  /* Use = when comparing prices 0=no, 1=yes */
        char use_adjust; /* 0=no 1=yes should adjustments be made to calcs when period < 8 */
        char use_mavg; /* determines the moving average to apply 0=none, 1=simple, 2=exponential */
        char center; /* 0=no, 1=yes */    
        short period; /* moving average period */
        short smoothing; /* Number of bars for the smoothing */
        short Qualifier; /*TD Price Oscillator Qualifier: enum tagEnumTDPOQ {Standard = 0, Advanced, none} */
        short UseAltOscillator; /*Default = TRUE*/
        short DisplayGaps; /*Default = TRUE*/
        short BarsBack; /*Max bars back for "mild" conditions - default = 6*/
        float UpperNeutral; /*Default = 50.0 */
        float LowerNeutral; /*Default = 50.0 */
        ChartLineParameters line_params[1]; // line 1 = Marker; other lines use point parameters
        ChartPointParameters point_params[3]; // line 2 = POQ Arrows; line 3 = POQ Open Arrows; line 4 = POQ Aom Arrows
		ChartLineParameters ma_line_params; // Added for MA by Lu Sun
		short version;						// added by Lu Sun for version switch
		char  duration_on;		// If suration is on or off - added by Lu Sun
		char  show_maline;		// show moving average line or not - added by Lu Sun
		short ma_length;		// Moving average length - added by Lu Sun
    } *StudyMessagePtr;

    enum tagEnumTDPOQ {none = 0, Standard = 1, Advanced = 2} m_nQualifier;
    BOOL m_bUseAltOscillator;
    BOOL m_bDisplayGaps;
    BOOL m_bDisplayPOQ;
    short m_nBarsBack;
    float m_fUpperNeutral;
    float m_fLowerNeutral;
                                                                                
 public:
    CTDDeMarker2() : CTDDeMarker1(){
        m_nQualifier = Standard; /*TD Price Oscillator Qualifier*/
        m_bUseAltOscillator = TRUE;
        m_bDisplayGaps = TRUE;
        m_nBarsBack = 6;
        m_fUpperNeutral = 50.0f;
        m_fLowerNeutral = 50.0f;
        m_bDisplayPOQ = FALSE;
        m_nPeriod = 8;
        m_strButtonText = "TD DeMarker II";
    }
        
    virtual ~CTDDeMarker2(){}

// IBLPStudy overrides
    virtual HRESULT ConvertBuf(void * pBuf);
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};

class CTDPressure : public CBLPOscillatorStudy
{
//      DECLARE_DYNCREATE(CTDPressure)
 protected:
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   
        short instance_id;
        short base_id;

        short nPeriod;
        short nROCPeriod;
                
        short nFormulaRatio; //enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR}
                
        short nUsePriceRatio; //enum tagEnumPrice {P, PV, V}
        float fGapPercent;
                
        short bDisplayPressureRatio, bDisplayROCRatio, bDisplayPI; // boolean

        // Line parameters
        ChartLineParameters line_params[3]; /* line1 = P Ratio , line2 = P ROC, line 3 = PI */

        // These are new msg variables for TD Pressure. Version will be 1.
        short nVersion, nRatioPeriod;
        short nOverbought, nOversold;
        long  fStartDate;  // this is not used on DLL side.. Only important on the big side to keep 
        // trak of the date from which the user is starting PI.
        short n_startBar; // the first bar to start calculation
        // For Ratio ROC and Cum. Index
        short nFormula[2]; 
        short nUsePrice[2];
  
		// New msg variables for TD Pressure Version 2. Shi F 01/30/03
        short nOverboughtROC;               // Overbought line on ROC panel
	    short nOversoldROC;                 // Oversold line on ROC panel 
        short nDurationBarsRatio;           // Bars for duration on Ratio panel
	    short nDurationBarsROC;             // Bars for duration on ROC panel
		char  cLowRiskUpArrowRatio;         // Low risk up arrows for Ratio on price chart
	    char  cLowRiskDownArrowRatio;       // Low risk down arrows for Ratio on price chart
	    char  cLowRiskUpArrowROC;           // Low risk up arrows for ROC on price chart
	    char  cLowRiskDownArrowROC;         // Low risk down arrows for ROC on price chart
	    char  cConsecutiveUpArrowRatio;     // Consecutive up arrows for Ratio on price chart
	    char  cConsecutiveDownArrowRatio;   // Consecutive down arrows for Ratio on price chart
	    char  cDurationEnabledRatio;        // Duration box on Ratio panel
	    char  cDurationEnabledROC;          // Duration box on roc panel
	
	} *StudyMessagePtr;

    short m_nPeriod;
    short m_nROCPeriod;
    short m_nVersion;
    short m_nRatioPeriod;
    enum tagEnumFormula {COHL, CpC, CO, CpCHL, CpCtR, COtR} m_nRatioFormula,m_nROCFormula,m_nIndexFormula;
    enum tagEnumPrice {P, PV, V} m_nRatioUsePrice, m_nROCUsePrice, m_nIndexUsePrice;
    float m_fGapPercent;
    BOOL m_bDisplayPI, m_bDisplayROCRatio, m_bDisplayPressureRatio;
    long m_nStartBar, m_fStartDate;
    short m_nOverbought, m_nOversold;
	
    // New member variables for TD Pressure Version 2. Shi F 01/30/03
	short m_nOverboughtROC;
	short m_nOversoldROC;
	short m_nDurationBarsRatio;
	short m_nDurationBarsROC;
    BOOL  m_bLowRiskUpArrowRatio;
	BOOL  m_bLowRiskDownArrowRatio;
	BOOL  m_bLowRiskUpArrowROC;
	BOOL  m_bLowRiskDownArrowROC;
	BOOL  m_bConsecutiveUpArrowRatio;
	BOOL  m_bConsecutiveDownArrowRatio;
	BOOL  m_bDurationEnabledRatio;
	BOOL  m_bDurationEnabledROC;
	enum {
	        TDPressureDurationRatioCalc= 1,
		    TDPressureDurationROCCalc= 2
			};
	
	// Lines and Points Parameters for the Charts
    ChartLineParameters m_pPressureLineParameters[3];
    CComPtr<IChart> m_pPRatio_Chart;
    CComPtr<IChart> m_pPROC_Chart;
    CComPtr<IChart> m_pPI_Chart;
    CComPtr<IChart> m_pArrow_Chart;   // Shi F 01/30/03
 
 public:
        CTDPressure() {
        m_nPeriod                 = 5;
        m_nROCPeriod              = 13;
        m_nRatioPeriod            = 89;
        m_nRatioFormula           = COtR;
        m_nRatioUsePrice          = P;
        m_fGapPercent             = 8.0f;
        m_bDisplayPI              = TRUE;
        m_bDisplayROCRatio        = FALSE;
        m_bDisplayPressureRatio   = TRUE;
        m_nStartBar               = 1;
        m_strButtonText = "TD Pressure";
        m_nROCFormula             = COtR;
        m_nROCUsePrice            = P;
        m_nIndexFormula           = COtR;
        m_nIndexUsePrice          = PV;

		//Init the new variables of version 2  Shi F 01/30/03
	    m_nOverboughtROC               = 107;
	    m_nOversoldROC                 = 93;
	    m_nDurationBarsRatio           = 6;
	    m_nDurationBarsROC             = 13;
        m_bLowRiskUpArrowRatio         = TRUE;
	    m_bLowRiskDownArrowRatio       = TRUE;
	    m_bLowRiskUpArrowROC           = FALSE;
	    m_bLowRiskDownArrowROC         = FALSE;
	    m_bConsecutiveUpArrowRatio     = TRUE;
	    m_bConsecutiveDownArrowRatio   = TRUE;
	    m_bDurationEnabledRatio        = TRUE;
	    m_bDurationEnabledROC          = TRUE;
		m_pDurationTopLine			   = NULL;
        m_pDurationBottomLine		   = NULL;
    }

    virtual ~CTDPressure(){}
 private:
    //If nBarsBack = 0, return nStartBar
    float CTDPressure::GetnRatioBack(float *DATA,
                                     long nStartBar,
                                     long nBarsBack);
    long  CTDPressure::ValidOHLC(float *OPEN, float *HIGH, float *LOW, 
                                 float *CLOSE, float *TRH, float *TRL, 
                                 short useFormula);
    HRESULT CTDPressure::CreatePressureRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE,
                                          float *TRH, float *TRL, float *VOL, long nPoints, 
                                          float *pfPressureRatio);

    HRESULT CTDPressure::CreateROCRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE,
                                     float *TRH, float *TRL, float *VOL, long nPoints, 
                                     float *pfRatioROC);

    HRESULT CTDPressure::CreatePressureIndex(float *OPEN, float *HIGH, float *LOW, float *CLOSE,
                                          float *TRH, float *TRL, float *VOL, long nPoints, 
                                          float *pfPressureIndex);
    HRESULT CTDPressure::GetPriceRatio(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRH, 
                                    float *TRL, float *VOL, long nPoints, float *pfWorkArray,
                                    short Formula, short Price);

	void CTDPressure::DurationCalc(float *pfPressureRatioROC, float *RatioROCDurationInside, float *RatioROCDurationOutside,
		                                short nDurationBarsRatioROC, short nRatioROCPeriod, short nOverBought, short nOverSold,
										long FirstBar, long LastBar, long RatioROC, BOOL *RatioROCFlag);
	
	void CTDPressure::ArrowCalc(float *OPEN, float *HIGH, float *LOW, float *CLOSE, float *TRUEHIGH, float *TRUELOW, 
								float *pfPressureRatio, float *pfPressureROC, float *LowRiskUpArrowRatio, float *LowRiskDownArrowRatio, 
								float *ConsecutiveUpArrowRatio, float *ConsecutiveDownArrowRatio, float *LowRiskUpArrowROC, 
								float *LowRiskDownArrowROC, short OverBoughtRatio, short OverSoldRatio, short OverBoughtROC, 
								short OverSoldROC,long FirstBar, long LastBar, BOOL *RatioFlag, BOOL *ROCFlag);

	long CTDPressure::GetnBarsBack(float *DATA, long nFirstBar, long nStartBar, long nBarsBack)	;
	

	// IBLPStudy overrides
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph) ;
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph) ;
    virtual HRESULT ConvertBuf(void * pBuf) ;
};

class CTDMavg1 : public CTDStudy
{
 protected:
    // specify study-specific parameters:
    short m_nPeriod;            // the MAVG period
    short m_nLookback;  // How far back to look for high/low
    enum tagEnumMavg {none = -1, simple = 0, exponential = 1, weighted = 2} m_nType;
    BOOL m_bCenter;
    short m_nSmoothing;
    float m_fHighFactor; // Multiple of the high
    float m_fLowFactor; // Multiple of the low
    char m_zHigherId[ FIELD_LEN + 1 ]; //use "TD007" for TD True High
    char m_zLowerId[ FIELD_LEN + 1 ];  //use "TD008" for TD True Low

 public:
    CTDMavg1() : CTDStudy(){
        m_nLookback = 5;
        m_fHighFactor = 1.0f;
        m_fLowFactor = 1.0f;
        m_nType = simple;
        m_bCenter = FALSE;
        m_strButtonText = "TD Mavg 1";
    }
        
    virtual ~CTDMavg1(){}

// IBLPStudy overrides
    virtual HRESULT ConvertBuf(void * pBuf);
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};

class CTDRangeProjections : public CTDStudy
{
 protected:
    BOOL m_bShowAll;
    BOOL m_bUseTrueHighLow;

 public:
    CTDRangeProjections() : CTDStudy()
        {
            m_bShowAll = FALSE;
            m_bUseTrueHighLow = TRUE;
            m_strButtonText = "TD Range Proj";
        }
    virtual ~CTDRangeProjections(){}

// IBLPStudy overrides
    virtual HRESULT ConvertBuf(void * pBuf);
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};


class CTDRei : public CBLPOscillatorStudy
{
 protected:
    typedef struct tagStructStudyMessageBuf {
        short                   msg_id; /* blgTDREI */
        short                   instance_id; 
        short                   base_id;
        short                   rei_period;     /* Period for REI calculation */
        short                   duration;       /* number of bars to REI needs to stay */  
        short                   spare;  /* used for 4 bytes pack in NT platform */
        OscillatorParameters    zones;  /* overbought, oversold */
        char                    higher_id[FIELD_LEN];   /* useless in bigside and dll side */
        char                    lower_id[FIELD_LEN];    /* useless in bigside and dll side */
        short                   high_lookback;  /* number of bars to lookback for new highs */
        short                   low_lookback;   /* number of bars to lookback for new lows */
        char                    use_equal;      /* Use = when comparing prices 0=no, 1=yes, not used in dll side */
        char                    use_adjust;     /* adjustments be made to calcs when period<8, not used in dll side */
        char                    use_mavg;       /* determines the moving average to apply, not used in dll side */
        char                    center; /* 0=no, 1=yes, not used in dll side */    
        short                   period; /* moving average period, not used in dll side */
        short                   smoothing;      /* Number of bars for the smoothing, not used in dll side */
        ChartLineParameters     line_params[1]; /* rei line parameters, only width and color are used, line style is ignored*/
        ChartPointParameters    point_params[1];        /* rei line point parameters, all ignored */
//      long                    reserved[4]; /* diliberatedly taken out to have the ability to deal with old message structure */
        /*
         * The following are added for TDREIV2
         */
        short                   version;        /* version control */
        short                   data_source;    /* data source, used for low risk up/down*/
        long                    overbought_color;       /* overbought line color */
        long                    oversold_color; /* oversold line color */
        long                    low_risk_up_color;      /* Low risk up arrow color */
        long                    low_risk_down_color;    /* Low risk down arrow color */
        long                    poss_exc_color; /* Possible Exception arrow color */
        short                   threshold_up;   /* up of the threshold*/
        short                   threshold_down; /* bottom of the threshold */
        short                   alt_days_back;  /* # days back will be affected in TD REI Alt */
        char                    duration_on;    /* Use duration on TD REI chart */
        char                    alternate_on;   /* Do TD REI Alt or not */
        char                    low_risk_up_on; /* Low risk up arrows on Bar chart */
        char                    low_risk_down_on;       /* Low risk down arrows on Bar chart */
        char                    poss_exc_up_on; /* Possible Exception Up arrows on Bar chart */
        char                    poss_exc_down_on;       /* Possible Exception Down arrows on Bar chart */
    } *StudyMessagePtr;

    short m_nLowLookback;
    short m_nHighLookback;
    enum tagEnumPrice {Open, High, Low, Close, HighLow, HighLowClose} m_nPriceHigh, m_nPriceLow;
    short m_nDuration;
    short m_nPeriod;
    enum tagEnumSource {UseTrueValue = 1, UseBarValue = 2} m_nPriceSource; //Price source for low risk up/down

    //New member variables used for TD REI Study
    long m_nReiColor;
    short m_nReiWidth;
    long m_nOverBoughtColor; 
    long m_nOverSoldColor;
    BOOL m_bDuration;
    BOOL m_bTDREIAlternate;
    BOOL m_bLowRiskUp;
    BOOL m_bLowRiskDown;
    BOOL m_bPossExcUp;
    BOOL m_bPossExcDown;
    short m_nThresholdUp;
    short m_nThresholdDown;
    short m_nAltDaysBack;
    long m_nLowRiskUpColor;
    long m_nLowRiskDownColor;
    long m_nPossExcColor;
    CComPtr<IChart> m_CArrowChart;
    CComPtr<IChart> m_CReiChart;

 public:
    CTDRei() {
        m_nPeriod = 5;
        m_nDuration = 6;
        m_zones.overbought = 40;
        m_zones.oversold = -40;
        m_nPriceHigh = High;
        m_nPriceLow = Low;
        m_nHighLookback = m_nLowLookback = 2;
        m_pDurationTopLine = NULL;
        m_pDurationBottomLine = NULL;
        m_fMax = 110;
        m_fMin = -110;
        m_nPriceSource = UseTrueValue;
    }
    virtual ~CTDRei(){}

    //Impement the virtual method
    virtual HRESULT ConvertBuf(void * pBuf);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
 private:
    enum {
        OPT_OPEN,
        OPT_HIGH,
        OPT_LOW,
        OPT_CLOSE,
        OPT_MIDPT,
        OPT_PIVOT };

    enum {
        MildlyOverBought = 1,
        MildlyOverSold = -1};
        
    //Calculate TD REI Standard
    void CTDRei::TDReiCalc(
        float *OPEN,
        float *HIGH,
        float *LOW,
        float *CLOSE,
        long  TDReiPrice1,              // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev highs
        long  TDReiPrice2,              // 0 1 2 3 4 5  OHLC Mid Piv  -- compare to prev lows
        long  TDReiBarsBack1,   // bars back for highs offset comparison
        long  TDReiBarsBack2,   // bars back for lows offset comparison
        BOOL  TDReiUseEqual,    // TRUE for use equal when comparing
        short nPeriod,
        float *Rei,
        long  first_bar,                // calc only this many bars from end (speedup)          
        long  last_bar);

    //Calculate TD REI Alternate based on TD REI Standard
    void CTDRei::TDReiAltCalc(
        short nAltDaysBack,   //How many days back
        float *Rei,
        long first_bar,
        long last_bar);

    //Calcaute duration box
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
        long last_bar);

    //Calculate arrows
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
        long last_bar);
        
    void CTDRei::TDReiMonitorLastBar(float *PossExcOverBought,
                                     float *PossExcOverSold,
                                     float *LOW,
                                     float *HIGH,
                                     float *CLOSE,
                                     long last_bar);
        
    void CTDRei::fillter(float *pfBuyArrows,
                         float *pfSellArrow,
                         long *pnDuration,
                         long first_bar,
                         long last_bar);
        
    //change char to BOOL
    BOOL CTDRei::TDReiCharToBOOL(char flag);

    //Get the max value of x, y
    long CTDRei::Max(long x, long y);

    //Get the valid nBarsBack.
    //Return -1 if fail.
    //If nBarsBack = 0, return nStartBar
    long CTDRei::GetnBarsBack(float *DATA,
                              long nFirstBar,
                              long nStartBar,
                              long nBarsBack);

    //If one of the data is invalid, including HOLEF, or HIGH < LOW
    //it will reset all the value to HOLEF
    long CTDRei::ValidateData(float *OPEN,
                              float *CLOSE,
                              float *HIGH,
                              float *LOW,
                              long first_bar,
                              long last_bar);

    //Copy data from pfSrc to pfDes
    void CTDRei::CopyData(float *pfSrc,
                          float *pfDes,
                          long first_bar,
                          long last_bar);
};

class CTDRetracement : public CBLPRetracement
{
 public:
    typedef struct tagStructStudyMessageBuf {
        union {   
            struct {
                short msg_id;   
                short instance_id;
                short base_id;
            } StudyHeader;

            struct {
                short msg_id;   //      blgTDTrendFactor 1041
                short instance_id;
                short base_id;

                BOOL bSave;
                BOOL bDirection; // TRUE means Down retracement
                CBLPStudy::enumPriceType nProjectionPrice; // default High for downside retracement; Low for upside
                long nMaxLookback; // default 60
                BOOL bQualifier1;
                BOOL bQualifier2;
                BOOL bQualifier3;
                BOOL bCancel1;
                BOOL bCancel2;
                BOOL bCancel3;
                BOOL bDisplayProjections;
                BOOL bDisplayHalfLevels;

                char zButtonText[32];
                char zErrorText[80];
                ChartLineParameters line_params[1];
            } TDTrendFactor;
  
        }; // end of study specific union

        long reserved[4]; //reserved tail for all study messages
    } *StudyMessagePtr;

 public:
    virtual HRESULT ConvertBuf(void *pBigBuf)
        {
            CBLPRetracement::ConvertBuf(pBigBuf);
            StudyMessagePtr pBuf = (StudyMessagePtr)pBigBuf;
            return NOERROR;
        }
};

class CTDRelativeRetracement : public CTDRetracement
{
 public:
    CTDRelativeRetracement()
        {
            m_nMaxLookback = 180;
            m_bDisplayMagnet = TRUE;
            m_bDisplayAdditional = TRUE;
            m_nPenetration = 1;
            memset(m_LineParms, -1, sizeof(m_LineParms));
            m_bSaveTrendline = FALSE;
            m_strButtonText = "Relative";

            m_nNumStudyLines = 2;
        }

    virtual HRESULT ConvertBuf(void *pBuf);
    virtual HRESULT DoRetracement(long x_value, BOOL bDirection,
                                  // Interface - PURE virtual functions must be overrridden by derived classes
                                  IBLPGraphData *pGraph);

 protected:
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   //      blgTDRelativeRetracement 1039
        short instance_id;
        short base_id;

        long  nVersion;
        BOOL  bDirection; // TRUE means Down retracement
        CBLPStudy::enumPriceType nProjectionPrice; // default High for downside retracement; Low for upside
        long  nMaxLookback; // no longer in use.  will always go as far back as we have data
        short nPenetration; // default 1
        BOOL  bQualifier1;
        BOOL  bQualifier2;
        BOOL  bQualifier3;
        BOOL  bCancel1;
        BOOL  bCancel2;
        BOOL  bCancel3;
        BOOL  bDisplayProjections;
        BOOL  bDisplayMagnet;
        BOOL  bDisplayAdditional;

        char  zButtonText[32];
        char  zErrorText[28];

        float fPrecent_retracement[6];
        BOOL  bToggle[6];
        long  nNum_retracements;

        ChartLineParameters line_params[2];
    } *StudyMessagePtr;
    long m_nMaxLookback;
    short m_nPenetration;
    BOOL m_bDisplayMagnet;
    BOOL m_bDisplayAdditional;
    ChartLineParameters m_LineParms[2];
    float m_fPrecentRetracement[6];
    BOOL m_bToggle[6];
    long m_nNum_retracements;
    ChartLineParameters m_ArrowLineParms[1];
    ChartPointParameters m_ArrowPointParms[1]; 
};

class CTDTrendFactor : public CTDRetracement
{
 protected:
// member variables passed down from the BIG
    long m_nMaxLookback;
    BOOL m_bDisplayHalfLevels;
    ChartLineParameters m_LineParms[1];
    long m_nVersion;
    BOOL m_bShowLevel[9];

 public:
    typedef struct tagStructStudyMessageBuf{
        short msg_id;   //      blgTDTrendFactor 1041
        short instance_id;
        short base_id;

        long nVersion;
        BOOL bDirection; // TRUE means Down retracement
        CBLPStudy::enumPriceType nProjectionPrice; // default High for downside retracement; Low for upside
        long nMaxLookback; // default 60
        BOOL bQualifier1;
        BOOL bQualifier2;
        BOOL bQualifier3;
        BOOL bCancel1;
        BOOL bCancel2;
        BOOL bCancel3;
        BOOL bDisplayProjections;
        BOOL bDisplayHalfLevels;
        long bShowLevel[9];

        char zButtonText[32];
        char zErrorText[80];
        ChartLineParameters line_params[1];
    } *StudyMessagePtr;

    CTDTrendFactor() : CTDRetracement()
        {
            m_nMaxLookback = 60;
            m_bDisplayHalfLevels = FALSE;
            m_strButtonText = "Trend Factor";
        }

// IBLPStudy overrides
    virtual HRESULT ConvertBuf(void *pBuf);
    virtual HRESULT DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph);
};

class CTDAbsoluteRetracement : public CTDRetracement
{
 public:
    CTDAbsoluteRetracement()
        {
//              m_strButtonText = "Absolute";
        }
        
    // IBLPStudy overrides
    virtual HRESULT ConvertBuf(void *pBuf);
    virtual HRESULT DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph);

 protected:
    // member variables
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   //      blgTDAbsoluteRetracement 1040
        short instance_id;
        short base_id;
                
        long nVersion;
        BOOL bDirection; // TRUE means Down retracement
        CBLPStudy::enumPriceType nProjectionPrice; // default High for downside retracement; Low for upside
        BOOL bQualifier1;
        BOOL bQualifier2;
        BOOL bQualifier3;
        BOOL bCancel1;
        BOOL bCancel2;
        BOOL bCancel3;
        BOOL bDisplayProjections;
                
        char zButtonText[32];
        char  zErrorText[32];
                
        float fPrecent_retracement[6];
        BOOL  bToggle[6];
//              long  nProject;
        ChartLineParameters line_params;
    } *StudyMessagePtr;
    float m_fPrecentRetracement[6];
    BOOL m_bToggle[6];  
//      long m_nProject;
    ChartLineParameters m_LineParms;
};


class CTDMav1ex : public CTDStudy
{
 protected:
    //Message structure sending from the big side
    typedef struct tagStructStudyMessageBuf {
        short msg_id;   
        short instance_id;
        short base_id;

        char type[2];  //type of the moving average 0=simple  

        char higher_id[2][FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        char lower_id[2][FIELD_LEN]; /* high=2,low=3,open=4,close=5 */
        short period[2];       /* Length of the moving average */  
        short high_factor[2];  
        short low_factor[2];  
        short lookback[2];  /* number of bars to lookback for new highs/lows */ 
        ChartLineParameters line_params[2][2]; /* line1 = high, line2 = low */
        short version;
        short bars_to_extend[2];
        short projector[2];
        short toggle_switch[2][2];
        short num_study;
        short toggle_lookback;
    } *StudyMessagePtr;
        
    // specify study-specific parameters:
    short m_nPeriod[2];         // the MAVG period
    short m_nLookback[2];       // How far back to look for high/low
    enum tagEnumMavg {none = -1, simple = 0, exponential = 1, weighted = 2} m_nType[2];
    BOOL m_bCenter;
    float m_fHighFactor[2]; // Multiple of the high
    float m_fLowFactor[2]; // Multiple of the low
    char m_zHigherId[2][ FIELD_LEN + 1 ]; //use "TD007" for TD True High
    char m_zLowerId[2][ FIELD_LEN + 1 ];  //use "TD008" for TD True Low
    short m_nBarsToExtend[2];
    short m_nProjector[2];
    short m_nToggleSwitch[2][2];
    short m_nNumStudy;
    short m_nToggleLookback;

 public:
    CTDMav1ex(){
        int i = 0;

        for(i = 0; i < 2; i++){
            m_nLookback[i] = 5;
            m_fHighFactor[i] = 1.0f;
            m_fLowFactor[i] = 1.0f;
            m_nType[i] = simple;
            m_nBarsToExtend[i] = 4;
            m_nProjector[i] = 0;
            m_nToggleSwitch[i][0] = 0;
            m_nToggleSwitch[i][1] = 0;
            m_nNumStudy = 1;
        }

        m_strButtonText = "TD Mavg 1";
    }


    virtual ~CTDMav1ex(){}

// IBLPStudy overrides
    virtual HRESULT ConvertBuf(void * pBuf);
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

 private:
    HRESULT GetTDmav1exStudy(IBLPGraphData *pGraph, int i, int num_graphs,
                             float *pfTDMAVG1High1, float *pfTDMAVG1Low1);
    HRESULT Get2GetTDmav1exStudy(IBLPGraphData *pGraph, float *pfTDMAVG1High1, float *pfTDMAVG1Low1, 
                                 float *pfTDMAVG1High2, float *pfTDMAVG1Low2);
};

class CTDROC : public CBLPOscillatorStudy
{
 protected:
    typedef struct tagStructStudyMessageBuf {
        short msg_id;
        short instance_id;
        short base_id;

        short period;       /* Length of the moving average */  
        char field_id[FIELD_LEN]; /*This is a fortran string*/
        short duration_type;
        float overbought;
        float oversold;
        ChartLineParameters line_params; /* line1 = high, line2 = low */ 
        ChartPointParameters point_params;
        short version;
		short duration_length;
    } *StudyMessagePtr;
        
    // specify study-specific parameters:
    short m_nPeriod;            // the TDROC period
    char m_zField[FIELD_LEN + 1]; 
    short m_nDuration_type;
    float m_fOverbought;
    float m_fOversold;
    short m_nVersion;
	short m_nDuration_length;

    float *m_pfTDROC;   

    CComPtr<IChart> m_pTDROC_Chart;
	enum {NONE = 0, OVERBOUGHT = 1, OVERSOLD = 2, BOTH = 3};
 public:
    CTDROC(){
        m_strButtonText = "TD ROC";
    }
        
    virtual ~CTDROC(){}

// IBLPStudy overrides 
    virtual HRESULT ConvertBuf(void * pBuf);
    // Interface - PURE virtual functions must be overrridden by derived classes
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
    virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
};



class CTDDWave : public CTDStudyWithCountdown,
                 public IStudyTrendlines

{
protected:
	typedef struct tagStructStudyMessageBuf
	{
		short msg_id; // TDWave 1045
		short instance_id;
		short base_id;
		char wave_direction[2];
		char condition[2][2];
		short level[DWAVE_MAX_WAVES];
		short color[DWAVE_MAX_WAVES];
		char on[DWAVE_MAX_LINES];
		char from[DWAVE_MAX_LINES];
		char to[DWAVE_MAX_LINES];
		float times[DWAVE_MAX_LINES];
		char dir[DWAVE_MAX_LINES];
		char at[DWAVE_MAX_LINES];
		char stop[DWAVE_MAX_LINES];
		short pcolor[DWAVE_MAX_LINES];
		char min_waves[2];
		char wave_height[2];
		char true_vals[2];
		short version;
	} *StudyMessagePtr;

public:
	virtual ~CTDDWave(){}

	IMPLEMENT_IUnknown(CTDDWave);
	BEGIN_BLPINTERFACE_MAP(CTDDWave, CTDStudyWithCountdown)
		SUPPORT_GEN_INTERFACE(IStudyTrendlines)
	END_BLPINTERFACE_MAP()

    virtual HRESULT ConvertBuf(void * pBuf);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);

//IStudyTrendlines
	virtual HRESULT get_TrendlineCount(short *nCount)
	{
		*nCount = MAX_TRENDLINES;
		return S_OK;
	}

	virtual HRESULT get_TrendlineId(short Index, long *pnId)
	{
		if ((Index >= 0) && (Index < MAX_TRENDLINES)) {
			*pnId = m_pTrendLineIds[Index];
			return S_OK;
		}
		else {
			*pnId = 0;
			return E_INVALIDARG;
		}
	}

	CTDDWave();

private:
	char m_wave_direction[2];
	char m_condition[2][2];
	long m_level[DWAVE_MAX_WAVES];
	long m_color[DWAVE_MAX_WAVES];
	char m_on[DWAVE_MAX_LINES];
	char m_from[DWAVE_MAX_LINES];
	char m_to[DWAVE_MAX_LINES];
	float m_times[DWAVE_MAX_LINES];
	char m_dir[DWAVE_MAX_LINES];
	char m_at[DWAVE_MAX_LINES];
	char m_stop[DWAVE_MAX_LINES];
	long m_nTrendLineIndex;
	long m_pTrendLineIds[MAX_TRENDLINES];
	long m_pcolor[DWAVE_MAX_LINES];
	char m_min_waves[2];
	char m_wave_height[2];
	char m_true_vals[2];

	HRESULT GetData(float *pfDataArray[6], IBLPGraphData *pGraph);

	void SetUpOrDown(short *pbUpOrDown);

	long FindWaveMinimum(short nMinWaveNumber, long nPrevMinWave, short *pbUpOrDown, 
								long *pnWaves, long nWaveIndex, float *pfDataArray[6]);
	
	long FindWave(short nWaveNumber, long nPrevMinWave, long nMinWave, short *pbUpOrDown, 
								long *pnWaves, long nWaveIndex, float*pfDataArray[6]);

	HRESULT AdjustPrevWaves(long *pnWaves, short *pnWaveNums, long &nWaveIndex,
								   short *pbUpOrDown, float *pfDataArray[6]);

	HRESULT AdjustLastMin(long  *pnMinWaves, short *pnMinWaveNums, long &nMinWaveIndex,
			long  *pnWaves, long &nWaveIndex, short *pbUpOrDown, float *pfDataArray[6]);


	bool DoWaveRelabeling(short &nWaveNumber, short &nMinWaveNumber, long &nPrevMinWave,
								long *pnWaves, short *pnWaveNums, long &nWaveIndex, 
								long *pnMinWaves, short *pnMinWaveNums, long &nMinWaveIndex, 
								short *pbUpOrDown, float *pfDataArray[6]);

	HRESULT DrawWaves(long  *pnMinWaves, short *pnMinWaveNums, long nMinWaveIndex,
						long  *pnWaves, short *pnWaveNums, long nWaveIndex, 
						short *pbUpOrDown, float *pfDataArray[6]);


	HRESULT FindProjections(long *pnWaves, short *pnWaveNums, long nWaveIndex, 
		float *pfDataArray[6], IBLPGraphData *pGraph);

	void DrawProjection(long nFromPos, long nToPos, long  nAtPos, long nStopPos, 
		short nProjectionNumber, float *pfDataArray[6], CComPtr<ITrendlineManager> iTrendlineMgr);

};

#endif // __DEMARKSTUDIES_H__

