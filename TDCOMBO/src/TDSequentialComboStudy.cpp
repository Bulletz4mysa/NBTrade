#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

/***********************************************************************************
 **********************************  TD Sequential ********************************* 
 ***********************************************************************************/

HRESULT CTDSequential::ConvertQualifier(structQualifier *pQualifier)
{
    pQualifier->nPrice1 = (enumPriceType)ntohl(pQualifier->nPrice1);
    pQualifier->nBar1 = ntohs(pQualifier->nBar1);
    pQualifier->nBar2 = ntohs(pQualifier->nBar2);
    pQualifier->nCondition = (enumQualifierCondition)ntohl(pQualifier->nCondition);
    pQualifier->nPrice2 = (enumPriceType)ntohl(pQualifier->nPrice2);
        
    return NOERROR;
}

HRESULT CTDSequential::ConvertBuf(void * pBigBuf)
{
    CBLPStudy::ConvertBuf(pBigBuf);
    StudyMessagePtr pBuf = (StudyMessagePtr)pBigBuf;
        
    long *pConvertFloat;
    pBuf->nSetupLookback       = ntohs(pBuf->nSetupLookback); 
    pBuf->nMinSetupBars        = ntohs(pBuf->nMinSetupBars); 
    pBuf->nSetupPrice          = (enumPriceType)ntohl(pBuf->nSetupPrice); 
    pBuf->nCancelRule          = ntohl(pBuf->nCancelRule); 
    pBuf->nRecycleRule         = ntohl(pBuf->nRecycleRule); 
    pBuf->nStartIntersection   = ntohs(pBuf->nStartIntersection); 
    pBuf->nIntersectionRule    = (enumIntersectionRule)ntohl(pBuf->nIntersectionRule); 
    pBuf->nCountdownLookback   = ntohs(pBuf->nCountdownLookback); 
    pBuf->nMinCountdown        = ntohs(pBuf->nMinCountdown); 
    pBuf->nCountdownPrice      = (enumPriceType)ntohl(pBuf->nCountdownPrice); 
    pBuf->nVersion             = ntohl(pBuf->nVersion);
    pBuf->nIgnoreSetupRule = ntohl(pBuf->nIgnoreSetupRule);
    
    // Line 0 = Sell Setup Shading; Line 1 = Buy Setup Shading;
    // Line 2 = Setup numbering; Line 4 = Countdown numbering
    ConvertLineParameters(pBuf->line_params, 4);
    ConvertQualifier(&(pBuf->CountdownQualifier2));
    
    // Setup parameters
    m_nSetupLookback   = pBuf->nSetupLookback;
    m_nMinSetupBars    = pBuf->nMinSetupBars;
    m_bShowFullSetup   = pBuf->bShowFullSetup;
    m_nSetupPrice      = pBuf->nSetupPrice;
        
    // Cancel and Recycle rules
    m_nCancelRule                   = pBuf->nCancelRule;
    m_nRecycleRule                  = pBuf->nRecycleRule;
        
    // Intersection rule
    m_nStartIntersection = pBuf->nStartIntersection;
    m_nIntersectionRule  = pBuf->nIntersectionRule;
        
    // Countdown parameters
    m_nCountdownLookback  = pBuf->nCountdownLookback;
    m_nMinCountdownBars       = pBuf->nMinCountdown;
    m_nCountdownPrice     = pBuf->nCountdownPrice;
        
        
    // m_CountdownQualifier2.nPrice1 will be used as the compare price
    // for termination count.  See DoStudy
    m_bAndCountdownQualifiers = pBuf->bAndCountdownQualifiers;
    // Line parameters
    m_pLineParameters         = pBuf->line_params;
    m_nTerminationCountPrice = pBuf->CountdownQualifier2.nPrice1;
    
    //This is done for backward compatibility. We will use SetupQualifier.bUse to distinguish the old big message and new big message
    if (pBuf->nVersion >= 2)
    {
        pBuf->nComboVersion            = ntohl(pBuf->nComboVersion);
        pBuf->nComboCountdownUsesEqual = ntohs(pBuf->nComboCountdownUsesEqual);
        m_nComboVersion                = pBuf->nComboVersion;
        m_nComboCountdownUsesEqual     = pBuf->nComboCountdownUsesEqual;
        
        pBuf->nRecycleCount               = ntohs(pBuf->nRecycleCount);
        m_nRecycleCount                   = pBuf->nRecycleCount;
        pBuf->nSetupWithinSetupOptions[0] = ntohs(pBuf->nSetupWithinSetupOptions[0]);
        m_nSetupWithinSetupOptions[0]     = pBuf->nSetupWithinSetupOptions[0];
        pBuf->nSetupWithinSetupOptions[1] = ntohs(pBuf->nSetupWithinSetupOptions[1]);
        m_nSetupWithinSetupOptions[1]     = pBuf->nSetupWithinSetupOptions[1];
                
        pBuf->nStopLossLevelOption = ntohs(pBuf->nStopLossLevelOption);
        m_nStopLossLevelOption = pBuf->nStopLossLevelOption;
        
        m_bShowSetup               = (pBuf->cShowSetup == 'Y') ? TRUE : FALSE;
        m_bShowSetupQualifier      = (pBuf->cShowSetupQualifier == 'Y') ? TRUE : FALSE;
        m_bShowFullSetup           = (pBuf->bShowFullSetup) ? TRUE : FALSE;
        m_bShowCountdown           = (pBuf->cShowCountdown == 'Y') ? TRUE : FALSE;
        m_bShowCountdownCompletion = (pBuf->cShowCountdownCompletion == 'Y') ? TRUE : FALSE;
        m_bShowStopLossLevel       = (pBuf->cShowStopLossLevel == 'Y') ? TRUE : FALSE;
        m_bRecycleConfirm          = (pBuf->cRecycleConfirm == 'Y') ? TRUE : FALSE;
        m_bShowShade               = (pBuf->cShowShade == 'Y') ? TRUE : FALSE;
        m_bShowTDST                = (pBuf->cShowTDST == 'Y') ? TRUE : FALSE;
        m_bExtendTDST              = (pBuf->cExtendTDST == 'Y') ? TRUE : FALSE;
        m_bRecycleCount            = (pBuf->cRecycleCount == 'Y') ? TRUE : FALSE;
        m_bLastCountdownQualifier  = (pBuf->cLastCountdownQualifier == 'Y') ? TRUE : FALSE;
        m_bShowFullCountdown       = (pBuf->cShowFullCountdown == 'Y') ? TRUE : FALSE;
        
		//Sequential Variables added for TREQ 36665 by Arun
		if(pBuf->cEnableSWithinS == 'Y') m_bEnableSWithinS = TRUE;
		else m_bEnableSWithinS = FALSE;
		
		if(pBuf->cReverseSetupCancel == 'Y') m_bReverseSetupCancel = TRUE;
		else m_bReverseSetupCancel = FALSE;

		m_nShowSetupTo = pBuf->cShowSetupTo % 10;

		//Combo Variables added for TREQ 36665 by Arun
        if(((pBuf->cCntCondCombo & 32) / 32) == 1) m_bEnableSWSCombo = TRUE;
		else m_bEnableSWSCombo = FALSE;

		if(((pBuf->cCntCondCombo & 16) / 16) == 1) m_bRevCancelCombo = TRUE;
		else m_bRevCancelCombo = FALSE;
        
		m_nShowSetupToCombo = (pBuf->cShowSetupTo/10)%10;
		
		if(((pBuf->cCntCondCombo & 1) / 1) == 1) m_nSellCntCond1Combo =2;
		else m_nSellCntCond1Combo =1;
		
		if(((pBuf->cCntCondCombo & 2) / 2) == 1) m_nSellCntCond2Combo =2;
		else m_nSellCntCond2Combo =1;

		if(((pBuf->cCntCondCombo & 4) / 4) == 1) m_nBuyCntCond1Combo =2;
		else m_nBuyCntCond1Combo =1;

		if(((pBuf->cCntCondCombo & 8) / 8) == 1) m_nBuyCntCond2Combo =2;
		else m_nBuyCntCond2Combo =1;
		// End of changes for TREQ 36665

        pConvertFloat        = (long *)&(pBuf->fLargeRecycleLevel);
        *pConvertFloat       = ntohl(*pConvertFloat);
        m_fLargeRecycleLevel = *((float *)pConvertFloat);
                
        pConvertFloat        = (long *)&(pBuf->fSmallRecycleLevel);
        *pConvertFloat       = ntohl(*pConvertFloat);
        m_fSmallRecycleLevel = *((float *)pConvertFloat);
        
        if (m_pLineParameters[0].line_style < 0)
        {
            m_pLineParameters[0].line_style = -m_pLineParameters[0].line_style;
        }
                
        if (m_pLineParameters[1].line_style < 0)
        {
            m_pLineParameters[1].line_style = -m_pLineParameters[1].line_style;
        }

    }
    else
    {
        pConvertFloat        = (long *)&(pBuf->fLargerSetupFactor);
        *pConvertFloat       = ntohl(*pConvertFloat);
        m_fLargeRecycleLevel = *((float *)pConvertFloat);
                
        m_nStopLossLevelOption = AllCountdownBars;
        m_nRecycleRule = ntohl(pBuf->nIgnoreSetupRule);
                
        if (m_fLargeRecycleLevel >= 10)
        {
            m_nRecycleCount       = (short)m_fLargeRecycleLevel - 1;
            m_fLargeRecycleLevel -= m_nRecycleCount;
        }
                
        if (m_pLineParameters[0].line_style < 0)
        {
            m_bShowTDST                     = FALSE;
            m_pLineParameters[0].line_style = -m_pLineParameters[0].line_style;
            m_pLineParameters[1].line_style = -m_pLineParameters[1].line_style;
        }
        else
            m_bShowTDST = TRUE;
                
        if (m_pLineParameters[1].line_style == XRT_BACKGROUND_NONE)
            m_bShowShade = FALSE;
        else
            m_bShowShade = TRUE;
    }
        
    return NOERROR;
}


HRESULT CTDSequential::CreateStudy(IBLPGraphStudyList * pGraph)
{
    ChartLineParameters LineParms[] =
        {
            {1, XRT_LPAT_SOLID, blgGreen},                // Sell Setup Count
            {1, XRT_LPAT_SOLID, blgGreen},                // Buy Setup Count
            {3, XRT_LPAT_SOLID, RGB(128, 0, 0)},                // Sell Shade
            {3, XRT_LPAT_SOLID, RGB(0, 128, 0)},                // Down Shade
            {1, XRT_LPAT_SOLID, blgRed},                // Sell Countdown
            {1, XRT_LPAT_SOLID, blgRed},                // Buy Countdown
            {1, XRT_LPAT_SOLID, blgRed},                // Sell Countdown
            {1, XRT_LPAT_SOLID, blgRed},                // Buy Countdown
            {1, XRT_LPAT_SOLID, blgRed},
            {1, XRT_LPAT_SOLID, blgGreen},
			{1, XRT_LPAT_SOLID, blgYellow}
        };
        
    ChartPointParameters PointParms[] = 
        {
            {XRT_POINT_SEQUENTIAL_UP,0,0},                      // Sell Setup Count
            {XRT_POINT_SEQUENTIAL_DOWN,0,0},                    // Buy Setup Count
            {XRT_BACKGROUND_SHADE,0,0},                         // Sell Shade
            {XRT_BACKGROUND_SHADE,0,0},                         // Down Shade
            {XRT_POINT_INDEX_ABOVE,0,0},                        // Sell Countdown
            {XRT_POINT_INDEX_BELOW,0,0},                        // Buy Countdown
            {XRT_POINT_INDEX_ABOVE_RAISED,0,0},                 // Sell Countdown
            {XRT_POINT_INDEX_BELOW_DROPPED,0,0},                // Buy Countdown
            {XRT_POINT_DOWNARROW, 0, 0},                        // Sell Setup and Countdown Arrows
            {XRT_POINT_UPARROW, 0, 0},                           // Buy Setup and Countdown Arrows   
			{XRT_POINT_INDEX_ABOVE, 0, 0}		
        };
        
    PointParms[4].point_style += 100 * m_nCountdownDisplayIndex;
    PointParms[5].point_style += 100 * m_nCountdownDisplayIndex;
    PointParms[6].point_style += 100 * m_nCountdownDisplayIndex;
    PointParms[7].point_style += 100 * m_nCountdownDisplayIndex;
        
    LineParms[4].line_color = m_pLineParameters[3].line_color;
    LineParms[5].line_color = m_pLineParameters[3].line_color;
    LineParms[6].line_color = m_pLineParameters[3].line_color;
    LineParms[7].line_color = m_pLineParameters[3].line_color;
        
    int rc = AddStudy(pGraph, 10, 10, LineParms, PointParms);
    if (FAILED(rc))
        return rc;
        
    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    CComPtr<IChart> iChart;
    if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK)
    {
        AddChart(iChart);
        iGraphToolbar->AddStudyButton(0, m_strButtonText, this);
    }
        
    return rc;//CBLPStudy::CreateStudy(pGraph);
}


#include "logo.h"
#define NewCountdown
HRESULT CTDSequential::DoStudy(IBLPGraphData *pGraph)
{
 	long nNumPoints = m_nNumPoints;
    pGraph->get_NumPoints(&m_nNumPoints);
        
    m_vDeMarkCountdowns.clear();
    m_nTrendLineIndex = 0;
        
    //Special for Countdown
    if ((m_pnCountdownDisplay == NULL) || (m_nNumPoints != nNumPoints))
    {
        delete []m_pnCountdownDisplay;
        m_pnCountdownDisplay = new int[m_nNumPoints];
        if (m_pnCountdownDisplay == NULL)
            return E_OUTOFMEMORY;
    }
    
    //Reset the Countdown
    memset(m_pnCountdownDisplay, 0, m_nNumPoints * sizeof(int));
        
    /*
     * We need to initliaze the member variables before calling GetComparePrice()
     */
    m_pfClose = GetStudyDataLine(pGraph, Close);
    ASSERT(m_pfClose != NULL);
    if (m_pfClose == NULL)
        return E_HANDLE;
        
    m_pfHigh = GetStudyDataLine(pGraph, High);
    ASSERT(m_pfHigh != NULL);
    if (m_pfHigh == NULL)
        return E_HANDLE;
        
    m_pfLow = GetStudyDataLine(pGraph, Low);
    ASSERT(m_pfLow != NULL);
    if (m_pfLow == NULL)
        return E_HANDLE;
        
    m_pfOpen = GetStudyDataLine(pGraph, Open);
    ASSERT(m_pfOpen != NULL);
    if (m_pfOpen == NULL)
        return E_HANDLE;
        
    m_pfTrueHigh = GetStudyDataLine(pGraph, TrueHigh);
    ASSERT(m_pfTrueHigh != NULL);
    if (m_pfTrueHigh == NULL)
        return E_HANDLE;
    
    m_pfTrueLow = GetStudyDataLine(pGraph, TrueLow);
    ASSERT(m_pfTrueLow != NULL);
    if (m_pfTrueLow == NULL)
        return E_HANDLE;
    
    float *pfCancelMax;
    float *pfCancelMin;
    switch (m_nCancelRule) {
    case LowVsHigh:
    case HighVsHigh:
    case CloseVsHigh:
        pfCancelMax = m_pfHigh;
        pfCancelMin = m_pfLow;
                
    case HighVsTrueHigh:
    case CloseVsTrueHigh:
    case LowVsTrueHigh:
    case TrueLowVsTrueHigh:
        pfCancelMax = m_pfTrueHigh;
        pfCancelMin = m_pfTrueLow;
        break;
                
    case HighVsClose:
        pfCancelMax = m_pfClose;
        pfCancelMin = m_pfClose;
        break;
		
    default:
        pfCancelMax = m_pfClose;
        pfCancelMin = m_pfClose;
        break;
    }
        
    long nFirstSetupBar;
    for (nFirstSetupBar = m_nSetupLookback + 1; nFirstSetupBar < m_nNumPoints; nFirstSetupBar++)
    {
        int nLastSetupBar;
        BOOL bSellSetup;
                
        long nFirstBuySetupPriceFlip = GetBarOfPriceFlip(m_nSetupPrice, nFirstSetupBar, m_nNumPoints - 1, m_nSetupLookback, FALSE);
        long nFirstSellSetupPriceFlip = GetBarOfPriceFlip(m_nSetupPrice, nFirstSetupBar, m_nNumPoints - 1, m_nSetupLookback, TRUE);
                
        nFirstSetupBar = min(nFirstBuySetupPriceFlip, nFirstSellSetupPriceFlip);
                
        //When find a Price Flip pattern, we start looking for a complete Sell/Buy Setup
        if (nFirstSellSetupPriceFlip < nFirstBuySetupPriceFlip)
        {
            bSellSetup = TRUE;
            for (nLastSetupBar = nFirstSetupBar + 1; nLastSetupBar < m_nNumPoints; nLastSetupBar++)
            {
                float   fLookback = GetComparePrice(m_nSetupPrice, bSellSetup, nLastSetupBar - m_nSetupLookback);
                float   fToday    = GetComparePrice(m_nSetupPrice, bSellSetup, nLastSetupBar);
                                
                if (IsInvalid(fLookback) || IsInvalid(fToday) || fToday <= fLookback)
                    break;
            }
        }
        else if (nFirstBuySetupPriceFlip < nFirstSellSetupPriceFlip)
        {
            bSellSetup = FALSE;
            for (nLastSetupBar = nFirstSetupBar + 1; nLastSetupBar < m_nNumPoints; nLastSetupBar++)
            {
                float   fLookback = GetComparePrice(m_nSetupPrice, bSellSetup, nLastSetupBar - m_nSetupLookback);
                float   fToday    = GetComparePrice(m_nSetupPrice, bSellSetup, nLastSetupBar);
                                
                if (IsInvalid(fLookback) || IsInvalid(fToday) || fToday >= fLookback)
                    break;
            }
        }
        else
            continue;
                
        //Setup Range from nFirstSetupBar to nLastSetupBar - 1
        if (nLastSetupBar != m_nNumPoints && nLastSetupBar - nFirstSetupBar < m_nMinSetupBars)
            continue;
        
        CBLPPrice fMaxCancel, fMinCancel;
        long nMax, nMin;
        long nLast = min(nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);
        
        nMax = GetBarOfMax(pfCancelMax, nFirstSetupBar, nLast);
        ASSERT(nMax >= 0);
        fMaxCancel = pfCancelMax[nMax];
        
        nMin = GetBarOfMin(pfCancelMin, nFirstSetupBar, nLast);
        ASSERT(nMin >= 0);
        fMinCancel = pfCancelMin[nMin];
        
        int nIntersectionBar = nFirstSetupBar + m_nStartIntersection;
        BOOL bCancel;
                
        // 1. Incomplete Setup will be Cancelled
        // 2. Check for Cancelled Setup because of Intersection Rule
        //    It is default to noIntersection at the BigSide
        // Intersection Criteria - adjust the bar on which the countdown starts
        // Currently, we always send noIntersection, which means this part of
        // code does not do anything. But according to John's suggestion, we
        // leave it here.
        if (nLastSetupBar != m_nNumPoints)
        {
            if (m_nIntersectionRule == noIntersection)
            {
                nIntersectionBar = nFirstSetupBar + m_nStartIntersection;
                bCancel = FALSE;
            }
            else
            {
                for (nIntersectionBar = nFirstSetupBar + m_nStartIntersection - 1; nIntersectionBar < m_nNumPoints; nIntersectionBar++)
                {
                    if (IsInvalid(m_pfClose[nIntersectionBar]))
                        continue;
                                        
                    long nBar;
                    if (bSellSetup) {
                        if (bCancel = TestCancelCriteria(pGraph, m_nCancelRule, bSellSetup, nIntersectionBar, fMinCancel))
                            break;
                                                
                        for (nBar = nIntersectionBar - 3; nBar >= nFirstSetupBar; nBar--)
                        {
                            if ((CBLPPrice)m_pfLow[nIntersectionBar] <= m_pfHigh[nBar])
                                break;
                        }
                                                
                        if (nBar >= nFirstSetupBar)
                            break;
                    }
                    else {
                        if (bCancel = TestCancelCriteria(pGraph, m_nCancelRule, bSellSetup, nIntersectionBar, fMaxCancel))
                            break;
                                                
                        for (nBar = nIntersectionBar - 3; nBar >= nFirstSetupBar; nBar--)
                        {
                            if ((CBLPPrice)m_pfHigh[nIntersectionBar] >= m_pfLow[nBar])
                                break;
                        }
                                                
                        if (nBar >= nFirstSetupBar)
                            break;
                    }
                }
            }
            
            if (bCancel || ((m_nIntersectionRule == during) && (nIntersectionBar >= nLastSetupBar) && (nLastSetupBar < m_nNumPoints - 1)))
            {
                nFirstSetupBar = nLastSetupBar;
                continue;
            }
        }
                
        //Once a setup is complete(the last setup can be incomplete), start countdown
        XDeMarkCountdown *pNewCountdown;
        if (nLastSetupBar - nFirstSetupBar < m_nMinSetupBars)
        {
            //The last setup which is still undergoing but have not reached the minimum setup yet.
            //Create a Countdown as this setup is incomplete
            //You can not do pNewCountdown = &newCountdown, as vector stores a copy of XDeMarkCountdown.
            ASSERT(nLastSetupBar == m_nNumPoints);
            XDeMarkCountdown newCountdown;
            newCountdown.m_bSellCountdown = bSellSetup;
            m_vDeMarkCountdowns.push_back(newCountdown);
            pNewCountdown = &m_vDeMarkCountdowns.back();
            pNewCountdown->m_bCompletedSetup = FALSE;
        }
        else
        {
            pNewCountdown = DoCountdown(pGraph, bSellSetup, nIntersectionBar, fMinCancel, fMaxCancel);
            pNewCountdown->m_bCompletedSetup = TRUE;
        }
        
        pNewCountdown->m_nFirstSetupBar = nFirstSetupBar;
        pNewCountdown->m_nLastSetupBar = nLastSetupBar - 1;
                
        //If we reach the end of bars, no need to go further to look for FirstSetupBar
        if ((nLastSetupBar == m_nNumPoints))
            break;
                
        //As we will do ++ automatically, so we start from nLastSetupBar - 1
        nFirstSetupBar = nLastSetupBar - 1;
    }
        
        
    /////////////////////////////////////////////////////////////////
    //
    //Go over all the Countdowns to decide Recycle, SetupWithinSetup
    //
    /////////////////////////////////////////////////////////////////
    BLPVector<XDeMarkCountdown>::iterator pCountdown;
	for (pCountdown = m_vDeMarkCountdowns.begin(); pCountdown != m_vDeMarkCountdowns.end(); pCountdown++)
    {
		
		pCountdown->m_bDisplayR = FALSE;       
        //A Complete Setup
        if (pCountdown->m_bCompletedSetup)
        {
            long nLastCountdownBar = pCountdown->m_vnCountdownBars.empty() ? m_nNumPoints : pCountdown->m_vnCountdownBars.back();
    
            if ((pCountdown->m_vnCountdownBars.size() < m_nMinCountdownBars) || (nLastCountdownBar < 0))
                nLastCountdownBar = m_nNumPoints;
                        
            // Recycling
            BOOL bRecycle = FALSE;
			BOOL bRecycleR = FALSE;
            BLPVector<XDeMarkCountdown>::iterator pNextCountdown = pCountdown + 1;

            if(pNextCountdown != m_vDeMarkCountdowns.end() && pNextCountdown->m_bCompletedSetup)
            {
                if (pCountdown->m_bSellCountdown != pNextCountdown->m_bSellCountdown)
                {
                    if (nLastCountdownBar >= (pNextCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1))
					{
						if(m_strButtonText == "TD Combo")
						{
							if(m_bRevCancelCombo == TRUE)
							{
								bRecycle = TRUE;
							}
						}
						else if(m_bReverseSetupCancel == TRUE)
						{
							bRecycle = TRUE;
						}
					}
                }
                else 
                {
                    CBLPPrice  fPriorSpread = pCountdown->m_fMaxSetup - pCountdown->m_fMinSetup;
                    CBLPPrice  fNextSpread = pNextCountdown->m_fMaxSetup - pNextCountdown->m_fMinSetup;

                    BOOL bSetupWithinSetup = SetupWithinSetup(&pNextCountdown[0]);

                    if (!bSetupWithinSetup)
                    {
                        
                        switch(m_nRecycleRule)
                        {
                        case IgnoreNone:
                            if (m_bRecycleCount)
                            {
								// We also consider recycle if a setup's countdown overlaps another's setup.
								while(!bRecycle && pNextCountdown != m_vDeMarkCountdowns.end() &&
									nLastCountdownBar >= pNextCountdown->m_nFirstSetupBar &&
									pCountdown->m_bSellCountdown == pNextCountdown->m_bSellCountdown)
								{
									if (pNextCountdown->m_nLastSetupBar - pNextCountdown->m_nFirstSetupBar >= m_nRecycleCount - 1)
									{
										long nRecycleBar = pNextCountdown->m_nFirstSetupBar + m_nRecycleCount - 1;
    
										if (m_bRecycleConfirm)
										{
											if (nRecycleBar < m_nNumPoints - 1)
											{
												if (pNextCountdown->m_bSellCountdown)
													bRecycle = m_pfOpen[nRecycleBar + 1] > m_pfClose[nRecycleBar - 4];
												else
													bRecycle = m_pfOpen[nRecycleBar + 1] < m_pfClose[nRecycleBar - 4];
											}
											else
												bRecycle = FALSE;
										}
										else
											bRecycle = TRUE;
									}
									
									if(!bRecycle)
										++pNextCountdown;
								}

								bRecycleR = bRecycle;

                            }else
                                bRecycle = FALSE;
                            break;
                        case IgnoreSmallAndLarge:
                            bRecycle = (fPriorSpread * m_fSmallRecycleLevel < fNextSpread) && (fPriorSpread * m_fLargeRecycleLevel > fNextSpread);
                            break;
                        case IgnoreCurrent:
                            if (pNextCountdown + 1 != m_vDeMarkCountdowns.end())
                                bRecycle = TRUE;
                            break;
                        case IgnoreAll:
                            bRecycle = FALSE;
                            break;
                        }
                    }
                    

                    for (; pNextCountdown != m_vDeMarkCountdowns.end() && nLastCountdownBar >= (pNextCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1); ++pNextCountdown)
                    {
                        if (pCountdown->m_bSellCountdown != pNextCountdown->m_bSellCountdown
                            && nLastCountdownBar >= (pNextCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1))
						{
							if(m_strButtonText == "TD Combo")
							{
								if(m_bRevCancelCombo == TRUE)
								{
									bRecycle = TRUE;
									bRecycleR= FALSE;
								}
							}
							else if(m_bReverseSetupCancel == TRUE)
							{
								bRecycle = TRUE;
								bRecycleR= FALSE;
							}
						}
                    }                                       
                }
                if (bRecycle)
                {
					pCountdown->m_vnCountdownBars.clear();
					if (bRecycleR && pCountdown->m_bCompletedCountdown == TRUE){
						pCountdown->m_bDisplayR = TRUE;
						pCountdown->m_vnCountdownBars.push_back(pCountdown->m_nCompleteCountdownBar);
					}
					pCountdown->m_bCompletedCountdown = FALSE;
                }
            }
        }
    }

        
    if (m_bShowFullCountdown && !m_vDeMarkCountdowns.empty())
    {
        for (pCountdown = m_vDeMarkCountdowns.begin(); pCountdown != m_vDeMarkCountdowns.end();)
        {
            if (pCountdown->m_bCompletedCountdown)
            {
                BLPVector<XDeMarkCountdown>::iterator pNextCountdown = pCountdown + 1;
                long nLastCountdownBar;
                long nBar;
                
                while ((pNextCountdown != m_vDeMarkCountdowns.end()) &&
                       (pNextCountdown->m_bSellCountdown == pCountdown->m_bSellCountdown) &&
                       !pNextCountdown->m_bCompletedCountdown)
                    ++pNextCountdown;

                if (pNextCountdown == m_vDeMarkCountdowns.end())
                {
                    nLastCountdownBar = m_nNumPoints - 1;
                }
                else if (pNextCountdown->m_bSellCountdown == pCountdown->m_bSellCountdown)
                {
                    nLastCountdownBar = pNextCountdown->m_vnCountdownBars.front();
                }
                else
                {
                    nLastCountdownBar = pNextCountdown->m_nFirstSetupBar;
                }

                for (nBar = pCountdown->m_nCompleteCountdownBar + 1; nBar < nLastCountdownBar; ++nBar)
                {
                    if (TestCountdownRule(pGraph, nBar, pCountdown->m_bSellCountdown, pCountdown->m_vnCountdownBars.size(), m_nCountdownPrice))
                    {
                        pCountdown->m_vpriceCountdown[nBar] = pCountdown->m_bSellCountdown ? m_pfHigh[nBar] : m_pfLow[nBar];
                        pCountdown->m_vnCountdownBars.push_back(nBar);
                    }
                }
                pCountdown = pNextCountdown;
            }
            else
                ++pCountdown;
        }
    }
    
    

    //Calcluate the offset
    long  nOffsetMax = GetBarOfMax(m_pfHigh, 0 , m_nNumPoints - 1);
    float fOffsetMax = m_pfHigh[nOffsetMax];
    long  nOffsetMin = GetBarOfMin(m_pfLow, 0, m_nNumPoints - 1);
    float fOffsetMin = m_pfLow[nOffsetMin];
    float fOffset = (fOffsetMax - fOffsetMin) * 0.03;
    
    BLPVector<float> pfSellSetup(m_nNumPoints, HOLEF);
    BLPVector<float> pfBuySetup(m_nNumPoints, HOLEF);
    BLPVector<float> pfUpShade(m_nNumPoints, HOLEF);
    BLPVector<float> pfDownShade(m_nNumPoints, HOLEF);
    BLPVector<float> pfSellCountdown(m_nNumPoints, HOLEF);
    BLPVector<float> pfBuyCountdown(m_nNumPoints, HOLEF);
    BLPVector<float> pfRaisedSellCountdown(m_nNumPoints, HOLEF);
    BLPVector<float> pfDroppedBuyCountdown(m_nNumPoints, HOLEF);
    BLPVector<float> pfSellArrows(m_nNumPoints, HOLEF);
    BLPVector<float> pfBuyArrows(m_nNumPoints, HOLEF);
    
    //fill up the DataLines
    for (pCountdown = m_vDeMarkCountdowns.begin(); pCountdown != m_vDeMarkCountdowns.end(); pCountdown++)
    {
        
		//Setup
        if (m_bShowSetup)
        {
            long nBar;
			//Modified by Arun for TREQ 36665. This enables setup to be displayed to the specified range. 
            long nLast; 
			if(m_nComboVersion >=3)
			{
				if(m_strButtonText == "TD Combo")
				{
					switch(m_nShowSetupToCombo)
					{
						case 2: 
							nLast = min(pCountdown->m_nFirstSetupBar + m_nRecycleCount -1, pCountdown->m_nLastSetupBar);
							break;
						case 3:
							nLast = pCountdown->m_nLastSetupBar;
							break;
						default:
							nLast = min(pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);
							break;	
					}
				}
				else 
				{
					switch(m_nShowSetupTo)
					{
						case 2:
							nLast = min(pCountdown->m_nFirstSetupBar + m_nRecycleCount -1, pCountdown->m_nLastSetupBar);
							break;
						case 3:
							nLast = pCountdown->m_nLastSetupBar;
							break;
						default:
							nLast = min(pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);
							break;	
					}

				}
			}
			else 
				nLast = (m_bShowFullSetup) ?
                pCountdown->m_nLastSetupBar : min(pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);

            if (pCountdown->m_bSellCountdown)
            {
                for (nBar = pCountdown->m_nFirstSetupBar; nBar <= nLast; ++nBar)
					if (!IsInvalid(m_pfHigh[nBar]))
						pfSellSetup[nBar] = m_pfHigh[nBar];
					else
						pfSellSetup[nBar] = GetComparePrice(m_nSetupPrice, pCountdown->m_bSellCountdown, nBar);
            }
            else
            {
                for (nBar = pCountdown->m_nFirstSetupBar; nBar <= nLast; ++nBar)
					if (!IsInvalid(m_pfLow[nBar]))
						pfBuySetup[nBar] = m_pfLow[nBar];
					else
						pfBuySetup[nBar] = GetComparePrice(m_nSetupPrice, pCountdown->m_bSellCountdown, nBar);
            }
        }
                
        //Setup Qualification Arrows
        if (m_bShowSetupQualifier)
        {
            if (pCountdown->m_bCompletedSetup)
            {
                long nFirst = pCountdown->m_nFirstSetupBar + m_nMinSetupBars -1 ;
                //We would at most look for another 5 bars
                long nLast = min(nFirst + 5, m_nNumPoints - 1);
                long nDay2 = nFirst - 2;
                long nDay3 = nFirst - 3;
                
                BOOL bQualifier = FALSE;
                                
                while (nFirst <= nLast && !bQualifier)
                {
                    if (pCountdown->m_bSellCountdown && (max(m_pfHigh[nFirst], m_pfHigh[nFirst - 1]) > max(m_pfHigh[nDay2], m_pfHigh[nDay3])))
                    {
                        bQualifier = TRUE;
                        ASSERT(!IsInvalid(m_pfHigh[nFirst]));
						if (!IsInvalid(m_pfHigh[nFirst]))
							pfSellArrows[nFirst] = m_pfHigh[nFirst] + fOffset;
						else
							pfSellArrows[nFirst] = GetComparePrice(m_nSetupPrice, pCountdown->m_bSellCountdown, nFirst) + fOffset;
                    }
                    else if (!pCountdown->m_bSellCountdown && (min(m_pfLow[nFirst], m_pfLow[nFirst - 1]) < min(m_pfLow[nDay2], m_pfLow[nDay3])))
                    {
                        bQualifier = TRUE;
                        ASSERT(!IsInvalid(m_pfLow[nFirst]));
						if (!IsInvalid(m_pfLow[nFirst]))
							pfBuyArrows[nFirst] = m_pfLow[nFirst] - fOffset;
						else
							pfBuyArrows[nFirst] = GetComparePrice(m_nSetupPrice, pCountdown->m_bSellCountdown, nFirst) - fOffset;
                    }
                                        
                    ++nFirst;
                }
            }
        }
        
        //Shading
        if (m_bShowShade)
        {
            if (pCountdown->m_bCompletedSetup)
            {
                long nMax, nMin;
                nMax = GetBarOfMax(m_pfTrueHigh, pCountdown->m_nFirstSetupBar, pCountdown->m_nLastSetupBar);
                ASSERT(nMax >= 0);
                nMin = GetBarOfMin(m_pfTrueLow, pCountdown->m_nFirstSetupBar, pCountdown->m_nLastSetupBar);
                ASSERT(nMin >= 0);
                                
                if (pCountdown->m_bSellCountdown)
                {
                    pfUpShade[pCountdown->m_nFirstSetupBar] = m_pfTrueHigh[nMax];
                    pfUpShade[pCountdown->m_nLastSetupBar] = m_pfTrueLow[nMin];
                }
                else
                {
                    pfDownShade[pCountdown->m_nFirstSetupBar] = m_pfTrueHigh[nMax];
                    pfDownShade[pCountdown->m_nLastSetupBar] = m_pfTrueLow[nMin];
                }
            }
        }
                
        //Countdown
        if (m_bShowCountdown)
        {
            if (!pCountdown->m_vnCountdownBars.empty())
            {
                BLPVector<long>::iterator pBar;
                long nBar;
                short nCount = 0;
                ASSERT(pCountdown->m_vnCountdownBars.begin() <= pCountdown->m_vnCountdownBars.end());
                for (pBar = pCountdown->m_vnCountdownBars.begin(); pBar != pCountdown->m_vnCountdownBars.end(); pBar++)
                {
                    BOOL bDisqualified = FALSE;
                    nBar = *pBar;
					
                    if (nBar < 0) {
                        bDisqualified = TRUE;
                        nBar = -nBar;
                    }
                                                            
					else if (nBar == 0)
                        break; 
                    else
                        nCount++;

                    if (m_pnCountdownDisplay[nBar] == 0)
                    {
                        //Modified by Arun for TREQ 36665. This displays an 'R' symbol when a recycle occurs. 
						if(bDisqualified == TRUE)
						{
							m_pnCountdownDisplay[nBar] = -1;
						}
						else
						{
							if(pCountdown->m_bDisplayR == TRUE)
							{
								m_pnCountdownDisplay[nBar] = -('R');
								
							}
							else
							{
								
								m_pnCountdownDisplay[nBar] = nCount;

							}
						}

                        if (pCountdown->m_bSellCountdown)
                            pfSellCountdown[nBar] = pCountdown->m_vpriceCountdown[nBar];
                        else
                            pfBuyCountdown[nBar] = pCountdown->m_vpriceCountdown[nBar];
                    }
                }
            }
                        
            //Raise or Drop to avoid setup and countdown overlaps
            long nSetupBar;
            long nLastRaisedBar = m_bShowFullSetup ? pCountdown->m_nLastSetupBar : pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1;
            for (nSetupBar = pCountdown->m_nFirstSetupBar; nSetupBar <= nLastRaisedBar; nSetupBar++)
            {
                if (nSetupBar >= m_nNumPoints)
                    break;
                                
                if (pCountdown->m_bSellCountdown)
                {
                    pfRaisedSellCountdown[nSetupBar] = pfSellCountdown[nSetupBar];
                    pfSellCountdown[nSetupBar] = HOLEF;
                }
                else
                {
                    pfDroppedBuyCountdown[nSetupBar] = pfBuyCountdown[nSetupBar];
                    pfBuyCountdown[nSetupBar] = HOLEF;
                }
            }
        }
                
                
        //Countdown Completion Arrows
        if(m_bShowCountdownCompletion && pCountdown->m_bCompletedCountdown)
        {
            long nArrowBar = pCountdown->m_nCompleteCountdownBar;
                        
            if (pCountdown->m_bSellCountdown)
                pfSellArrows[nArrowBar] = m_pfHigh[nArrowBar] + fOffset;
            else
                pfBuyArrows[nArrowBar] = m_pfLow[nArrowBar] - fOffset;
        }
                
        //TDST Line
        if ((m_bShowTDST) && (pCountdown->m_bCompletedSetup))
            DrawTDSTLine(&pCountdown[0], pGraph);
                
        //Stop Loss Level
        if ((m_bShowStopLossLevel) && (pCountdown->m_bCompletedCountdown))
            DrawStopLossLevel(&pCountdown[0], pGraph);
                
        //Recycle Level
        if (m_nRecycleRule ==IgnoreSmallAndLarge &&
            pCountdown->m_nLastSetupBar == m_nNumPoints - 1 &&
            pCountdown->m_nLastSetupBar - pCountdown->m_nFirstSetupBar <= m_nMinSetupBars - 1 &&
            pCountdown != m_vDeMarkCountdowns.begin())
            DrawRecycleLevel(&pCountdown[0], pGraph);
    }
    
    SetStudyDataLine(0, &pfSellSetup[0]);
    SetStudyDataLine(1, &pfBuySetup[0]);
    SetStudyDataLine(2, &pfUpShade[0]);
    SetStudyDataLine(3, &pfDownShade[0]);
    SetStudyDataLine(4, &pfSellCountdown[0]);
    SetStudyDataLine(5, &pfBuyCountdown[0]);
    SetStudyDataLine(6, &pfRaisedSellCountdown[0]);
    SetStudyDataLine(7, &pfDroppedBuyCountdown[0]);
    SetStudyDataLine(8, &pfSellArrows[0]);
    SetStudyDataLine(9, &pfBuyArrows[0]);
        
    return NOERROR;
}

//This function will test the cancel criteria for a setup.
//During the countdown, if the testcanelcriteria is true, the m_vCountdownBars will be cleared. Read DoCountdown for more detail
BOOL CTDSequential::TestCancelCriteria(IBLPGraphData *pGraph, long nRule, BOOL bSellSetup, int nIndex, float fComparePrice)
{
    float *pfPriceLine;
    if (bSellSetup) {
        switch (nRule) {
        case LowVsHigh:
        case LowVsTrueHigh:
            pfPriceLine = m_pfHigh;
            break;
                        
        case HighVsHigh:
        case HighVsTrueHigh:
        case HighVsClose:
            pfPriceLine = m_pfLow;
            break;
                        
        case CloseVsHigh:
        case CloseVsTrueHigh:
            pfPriceLine = m_pfClose;
            break;
                        
        case TrueLowVsTrueHigh:
            pfPriceLine = m_pfTrueHigh;
            break;
                        
        default:
            return FALSE;
        }
                
        return ((CBLPPrice)pfPriceLine[nIndex] < fComparePrice);
    }
    else {
        switch (nRule) {
        case LowVsHigh:
        case LowVsTrueHigh:
            pfPriceLine = m_pfLow;
            break;
                        
        case HighVsHigh:
        case HighVsTrueHigh:
        case HighVsClose:
            pfPriceLine = m_pfHigh;
            break;
                        
        case CloseVsHigh:
        case CloseVsTrueHigh:
            pfPriceLine = m_pfClose;
            break;
                        
        case TrueLowVsTrueHigh:
            pfPriceLine = m_pfTrueLow;
            break;
                        
        default:
            return FALSE;
        }
                
        return ((CBLPPrice)pfPriceLine[nIndex] > fComparePrice);
    }
}

//This function will get Setup or Countdown Price on bar nIndex based on nPrice and the type of setup or countdown.
float CTDSequential::GetComparePrice(enumPriceType nPrice, BOOL bSellSetup, int nIndex)
{
    float fPrice;
    //{Open = 0, High = 1, Low = 2, Close = 3, TrueHigh = 4, TrueLow = 5, Midpoint = 6, HLCAverage = 7,
    // LowHigh = 8, HighLow = 9, TrueLowTrueHigh = 10, TrueHighTrueLow = 11}
    switch (nPrice)
    {
    case High :
        fPrice = m_pfHigh[nIndex];
        break;
    case Low :
        fPrice = m_pfLow[nIndex];
        break;
    case Close :
        fPrice = m_pfClose[nIndex];
        break;
    case Open:
        fPrice = m_pfOpen[nIndex];
        break;
    case TrueHigh:
        fPrice = m_pfTrueHigh[nIndex];
        break;
    case TrueLow:
        fPrice = m_pfTrueLow[nIndex];
        break;
    case HighLow:
        if (bSellSetup)
            fPrice = m_pfLow[nIndex];
        else
            fPrice = m_pfHigh[nIndex];
        break;
    case LowHigh:
        if (bSellSetup)
            fPrice = m_pfHigh[nIndex];
        else
            fPrice = m_pfLow[nIndex];
        break;
    case TrueLowTrueHigh:
        if (bSellSetup)
            fPrice = m_pfTrueHigh[nIndex];
        else
            fPrice = m_pfTrueLow[nIndex];
        break;
    case TrueHighTrueLow:
        if (bSellSetup)
            fPrice = m_pfTrueLow[nIndex];
        else
            fPrice = m_pfTrueHigh[nIndex];
        break;
    case Midpoint:
        if (IsInvalid(m_pfHigh[nIndex]) || IsInvalid(m_pfLow[nIndex]))
            fPrice = HOLEF;
        else
            fPrice = (m_pfHigh[nIndex] + m_pfLow[nIndex]) / 2;
        break;
    case HLCAverage:
        if (IsInvalid(m_pfHigh[nIndex]) || IsInvalid(m_pfLow[nIndex]) || IsInvalid(m_pfClose[nIndex]))
            fPrice = HOLEF;
        else
            fPrice = (m_pfHigh[nIndex] + m_pfLow[nIndex] + m_pfClose[nIndex]) /3;
        break;
    default:
        fPrice = HOLEF;
        break;
    }
    return fPrice;
}


//The function will do countdown start on nCountdownStart
//It will test each bar for Cancel Criteria until it finishes the countdown
//For the last countdown bar, it will test the termination count critera.
//It will also test the qualification for the last countdown bar.
//If fail, it will put a negative value in the m_vnCountdownBars.
//No matter what, it will create a XDeMarkCountdown, and push it
//to the m_vDeMarkCountdowns, and return the address of the instance in
//the vector.
//For the m_nFirstSetupBar, and m_nLastSetupBar, they are not needed in this
//function, and thus can be initliazed after the instance is created by this
//function, and should be initliazed after this function call.
#if (!defined __SUNPRO_CC) || (__SUNPRO_CC > 0x420)
CTDSequential::
#endif
XDeMarkCountdown* CTDSequential::DoCountdown(IBLPGraphData *pGraph, BOOL bSellCountdown, int nCountdownStart, float fMinSetup, float fMaxSetup)
{
    XDeMarkCountdown xCountdown(m_nNumPoints, bSellCountdown, fMinSetup, fMaxSetup);
        
    long nBar;
    for (nBar = nCountdownStart; nBar < m_nNumPoints; nBar++)
    {
        //Check for valid data for not
        if (IsInvalid(m_pfOpen[nBar], m_pfHigh[nBar], m_pfLow[nBar], m_pfClose[nBar]))
            continue;
        
        //Test to see if should we cancel the setup or not
        BOOL bCancel = bSellCountdown ?
            TestCancelCriteria(pGraph, m_nCancelRule, TRUE, nBar, fMinSetup):
            TestCancelCriteria(pGraph, m_nCancelRule, FALSE, nBar, fMaxSetup);
			
        if (bCancel)
        {
            xCountdown.m_vnCountdownBars.clear();
            break;
        }
                
        //The Last Countdown Bar, we needs some other condition to verify it
        //Otherwise, we just need regular TestCountdownRule to verify
        if (xCountdown.m_vnCountdownBars.size() >= m_nMinCountdownBars - 1)
        {
            //Test Termination Count
            if (TestTerminationCount(pGraph, nBar, bSellCountdown, xCountdown.m_vnCountdownBars.size()))
            {
                xCountdown.m_vpriceCountdown[nBar] = bSellCountdown ? m_pfHigh[nBar] : m_pfLow[nBar];
                                
                //Test Last Countdown bar qualifier. If qualify, finish countdown
                //Otherwise, push negative number as a sign
                //Countdown Qualifier only apply for Sequential. For Combo, it should be always be FALSE
                if (!m_bLastCountdownQualifier || LastCountdownQualifier(pGraph, bSellCountdown, nBar, xCountdown.m_vnCountdownBars))
                {
                    xCountdown.m_vnCountdownBars.push_back(nBar);
                    xCountdown.m_bCompletedCountdown = TRUE;
                    xCountdown.m_nCompleteCountdownBar = nBar;
                    break;
                }
                else
                {
                    xCountdown.m_vnCountdownBars.push_back(-nBar);
                }
            }
        }
        else
        {
            if (TestCountdownRule(pGraph, nBar, bSellCountdown, xCountdown.m_vnCountdownBars.size(), m_nCountdownPrice))
            {
                xCountdown.m_vpriceCountdown[nBar] = bSellCountdown ? m_pfHigh[nBar] : m_pfLow[nBar];
                xCountdown.m_vnCountdownBars.push_back(nBar);
            }
        }
    }
        
    m_vDeMarkCountdowns.push_back(xCountdown);
    ASSERT(m_vDeMarkCountdowns.begin() <= m_vDeMarkCountdowns.end() - 1);
    return &m_vDeMarkCountdowns.back();
}


//Test termination count condition. nCount is dummy here in Sequential. But it would be used in TD Combo.
BOOL CTDSequential::TestTerminationCount(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount)
{
    CTDStudy::enumPriceType nTerminationPrice = m_nTerminationCountPrice;
        
    if (bSellCountdown)
    {
        //Either is greater or equal than the high n bar ago
        float fTerminationPrice = max(GetComparePrice(m_nCountdownPrice, bSellCountdown, nBar), GetComparePrice(nTerminationPrice, bSellCountdown, nBar));
        return (CBLPPrice)fTerminationPrice >= m_pfHigh[nBar - m_nCountdownLookback];
    }
    else
    {
        //Either is smaller or equal than the low n bar ago
		if (m_nTerminationCountPrice == High)
			nTerminationPrice = Low;
        float fTerminationPrice = min(GetComparePrice(m_nCountdownPrice, bSellCountdown, nBar), GetComparePrice(nTerminationPrice, bSellCountdown, nBar));
        return (CBLPPrice)fTerminationPrice <= m_pfLow[nBar - m_nCountdownLookback];
    }
}


BOOL CTDSequential::TestCountdownRule(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount, CTDStudy::enumPriceType nComparePrice)
{
    if (bSellCountdown) {
        return (CBLPPrice)GetComparePrice(nComparePrice, bSellCountdown, nBar) >= m_pfHigh[nBar - m_nCountdownLookback];
    }
    else {
        return (CBLPPrice)GetComparePrice(nComparePrice, bSellCountdown, nBar) <= m_pfLow[nBar - m_nCountdownLookback];
    }
}

//This only applies to Sequential. It does not apply to Combo
BOOL CTDSequential::LastCountdownQualifier(IBLPGraphData *pGraph, BOOL bSellCountdown, int nCurrentBar, BLPVector<long> m_vnCountdownBars)
{
    ASSERT(m_vnCountdownBars.size() >= m_nMinCountdownBars - 1);
    
    long *nBar, nCount = 0;
    nBar = &m_vnCountdownBars.back();
    //Skip bar with +, go to Bar 12
    while (*nBar < 0)
        --nBar;
        
    //Go back another 4 bars    
    nBar = nBar - 4;
        
    BOOL bQualifier = FALSE;
    if (bSellCountdown)
        bQualifier = (m_pfHigh[nCurrentBar] >= m_pfClose[*nBar]);
    else
        bQualifier = (m_pfLow[nCurrentBar] <= m_pfClose[*nBar]);
        
    return bQualifier;
}

/*
 * Additional Functions.
 * Added by Yusong Li
 */

int CTDSequential::DrawStopLossLevel(XDeMarkCountdown* pCountdown, IBLPGraphData* pGraph)
{
/*
 * First calculate the true range.
 * nMax: the Bar of Highest TrueHigh or Lowest TrueLow during the Countdown
 */
    BLPVector<long>::iterator pBar;
    long nMax;
    float fTrueRange, fLevel;
        
        
    ASSERT(pCountdown->m_vnCountdownBars.begin() <= pCountdown->m_vnCountdownBars.end());
    if (m_nStopLossLevelOption == NumberedCountdownBars)
    {
        float fMax;
        nMax = pCountdown->m_vnCountdownBars.front();
        ASSERT(nMax >= 0);
        fMax = (pCountdown->m_bSellCountdown) ? m_pfTrueHigh[nMax] : m_pfTrueLow[nMax];
		pBar = pCountdown->m_vnCountdownBars.begin();
		if (&pBar[0] == NULL)
			return -1;

        for (; pBar != pCountdown->m_vnCountdownBars.end() && *pBar <= pCountdown->m_nCompleteCountdownBar; pBar++)
        {
            /*
             * We do not include the + bar
             */
            if (*pBar <= 0)
                continue;
            
            if (pCountdown->m_bSellCountdown)
            {
                nMax = (fMax > m_pfTrueHigh[*pBar]) ? nMax : *pBar;
                fMax = m_pfTrueHigh[nMax];
            }
            else
            {
                nMax = (fMax < m_pfTrueLow[*pBar]) ? nMax : *pBar;
                fMax = m_pfTrueLow[nMax];       
            }
        }
    }
    else
    {
        if (pCountdown->m_bSellCountdown)
            nMax = GetBarOfMax(m_pfTrueHigh, pCountdown->m_vnCountdownBars.front(), pCountdown->m_nCompleteCountdownBar);
        else
            nMax = GetBarOfMin(m_pfTrueLow,  pCountdown->m_vnCountdownBars.front(), pCountdown->m_nCompleteCountdownBar);
    }
    
    fTrueRange = m_pfTrueHigh[nMax] - m_pfTrueLow[nMax];
    
    if (pCountdown->m_bSellCountdown)
        fLevel = m_pfHigh[nMax] + fTrueRange;
    else
        fLevel = m_pfLow[nMax] - fTrueRange;
        
    
    long nBar13 = pCountdown->m_nCompleteCountdownBar;
    long nLastBar, nBar;
    XDeMarkCountdown* pFirstReverseCountdown = GetFirstReverseCountdown(pCountdown);
    if (pFirstReverseCountdown == NULL)
        nLastBar = m_nNumPoints - 1;
    else
        nLastBar = min(pFirstReverseCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);
        
    for (nBar = nBar13; nBar < nLastBar; ++nBar)
    {
        if (pCountdown->m_bSellCountdown                &&
            (m_pfClose[nBar]    > fLevel)               &&
            (m_pfOpen[nBar + 1] > m_pfClose[nBar])      &&
            (m_pfHigh[nBar + 1] > m_pfOpen[nBar]))
            break;
        
        if (!pCountdown->m_bSellCountdown               &&
            (m_pfClose[nBar]    < fLevel)               &&
            (m_pfOpen[nBar + 1] < m_pfClose[nBar])      &&
            (m_pfLow[nBar + 1]  < m_pfOpen[nBar + 1]))
            break;
    }
        
    ChartLineParameters pStopLossParms[] = {1, XRT_LPAT_LONG_DASH, blgMagenta};
    pGraph->AddStudyTrendLine(nBar13, fLevel, nBar, fLevel, pStopLossParms, line_type_segment, &(m_pTrendLineIds[m_nTrendLineIndex++]));
    
    return 0;
}

int CTDSequential::DrawTDSTLine(XDeMarkCountdown* pCountdown, IBLPGraphData *pGraph)
{
    long  nMax;
    float fMax;
        
    long nLastBar = min(pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1, m_nNumPoints - 1);
        
    if (!pCountdown->m_bSellCountdown)
    {
        nMax = GetBarOfMax(m_pfTrueHigh, pCountdown->m_nFirstSetupBar, nLastBar);
        ASSERT(nMax >= 0);
        fMax = m_pfTrueHigh[nMax];
    }
    else
    {
        nMax = GetBarOfMin(m_pfTrueLow, pCountdown->m_nFirstSetupBar, nLastBar);
        ASSERT(nMax >= 0);
        fMax = m_pfTrueLow[nMax];
    }
    
    XDeMarkCountdown* pFirstSimilarCountdown = GetFirstSimilarCountdown(pCountdown);
    if (pFirstSimilarCountdown == NULL)
        nLastBar = m_nNumPoints - 1;
    else
        nLastBar = pFirstSimilarCountdown->m_nFirstSetupBar;
        
    long nBar;
    ASSERT(pFirstSimilarCountdown == NULL || pFirstSimilarCountdown->m_bCompletedSetup);
        
    BOOL bTDSTExtendable = FALSE;
        
    if (m_bExtendTDST)
    {
        //n is the number of setup should be excluded as we will not include incomplete setup
        long n;
        if (m_vDeMarkCountdowns.back().m_bCompletedSetup)
            n = 0;
        else
            n = 1;
                
        if ((pCountdown == &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size() - 2 - n]) && (pFirstSimilarCountdown == &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size() - 1 - n]))
            bTDSTExtendable = TRUE;
    }
    
    
        
    if (bTDSTExtendable)
    {
        nBar = m_nNumPoints - 1;
    }
    else
    {
        for (nBar = pCountdown->m_nLastSetupBar; nBar < nLastBar; ++nBar)
        {
            if (!pCountdown->m_bSellCountdown           &&
                (m_pfClose[nBar]    > fMax)             &&
                (m_pfOpen[nBar + 1] > m_pfClose[nBar])  &&
                (m_pfHigh[nBar + 1] > m_pfOpen[nBar + 1]))
                break;
            
            if (pCountdown->m_bSellCountdown            &&
                (m_pfClose[nBar]    < fMax)             &&
                (m_pfOpen[nBar + 1] < m_pfClose[nBar])  &&
                (m_pfLow[nBar + 1]  < m_pfOpen[nBar + 1]))
                break;
        }
    }
    
    ASSERT(nBar <= m_nNumPoints - 1);
    ChartLineParameters pTDSTLineParms[] = {1, XRT_LPAT_LONG_DASH, blgGreen};
    pTDSTLineParms[0].line_style = QualifyBreakout(pGraph, this, !pCountdown->m_bSellCountdown, nBar, fMax) ? XRT_LPAT_SOLID : XRT_LPAT_LONG_DASH;
    pTDSTLineParms[0].line_color = (!pCountdown->m_bSellCountdown) ? blgGreen : blgRed;
    pGraph->AddStudyTrendLine(nMax, fMax, nBar, fMax, pTDSTLineParms, line_type_segment, &(m_pTrendLineIds[m_nTrendLineIndex++]));
        
    return 0;
}


#if (!defined __SUNPRO_CC) || (__SUNPRO_CC > 0x420)
CTDSequential::
#endif
XDeMarkCountdown* CTDSequential::GetFirstReverseCountdown(XDeMarkCountdown* pCountdown)
{
    ASSERT(pCountdown < &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()]);
    XDeMarkCountdown *pCountdownIndex = pCountdown + 1;
    for(; pCountdownIndex < &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()]; ++pCountdownIndex)
    {
        if (pCountdown->m_bSellCountdown != pCountdownIndex->m_bSellCountdown)
            break;
    }
        
    if (pCountdownIndex == &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()])
        return NULL;
    else if (pCountdownIndex->m_bCompletedSetup)
        return &pCountdownIndex[0];
    else
        return NULL;
}


#if (!defined __SUNPRO_CC) || (__SUNPRO_CC > 0x420)
CTDSequential::
#endif
XDeMarkCountdown* CTDSequential::GetFirstReverseCountdownBk(XDeMarkCountdown* pCountdown)
{
    ASSERT(pCountdown > &m_vDeMarkCountdowns[0]);
    XDeMarkCountdown* pCountdownIndex = pCountdown - 1;
    for(; pCountdownIndex > &m_vDeMarkCountdowns[0]; --pCountdownIndex)
    {
        if (pCountdown->m_bSellCountdown != pCountdownIndex->m_bSellCountdown)
            break;
    }

    ASSERT(pCountdownIndex->m_bCompletedSetup);
    if (pCountdown->m_bSellCountdown == pCountdownIndex->m_bSellCountdown)
        return NULL;
    else return pCountdownIndex;
}

#if (!defined __SUNPRO_CC) || (__SUNPRO_CC > 0x420)
CTDSequential::
#endif
XDeMarkCountdown* CTDSequential::GetFirstSimilarCountdown(XDeMarkCountdown* pCountdown)
{
    ASSERT(pCountdown < &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()]);
    XDeMarkCountdown* pCountdownIndex = pCountdown + 1;
    for(; pCountdownIndex < &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()]; ++pCountdownIndex)
    {
        if (pCountdown->m_bSellCountdown == pCountdownIndex->m_bSellCountdown)
            break;
    }
        
    if (pCountdownIndex == &m_vDeMarkCountdowns[m_vDeMarkCountdowns.size()])
        return NULL;
    else if (pCountdownIndex->m_bCompletedSetup)
        return pCountdownIndex;
    else
        return NULL;
}


//Refer to prototype
long CTDSequential::GetBarOfPriceFlip(enumPriceType nPrice, long nStart, long nEnd, short nLookback, BOOL bSell)
{
    ASSERT(nStart >= 0 && nEnd >= nStart && nEnd <= m_nNumPoints - 1);
    ASSERT(nLookback >= 0);
        
    long nBar;
    
    //Check to see if we have enough bars
    if (nStart - nLookback - 1 < 0)
        nBar = nEnd + 1;
    else
    {
        for (nBar = nStart; nBar <= nEnd; ++nBar)
        {
            float       fDay          = GetComparePrice(nPrice, bSell, nBar);
            float       fDayLookback  = GetComparePrice(nPrice, bSell, nBar - nLookback);
            float       fDay1         = GetComparePrice(nPrice, bSell, nBar - 1);
            float       fDay1Lookback = GetComparePrice(nPrice, bSell, nBar - 1 - nLookback);
            
            if (!IsInvalid(fDay, fDayLookback, fDay1, fDay1Lookback))
            {
                if (bSell && (fDay > fDayLookback) && (fDay1 <= fDay1Lookback))
                    break;
                else if (!bSell && (fDay < fDayLookback) && (fDay1 >= fDay1Lookback))
                    break;
            }
        }
    }
    
    return nBar;
}

BOOL CTDSequential::SetupWithinSetup(XDeMarkCountdown* pCountdown)
{
    ASSERT(pCountdown->m_bCompletedSetup);
    long nLast;
    
    switch (m_nSetupWithinSetupOptions[0])
    {
    case UseMinSetup:
        nLast = pCountdown->m_nFirstSetupBar + m_nMinSetupBars - 1;
        break;
    case UseMaxSetup:
    default:
        nLast = pCountdown->m_nLastSetupBar;
        break;
    }

    float *pfMax, *pfMin;
    
    switch(m_nSetupWithinSetupOptions[1])
    {
    case UseClosePrice:
        pfMax = m_pfClose;
        pfMin = m_pfClose;
        break;
    case UseTrueRange:
    default:
        pfMax = m_pfTrueHigh;
        pfMin = m_pfTrueLow;
    }
    
    long        nMaxCurrent = GetBarOfMax(pfMax, pCountdown->m_nFirstSetupBar, nLast);
    long        nMinCurrent = GetBarOfMin(pfMin, pCountdown->m_nFirstSetupBar, nLast);
    float       fMaxCurrent = pfMax[nMaxCurrent];
    float       fMinCurrent = pfMin[nMinCurrent];
        
    XDeMarkCountdown *pFirstReverseCountdown = GetFirstReverseCountdownBk(pCountdown);
    XDeMarkCountdown *p = pCountdown - 1;
    BOOL bWithin = FALSE;

    while (p != pFirstReverseCountdown && !bWithin)
    {
        long nLastBar;
        switch(m_nSetupWithinSetupOptions[0])
        {
        case UseMinSetup:
            nLastBar = p->m_nFirstSetupBar + m_nMinSetupBars - 1;
            break;
        case UseMaxSetup:
        default:
            nLastBar = p->m_nLastSetupBar;
            break;
        }
                        
        long nMaxBar = GetBarOfMax(m_pfTrueHigh, p->m_nFirstSetupBar, nLastBar);
        ASSERT(nMaxBar >= 0);
        long nMinBar = GetBarOfMin(m_pfTrueLow, p->m_nFirstSetupBar, nLastBar);
        ASSERT(nMinBar >= 0);
            
        if (m_pfTrueHigh[nMaxBar] > fMaxCurrent && m_pfTrueLow[nMinBar] < fMinCurrent)
            bWithin = TRUE;

        if (p == &m_vDeMarkCountdowns[0])
            break;
            
        --p;
    }

    //Modified by Arun for TREQ 36665. This disables Setup Within Setup if selected by the user. 
	
	if(m_bEnableSWithinS == FALSE) return FALSE;
	else return bWithin;
}


void CTDSequential::DrawRecycleLevel(XDeMarkCountdown* pCountdown, IBLPGraphData *pGraph)
{
    XDeMarkCountdown *pPreviousCountdown;
    pPreviousCountdown = pCountdown - 1;
    
    if (pPreviousCountdown->m_bSellCountdown == pCountdown->m_bSellCountdown)
    {
        float fPriorSpread = pPreviousCountdown->m_fMaxSetup - pPreviousCountdown->m_fMinSetup;
        long nFirstBar = pCountdown->m_nFirstSetupBar;
        long nLastBar = m_nNumPoints - 1;
        float fMax = m_pfTrueHigh[nFirstBar];
        float fMin = m_pfTrueLow[nFirstBar];
        long nBar;
                
        for (nBar = nFirstBar; nBar <= nLastBar; nBar++)
        {
            fMax = max(fMax, m_pfTrueHigh[nBar]);
            fMin = min(fMin, m_pfTrueLow[nBar]);
        }
                
        ChartLineParameters pRecycleLevelParms[] ={1, XRT_LPAT_SOLID, blgGreen};
        fPriorSpread = (pCountdown->m_bSellCountdown) ? fPriorSpread : -fPriorSpread;
        float fBaseLevel = (pCountdown->m_bSellCountdown) ? fMin : fMax;
        float fSmallRecycleLevel = fBaseLevel + fPriorSpread * m_fSmallRecycleLevel;
        float fLargeRecycleLevel = fBaseLevel + fPriorSpread * m_fLargeRecycleLevel;
                
        pGraph->AddStudyTrendLine(nFirstBar, fLargeRecycleLevel, nLastBar, fLargeRecycleLevel,
                                  pRecycleLevelParms, line_type_segment, &(m_pTrendLineIds[m_nTrendLineIndex++]));
        pGraph->AddStudyTrendLine(nFirstBar, fSmallRecycleLevel, nLastBar, fSmallRecycleLevel,
                                  pRecycleLevelParms, line_type_segment, &(m_pTrendLineIds[m_nTrendLineIndex++]));
    }
}

/***********************************************************************************
 ***************************  TD Combo  ******************************************** 
 ***********************************************************************************/



BOOL CTDCombo::TestTerminationCount(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount)
{
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Here are four possible interpretations for Termination Count in TD Combo, including
    // no Termination Count.  No Termination Count is what is currently implemented in the
    // core blpgraph.dll.  Calling SetFeature(FEATURE_GRAPH_RUN_EXTERNAL, blgCombo), will cause
    // TD Combo to run from this dll. If no specification of Termination Count is #define'd,
    // then FALSE is returned, meaniung no Termination count.  For the February 2002 Alpha
    // release the Termination Count implemented in response to DRQS 1464842 is #define'd,
    // so the BIG via a set feature can effectively reinsert the Termination Count requested
    // by that drqs.  That implementation was removed from the core graph code because DRQS 1553995
    // indicated that the specification was in error, and that a new specification would be
    // forthcoming.  In anticipation of a new specification two other possible interpretations
    // for Termination Count, as described on page 123 of "New Market Timing Techniques" are
    // provided.
    // 

//#define one of the following Termination Count tests (or implement another specification entirely)
//if none are #define'd, Termination Count will not be active in TD Combo.
//#define ImplementatedByDRQS1464842
#define Page123SubstituteFor
//#define Page123CombinationWith
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ImplementatedByDRQS1464842
    return CTDSequential::TestTerminationCount(pGraph, nBar, bSellCountdown, nCount);
#else
    CTDStudy::enumPriceType nPrice;
    nPrice = m_nTerminationCountPrice;
    if ((nPrice == High) && !bSellCountdown)
        nPrice = Low;

#ifdef Page123SubstituteFor
    return TestCountdownRule(pGraph, nBar, bSellCountdown, nCount, nPrice) ||
		TestCountdownRule(pGraph, nBar, bSellCountdown, nCount, Close);
#else
#ifdef Page123CombinationWith
    return CTDSequential::TestTerminationCount(pGraph, nBar, bSellCountdown, nCount) && 
        TestCountdownRule(pGraph, nBar, bSellCountdown, nCount, nPrice);
#else //No Termination Count
    return FALSE;
#endif //Page123CombinationWith
#endif //Page123SubstituteFor
#endif //ProposedChangeforDRQS1553995
}


BOOL CTDCombo::TestCountdownRule(IBLPGraphData *pGraph, long nBar, BOOL bSellCountdown, short nCount, CTDStudy::enumPriceType nComparePrice)
{
    BOOL bReturn;
    float *pfClose = GetStudyDataLine(pGraph, Close);
	float *pfHigh = GetStudyDataLine(pGraph, High);
	float *pfLow = GetStudyDataLine(pGraph, Low);
    if (nCount == 0) 
	{
		m_fPreviousCountdownClose = HOLEF;
        m_fPreviousCountdownHighOrLow = HOLEF;
	}
	
    if ( m_nComboVersion >= 2) 
	{  
        // If use countdown version 2 or greater
		if (nCount > 9) 
		{  
			// Use less restrictive elements when identifying Countdown days 11, 12, 13...
            if (bSellCountdown) 
			{ 
				// Sell Countdown

				//Aggressive Combo
                //Rule 4 - Coundown price > Previous Countdown High
				if(nComparePrice >= 8 && nComparePrice <=11)
				{ 
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayGreatThanTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownHighOrLow)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) == m_fPreviousCountdownHighOrLow));
					}
					else 
					{
						bReturn = ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownHighOrLow);
					}
				}

				//Regular Combo
				//Rule 4 - Coundown price < Previous Countdown Close
				else 
				{
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayGreatThanTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownClose)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) == m_fPreviousCountdownClose));
					}
					else 
					{
						bReturn = ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownClose);
					}
				}
            }
            else 
			{ 
				// Buy Countdown
				//Aggressive Combo
                //Rule 4 - Coundown price < Previous Countdown Low
				if(nComparePrice >= 8 && nComparePrice <= 11)
				{ 
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayGreatThanTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownHighOrLow)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == m_fPreviousCountdownHighOrLow));
					}
					else 
					{
						bReturn = ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownHighOrLow);
					}
				}

				//Regular Combo
				//Rule 4 - Coundown price < Previous Countdown Close
				else 
				{
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayGreatThanTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownClose)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == m_fPreviousCountdownClose));
					}
					else 
					{
						bReturn = ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownClose);
					}
				}    
            }
        }
                
        else 
		{ 
			// For day 2--10, use 4 Countdown rules
            if (bSellCountdown) 
			{ 
				// Sell Countdown
				
				//These rules is common for both Regular and Aggressive Combo
				
				//Rule 1 - CountdownPrice >= High 2 ago
				bReturn = ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) >= pfHigh[nBar - m_nCountdownLookback]);
				
				//Rule 2 - High > High 1 ago or High >= High 1 ago (depending on the user setting)
				if(m_nSellCntCond2Combo == 2)
				{
					bReturn = (((CBLPPrice)pfHigh[nBar] > pfHigh[nBar - 1])
						|| ((CBLPPrice)pfHigh[nBar] == pfHigh[nBar - 1]))
						&& bReturn;
				}
				else 
				{
					bReturn = bReturn && ((CBLPPrice)pfHigh[nBar] > pfHigh[nBar - 1]);
				}

				
				//The following set of rules are for Aggressive Combo
				if(nComparePrice >= 8 && nComparePrice <= 11)
				{ 
					//Rule 4 - Coundown price > Previous Countdown High
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayOneToTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownHighOrLow)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) == m_fPreviousCountdownHighOrLow))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownHighOrLow);
					}
				}
				
				//The following set of rules are for Regular Combo
				else 
				{
					//Rule 3 - Countdown Price > Countdown Price 1 Bar Ago 
					//                         or
					//Rule 3 - Countdown Price >= Countdown Price 1 Bar Ago (depending on the user setting)
					if(m_nSellCntCond1Combo == 2)
					{
						bReturn = (((CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar) > (CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar - 1))
							|| ((CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar) == (CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar - 1)))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar) > (CBLPPrice)GetComparePrice(nComparePrice, TRUE, nBar - 1));
					}

					//Rule 4 - Coundown price > Previous Countdown Close
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayOneToTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownClose)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) == m_fPreviousCountdownClose))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownClose);
					}
				}
            }

			//The following section is for Buy Countdown
            else 
			{ 
				//These rules is common for both Regular and Aggressive Combo
				
				//Rule 1 - CountdownPrice <= Low 2 ago
				bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < pfLow[nBar - m_nCountdownLookback])
					|| ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == pfLow[nBar - m_nCountdownLookback]));
				
				//Rule 2 - Low < Low 1 ago or Low <= Low 1 ago (depending on the user setting)
				if(m_nBuyCntCond2Combo == 2)
				{
					bReturn = (((CBLPPrice)pfLow[nBar] < pfLow[nBar - 1])
						|| ((CBLPPrice)pfLow[nBar] == pfLow[nBar - 1]))
						&& bReturn;
				}
				else 
				{
					bReturn = bReturn && ((CBLPPrice)pfLow[nBar] < pfLow[nBar - 1]);
				}

				
				//The following set of rules are for Aggressive Combo
				if(nComparePrice >=8 && nComparePrice <=11)
				{ 
					//Rule 4 - Coundown price < Previous Countdown Low
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayOneToTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownHighOrLow)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == m_fPreviousCountdownHighOrLow))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownHighOrLow);
					}
				}
				
				//The following set of rules are for Regular Combo
				else 
				{
					//Rule 3 - Countdown Price < Countdown Price 1 Bar Ago 
					//                         or
					//Rule 3 - Countdown Price <= Countdown Price 1 Bar Ago (depending on the user setting)
					if(m_nBuyCntCond1Combo == 2)
					{
						bReturn = (((CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar) < (CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar - 1))
							|| ((CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar) == (CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar - 1)))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar) < (CBLPPrice)GetComparePrice(nComparePrice, FALSE, nBar - 1));
					}

					//Rule 4 - Coundown price < Previous Countdown Close
					if(m_nComboCountdownUsesEqual == ComboCountdownUsesEqualDayOneToTen
						|| m_nComboCountdownUsesEqual == ComboCountdownUsesEqualAllDay)
					{
						bReturn = (((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownClose)
							|| ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == m_fPreviousCountdownClose))
							&& bReturn;
					}
					else 
					{
						bReturn = bReturn && ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownClose);
					}	            
				}
			}       
		}
   }     
    else 
	{  
		// If use countdown version 1 

        if (bSellCountdown) { // Sell Countdown
            bReturn = ((((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > pfHigh[nBar - m_nCountdownLookback]) ||
                        ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) == pfHigh[nBar - m_nCountdownLookback])) &&((CBLPPrice)pfClose[nBar] > pfClose[nBar - 1]) &&
                       ((CBLPPrice)pfHigh[nBar] > pfHigh[nBar - 1]) && ((CBLPPrice)GetComparePrice( nComparePrice, TRUE, nBar) > m_fPreviousCountdownClose));
        }
        else { // Buy countdown
            bReturn = ((((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < pfLow[nBar - m_nCountdownLookback]) ||
                        ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) == pfLow[nBar - m_nCountdownLookback])) &&((CBLPPrice)pfClose[nBar] < pfClose[nBar - 1]) &&
                       ((CBLPPrice)pfLow[nBar] < pfLow[nBar - 1]) && ((CBLPPrice)GetComparePrice( nComparePrice, FALSE, nBar) < m_fPreviousCountdownClose));
        }
                
    }

    if (bReturn) {
        m_fPreviousCountdownClose = pfClose[nBar];
        if(bSellCountdown) // Sell Countdown
			m_fPreviousCountdownHighOrLow = (CBLPPrice)pfHigh[nBar];
		else
            m_fPreviousCountdownHighOrLow = (CBLPPrice)pfLow[nBar];
	}

	return bReturn;
}

