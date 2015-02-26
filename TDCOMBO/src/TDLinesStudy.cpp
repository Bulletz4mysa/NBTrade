#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif


HRESULT CTDLines::DoStudy(IBLPGraphData *pGraph)
{
//	DebugBreak();	
	CComPtr<ITrendlineManager> iTrendlineMgr;
	HRESULT hr = pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineMgr);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	CComPtr<IChart> iChartProperties;
	hr = pSD->QueryInterface(IID_IChart, (LPVOID *)&iChartProperties);
	ASSERT(SUCCEEDED(hr));

	long	N;
	int nTrendLineIndex = 0;
	int nHighProjectionIndex = 0, nLowProjectionIndex = 0;
	int nProjectionsUp = 0, nProjectionsDown = 0;

	pGraph->get_NumPoints(&m_nNumPoints);
	N = m_nNumPoints;

	enumPriceType nPriceHigh = m_bUseTrueHighLow ? TrueHigh : High;
	enumPriceType nPriceLow = m_bUseTrueHighLow ? TrueLow : Low;
	float *pfHigh = GetStudyDataLine(pGraph, nPriceHigh);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;

	float *pfLow = GetStudyDataLine(pGraph, nPriceLow);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

	float *pfOpen = GetStudyDataLine(pGraph, Open);
	ASSERT(pfOpen != NULL);
	if (pfOpen == NULL)
		return E_HANDLE;

	float *pfTrueLow = GetStudyDataLine(pGraph, TrueLow);
	ASSERT(pfTrueLow != NULL);
	if (pfTrueLow == NULL)
		return E_HANDLE;

	float *pfTrueHigh = GetStudyDataLine(pGraph, TrueHigh);
	ASSERT(pfTrueHigh != NULL);
	if (pfTrueHigh == NULL)
		return E_HANDLE;

	float *pfDemandPrice = pfLow;
	ASSERT(pfDemandPrice != NULL);
	if (pfDemandPrice == NULL)
		return E_HANDLE;

	float *pfSupplyPrice = pfHigh;
	ASSERT(pfSupplyPrice != NULL);
	if (pfSupplyPrice == NULL)
		return E_HANDLE;

// define study lines
	CBLPPrice *fTDSupplyPoints = new CBLPPrice[N];
	CBLPPrice *fTDDemandPoints = new CBLPPrice[N];
	CBLPPrice *pfDemand = new CBLPPrice[N];
	CBLPPrice *pfSupply = new CBLPPrice[N];
	float *pfCanceled = new float[N];

	ChartLineParameters ObjectiveLineParms[] = {
	{1, XRT_LPAT_SOLID, RGB(255, 0, 0)}
	};

	BLPVector<XProjectionLine> HighProjection(MAX_TRENDLINES), 
		LowProjection(MAX_TRENDLINES), 
		MergedProjections(MAX_TRENDLINES);



// Find TD Points
    int i;
#ifdef DEBUG
// Check for free memory reads
	for (i = 0; i < N; i++) {
		float f1 = pfDemandPrice[i];
		float f2 = pfSupplyPrice[i];
		}
#endif
    int nLastday = N - m_nLevel - 1; 
	for (i = m_nLevel; i <= nLastday; i++) {
		BOOL bIsTDLow  = IsTDPoint(pfDemandPrice, i, m_nLevel, FALSE);
		BOOL bIsTDHigh = IsTDPoint(pfSupplyPrice, i, m_nLevel, TRUE);

		if (bIsTDLow)
			fTDDemandPoints[i] = pfDemandPrice[i];
		if (bIsTDHigh)
			fTDSupplyPoints[i] = pfSupplyPrice[i];
		}

	int *nDemandPointUse = new int[N];
	int *nSupplyPointUse = new int[N];
	memset(nDemandPointUse, 0, sizeof(int) * N);
	memset(nSupplyPointUse, 0, sizeof(int) * N);
	float fSlope;
	int nSupplyIndex, nDemandIndex;
	int nDisqualifiedLine = 0;
	bool bfoundLastUnbrokenSupplyLine = FALSE;
	bool bfoundLastUnbrokenDemandLine = FALSE;
	int nlastSupply = 0;
	int nlastDemand = 0;

	int nBreakoutIndex, nLastHighBreakout = -1, nLastLowBreakout = -1;
	int nLookback = (N > m_nLookback - 1) ? N + 1 - m_nLookback : 0;
	for (nDemandIndex = N - m_nLevel; nDemandIndex > nLookback; nDemandIndex--) {
		if ( (fTDDemandPoints[nDemandIndex] != HOLEF) && (nDemandPointUse[nDemandIndex]++ < m_nMaxUsePoint) ) {
			pfDemand[nDemandIndex] = fTDDemandPoints[nDemandIndex];
			
			int nDemandIndex2;
			int nLookback = (nDemandIndex > m_nLookback) ? nDemandIndex - m_nLookback : 0;
			for (nDemandIndex2 = nDemandIndex - 1; nDemandIndex2 >= nLookback; nDemandIndex2--) {
				if (nDemandIndex < 0) {
					nDemandIndex2 = nDemandIndex - m_nLookback - 1;
					break;
					}

				if ((CBLPPrice)fTDDemandPoints[nDemandIndex2] < fTDDemandPoints[nDemandIndex]) {
					if (nDemandPointUse[nDemandIndex2]++ < m_nMaxUsePoint) {
						//if we selected TDConfirm option, the close of point2, must be
						//less than the close of point1. If not continue
						if (m_nPointPrice == CBLPStudy::Close){
							float fclose1 = (CBLPPrice)pfClose[nDemandIndex];
							float fclose2 = (CBLPPrice)pfClose[nDemandIndex2];
							if (fclose1 <= fclose2){
							continue;
							}
						}
						pfDemand[nDemandIndex2] = fTDDemandPoints[nDemandIndex2];

						fSlope = (pfDemand[nDemandIndex] - pfDemand[nDemandIndex2]) / (float)(nDemandIndex - nDemandIndex2);
						if (nDemandIndex < N - 1) {
							pfDemand[nDemandIndex + 1] = pfDemand[nDemandIndex] + fSlope;
							pfDemand[nDemandIndex] = HOLEF;
							}

						int nNumTicks = 2;
						for (nBreakoutIndex = nDemandIndex + nNumTicks; nBreakoutIndex < N; nBreakoutIndex++) {
							float fProjected = pfDemand[nBreakoutIndex - 1] + fSlope;
							pfDemand[nBreakoutIndex - 1] = HOLEF;
							pfDemand[nBreakoutIndex] = fProjected;
							if ((CBLPPrice)fProjected > pfLow[nBreakoutIndex])
								break;	
						}
						
						if (nBreakoutIndex == N && !bfoundLastUnbrokenDemandLine){
									nlastDemand = nDemandIndex;
									bfoundLastUnbrokenDemandLine = TRUE;
						}
						
					}
					else
						nDemandIndex2 = nLookback - 1;

					break;
					}
				}

			if (nDemandIndex2 < nLookback) { // didn't find an endpoint, skip to next demand point
				pfDemand[nDemandIndex] = HOLEF;
				continue;
				}
			if ((bfoundLastUnbrokenDemandLine) && (nBreakoutIndex == N)){ //already found an unbroken demand line, don't show others.
				if (nlastDemand != nDemandIndex)
				{	pfDemand[nDemandIndex] = HOLEF;
					continue;
				}
			}

						
	//Qualify the breakout
			ChartLineParameters *pLineParms;

			BOOL bCheckCancel = TRUE;
			BOOL bBreakout = TRUE;

			if (nBreakoutIndex == N) {
				nBreakoutIndex = N - 1;
				bBreakout = FALSE;
				}

			if (nBreakoutIndex == N - 1)
				bCheckCancel = FALSE;
		
	
			BOOL bQualified = QualifyBreakout(pGraph, this, FALSE, nBreakoutIndex, pfDemand[nBreakoutIndex]);
		
			//Break out on the last bar
			if (bQualified && !bCheckCancel && bBreakout) {
				int i;
				CBLPPrice fExtremeHigh;
				CBLPPrice fProjection;
				for (i = nDemandIndex2 + 1; i <= nBreakoutIndex; i++) {
					pfDemand[i] = pfDemand[i - 1] + fSlope;
					if ((CBLPPrice)pfHigh[i] > fExtremeHigh) {
						fExtremeHigh = pfHigh[i];
						fProjection = fExtremeHigh - pfDemand[i];
						}
					}
		

				fProjection = pfDemand[nBreakoutIndex] - (fProjection * (m_fProjectionPercent / 100));
				nLastLowBreakout = nBreakoutIndex;
				LowProjection[nLowProjectionIndex].fPrice = fProjection;
				LowProjection[nLowProjectionIndex].nFirstBar = nBreakoutIndex;
				LowProjection[nLowProjectionIndex].nLastBar = nBreakoutIndex;

				if ( (m_nProjections != LastWithFibonacci) && (m_nProjections != noProjections) && m_bDisplayQualified)
						nLowProjectionIndex++;
				}

			if (bQualified && bCheckCancel) {
				// Check cancel criteria
				if (m_bCancel1 && 
					((CBLPPrice)pfOpen[nBreakoutIndex + 1] > (pfDemand[nBreakoutIndex]) + fSlope))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else if (m_bCancel2 &&
					((CBLPPrice)pfOpen[nBreakoutIndex + 1] > pfClose[nBreakoutIndex - 1]) &&
					((CBLPPrice)pfClose[nBreakoutIndex + 1] > (pfDemand[nBreakoutIndex]) + fSlope))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else if (m_bCancel3 &&
					((CBLPPrice)pfLow[nBreakoutIndex + 1] >= pfLow[nBreakoutIndex]))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else {
					pLineParms = m_bDisplayQualified && m_bDisplayDemand ? &m_QualifiedDemandLineParms : NULL;
					int i;
					CBLPPrice fExtremeHigh;
					CBLPPrice fProjection;
					for (i = nDemandIndex2 + 1; i <= nBreakoutIndex; i++) {
						pfDemand[i] = pfDemand[i - 1] + fSlope;
						if ((CBLPPrice)pfHigh[i] > fExtremeHigh) {
							fExtremeHigh = pfHigh[i];
							fProjection = fExtremeHigh - pfDemand[i];
							}
						}

					fProjection = pfDemand[nBreakoutIndex] - (fProjection * (m_fProjectionPercent / 100));
					if ( (nLastLowBreakout < nBreakoutIndex) || (m_nProjections == allProjections) ) {
						nLastLowBreakout = nBreakoutIndex;
						LowProjection[nLowProjectionIndex].fPrice = fProjection;
						LowProjection[nLowProjectionIndex].nFirstBar = nBreakoutIndex;//nDemandIndex2;
						for (LowProjection[nLowProjectionIndex].nLastBar = nBreakoutIndex; LowProjection[nLowProjectionIndex].nLastBar < N - 1; LowProjection[nLowProjectionIndex].nLastBar++) {
							if (LowProjection[nLowProjectionIndex].fPrice >= pfLow[LowProjection[nLowProjectionIndex].nLastBar])
								break;
							}
						if ( (m_nProjections != LastWithFibonacci) && (m_nProjections != noProjections) && m_bDisplayQualified)
							nLowProjectionIndex++;							
						}

					}
				}
			else if (bQualified)
				pLineParms = m_bDisplayQualified  && m_bDisplayDemand ? &m_QualifiedDemandLineParms : NULL;
			else 
				pLineParms = m_bDisplayDisqualified  && m_bDisplayDemand ? &m_DisqualifiedLineParms : NULL;

			if (pLineParms != NULL)
				iTrendlineMgr->AddStudyTrendLine(nDemandIndex2, pfDemand[nDemandIndex2],
								  nBreakoutIndex, pfDemand[nBreakoutIndex], pLineParms,
								  bBreakout ? line_type_segment : line_type_extended_forward,
								  &(m_pTrendlineIds[nTrendLineIndex++]));
			
			pfDemand[nBreakoutIndex] = HOLEF;
			pfDemand[nDemandIndex2] = HOLEF;
			}
		}

	for (nSupplyIndex = N - m_nLevel; nSupplyIndex > nLookback; nSupplyIndex--) {
		if ( (fTDSupplyPoints[nSupplyIndex] != HOLEF) &&
			(nSupplyPointUse[nSupplyIndex]++ < m_nMaxUsePoint) ) {
			pfSupply[nSupplyIndex] = fTDSupplyPoints[nSupplyIndex];
			
			int nSupplyIndex2;
			int nLookback = (nSupplyIndex > m_nLookback) ? nSupplyIndex - m_nLookback : 0;
			for (nSupplyIndex2 = nSupplyIndex - 1; nSupplyIndex2 >= nLookback; nSupplyIndex2--) {
				if (nSupplyIndex < 0) {
					nSupplyIndex2 = nSupplyIndex - m_nLookback - 1;
					break;
					}

				if ((CBLPPrice)fTDSupplyPoints[nSupplyIndex2] > fTDSupplyPoints[nSupplyIndex]) {
					if (nSupplyPointUse[nSupplyIndex2]++ < m_nMaxUsePoint) {
							//if we selected TDConfirm option, the close of point2, must be
							//greater than the close of point1. If not continue
							if (m_nPointPrice == CBLPStudy::Close){	
								float fclose1 = (CBLPPrice)pfClose[nSupplyIndex];
								float fclose2 = (CBLPPrice)pfClose[nSupplyIndex2];
								if (fclose1 >= fclose2){
									continue;
								}
							}
						pfSupply[nSupplyIndex2] = fTDSupplyPoints[nSupplyIndex2];

						fSlope = (pfSupply[nSupplyIndex] - pfSupply[nSupplyIndex2]) / (float)(nSupplyIndex - nSupplyIndex2);
						if (nSupplyIndex < N - 1) {
							pfSupply[nSupplyIndex + 1] = pfSupply[nSupplyIndex] + fSlope;
							pfSupply[nSupplyIndex] = HOLEF;
							}
				
						int nNumTicks = 2;
						for (nBreakoutIndex = nSupplyIndex + nNumTicks; nBreakoutIndex < N; nBreakoutIndex++) {
							float fProjected = pfSupply[nBreakoutIndex - 1] + fSlope;
							pfSupply[nBreakoutIndex - 1] = HOLEF;
							pfSupply[nBreakoutIndex] = fProjected;
							if ((CBLPPrice)fProjected < pfHigh[nBreakoutIndex]) 
								break;
						}
						
						if (nBreakoutIndex == N && !bfoundLastUnbrokenSupplyLine){
										nlastSupply=nSupplyIndex;
										bfoundLastUnbrokenSupplyLine = TRUE;
						}	
						
					}			
					else
						nSupplyIndex2 = nLookback - 1;

					break;
					}
				}

			if (nSupplyIndex2 <nLookback) { // didn't find an endpoint, skip to next demand point
				pfSupply[nSupplyIndex] = HOLEF;
				continue;
				}
			if ((bfoundLastUnbrokenSupplyLine) && (nBreakoutIndex == N)){ // we already have a most recent unbroken Supply so we don't want more.
				if (nlastSupply != nSupplyIndex)
				{
					pfSupply[nSupplyIndex] = HOLEF;
					continue;
				}
			}

			

	//Qualify the breakout
			BOOL bCheckCancel = TRUE;
			BOOL bBreakout = TRUE;
		
			if (nBreakoutIndex == N) {
				nBreakoutIndex = N - 1;
				bBreakout = FALSE;
				}

			if (nBreakoutIndex == N - 1)
				bCheckCancel = FALSE;

			ChartLineParameters *pLineParms;

			BOOL bQualified = QualifyBreakout(pGraph, this, TRUE, nBreakoutIndex, pfSupply[nBreakoutIndex]);
			if (bQualified && !bCheckCancel && bBreakout) {
				int i;
				CBLPPrice fExtremeLow;
				CBLPPrice fProjection;
				for (i = nSupplyIndex2 + 1; i <= nBreakoutIndex; i++) {
					pfSupply[i] = pfSupply[i - 1] + fSlope;
					if ((CBLPPrice)pfLow[i] < fExtremeLow) {
						fExtremeLow = pfLow[i];
						fProjection = pfSupply[i] - fExtremeLow;
						}
					}

				fProjection = pfSupply[nBreakoutIndex] + (fProjection * (m_fProjectionPercent / 100));
				if ( (HighProjection[nHighProjectionIndex].nLastBar < nBreakoutIndex) || (m_nProjections == allProjections) ) {
					nLastHighBreakout = nBreakoutIndex;
					HighProjection[nHighProjectionIndex].fPrice = fProjection;
					HighProjection[nHighProjectionIndex].nFirstBar = nBreakoutIndex;
					HighProjection[nHighProjectionIndex].nLastBar = nBreakoutIndex; 
					
					if ( (m_nProjections != LastWithFibonacci) && (m_nProjections != noProjections) && m_bDisplayQualified)
						nHighProjectionIndex++;					
					}
				}

			if (bQualified && bCheckCancel) {
				// Check cancel criteria
				if (m_bCancel1 && 
					((CBLPPrice)pfOpen[nBreakoutIndex + 1] < (pfSupply[nBreakoutIndex]) + fSlope))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else if (m_bCancel2 && 
					((CBLPPrice)pfOpen[nBreakoutIndex + 1] < pfClose[nBreakoutIndex - 1]) &&
					((CBLPPrice)pfClose[nBreakoutIndex + 1] < (pfSupply[nBreakoutIndex]) + fSlope))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else if (m_bCancel3 && 
					((CBLPPrice)pfHigh[nBreakoutIndex + 1] <= pfHigh[nBreakoutIndex]))
					pLineParms = m_bDisplayCancel ? &m_CancelLineParms : NULL;

				else {
					pLineParms = m_bDisplayQualified && m_bDisplaySupply ? &m_QualifiedSupplyLineParms : NULL;;
					int i;
					CBLPPrice fExtremeLow;
					CBLPPrice fProjection;
					for (i = nSupplyIndex2 + 1; i <= nBreakoutIndex; i++) {
						pfSupply[i] = pfSupply[i - 1] + fSlope;
						if ((CBLPPrice)pfLow[i] < fExtremeLow) {
							fExtremeLow = pfLow[i];
							fProjection = pfSupply[i] - fExtremeLow;
							}
						}

					fProjection = pfSupply[nBreakoutIndex]  + (fProjection * (m_fProjectionPercent / 100));
					if ( (nLastHighBreakout < nBreakoutIndex) || (m_nProjections == allProjections) ) {
						nLastHighBreakout = nBreakoutIndex;
						HighProjection[nHighProjectionIndex].fPrice = fProjection;
						HighProjection[nHighProjectionIndex].nFirstBar = nBreakoutIndex;//nSupplyIndex2;
						for (HighProjection[nHighProjectionIndex].nLastBar = nBreakoutIndex; HighProjection[nHighProjectionIndex].nLastBar < N - 1; HighProjection[nHighProjectionIndex].nLastBar++) {
							if (HighProjection[nHighProjectionIndex].fPrice <= pfHigh[HighProjection[nHighProjectionIndex].nLastBar])
								break;
							}
						
						if ( (m_nProjections != LastWithFibonacci) && (m_nProjections != noProjections) && m_bDisplayQualified)
							nHighProjectionIndex++;
							
						}
					}
				}
			else if (bQualified)
				pLineParms = m_bDisplayQualified && m_bDisplaySupply ? &m_QualifiedSupplyLineParms : NULL;
			else
				pLineParms = m_bDisplayDisqualified && m_bDisplaySupply ? &m_DisqualifiedLineParms : NULL;;


			if (pLineParms != NULL)
				iTrendlineMgr->AddStudyTrendLine(nSupplyIndex2, pfSupply[nSupplyIndex2],
								  nBreakoutIndex, pfSupply[nBreakoutIndex], pLineParms,
								  bBreakout ? line_type_segment : line_type_extended_forward,
								  &(m_pTrendlineIds[nTrendLineIndex++]));
			
			pfSupply[nBreakoutIndex] = HOLEF;
			pfSupply[nSupplyIndex2] = HOLEF;
			}
		}

// Price Projections

	if ( ((HighProjection[0].nFirstBar == 0) || (LowProjection[0].nFirstBar == 0))
		   && (m_nProjections == LastWithFibonacci) ) {
		m_nProjections = LastOnly; // no Fibonacci projections without both a high and a low
		}

	if (m_bDisplayQualified)
	switch (m_nProjections) {
	  case noProjections:
		break;

	  case LastWithFibonacci: {
		long nFirstBar = (HighProjection[0].nLastBar > LowProjection[0].nFirstBar) ? LowProjection[0].nFirstBar : HighProjection[0].nFirstBar;
		float fSpread = (HighProjection[0].fPrice - LowProjection[0].fPrice) / 0.472f;
		float fFibHigh = HighProjection[0].fPrice + (fSpread * 0.382f);

		iTrendlineMgr->AddStudyTrendLine(HighProjection[0].nLastBar, fFibHigh - fSpread, nFirstBar, 
								  fFibHigh, ObjectiveLineParms, //4
								  line_type_study_fibonacci_projection, &(m_pTrendlineIds[nTrendLineIndex++]));
		}
		break;

	  case LastOnly:
		iTrendlineMgr->AddStudyTrendLine(HighProjection[0].nFirstBar, HighProjection[0].fPrice, HighProjection[0].nLastBar, 
								  HighProjection[0].fPrice, ObjectiveLineParms, //5
								  line_type_study_price_projection, &(m_pTrendlineIds[nTrendLineIndex++]));
		iTrendlineMgr->AddStudyTrendLine(LowProjection[0].nFirstBar, LowProjection[0].fPrice, LowProjection[0].nLastBar, 
								  LowProjection[0].fPrice, ObjectiveLineParms, //5
								  line_type_study_price_projection, &(m_pTrendlineIds[nTrendLineIndex++]));
		break;

	  case allProjections: {
	    if (nLowProjectionIndex + nHighProjectionIndex == 0)
		    break;

		BLPSort(HighProjection.begin(), HighProjection.begin() + nHighProjectionIndex);
		BLPSort(LowProjection.begin(), LowProjection.begin() + nLowProjectionIndex);

		int nNextHighIndex, nNextLowIndex, nMergeIndex;

		for (nNextHighIndex = 0, nNextLowIndex = 0, nMergeIndex = 0; nNextHighIndex < nHighProjectionIndex; nNextHighIndex++) {
			for (; nNextLowIndex < nLowProjectionIndex; nNextLowIndex++) {
				if (HighProjection[nNextHighIndex].nFirstBar < LowProjection[nNextLowIndex].nFirstBar)
					break;
				MergedProjections[nMergeIndex] = LowProjection[nNextLowIndex];
				nMergeIndex++;
				}
			MergedProjections[nMergeIndex] = HighProjection[nNextHighIndex];
			nMergeIndex++;
			}
		
		for (; nNextLowIndex < nLowProjectionIndex;  nNextLowIndex++) {
			MergedProjections[nMergeIndex] = LowProjection[nNextLowIndex];
			nMergeIndex++;
			}

	     
		for (nMergeIndex = 0; nMergeIndex < nHighProjectionIndex + nLowProjectionIndex - 1; nMergeIndex++) {
			long nLastBar = (MergedProjections[nMergeIndex].nLastBar < MergedProjections[nMergeIndex + 1].nFirstBar) ? MergedProjections[nMergeIndex].nLastBar : MergedProjections[nMergeIndex + 1].nFirstBar;
			iTrendlineMgr->AddStudyTrendLine(MergedProjections[nMergeIndex].nFirstBar, MergedProjections[nMergeIndex].fPrice,
						nLastBar, MergedProjections[nMergeIndex].fPrice, ObjectiveLineParms, //5
						line_type_study_price_projection, &(m_pTrendlineIds[nTrendLineIndex++]));
			}
		iTrendlineMgr->AddStudyTrendLine(MergedProjections[nMergeIndex].nFirstBar, MergedProjections[nMergeIndex].fPrice,
					MergedProjections[nMergeIndex].nLastBar, MergedProjections[nMergeIndex].fPrice, ObjectiveLineParms, //5
					line_type_study_price_projection, &(m_pTrendlineIds[nTrendLineIndex++]));
		break;
		}
	}
	iTrendlineMgr->UpdateTrendLines();

	delete[] fTDSupplyPoints;
	delete[] fTDDemandPoints;
	delete[] pfDemand;
	delete[] pfSupply;
	delete[] nDemandPointUse;
	delete[] nSupplyPointUse;
	delete[] pfCanceled;
	
	return NOERROR;
}

HRESULT CTDLines::CreateStudy(IBLPGraphStudyList * pGraph) 
{
	ChartLineParameters LineParms[1];
	ChartPointParameters PointParms[1];

	LineParms[0].line_width = 1;
	LineParms[0].line_style = XRT_LPAT_SOLID;
	LineParms[0].line_color = RGB(255,255,255);

	PointParms[0].point_style = XRT_POINT_NONE;
	PointParms[0].point_width = 1;
	PointParms[0].point_color = RGB(255,255,255);

	HRESULT rc = AddStudy(pGraph, 1, 1, LineParms, PointParms);
    //If there is no data for this security, it will fail at this point
    if (FAILED(rc))
        return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "TD Lines", this);
		}
	return rc;//CBLPStudy::CreateStudy(pGraph);
}

HRESULT CTDLines::ConvertBufEx(StudyMessagePtr pBuf)
{
	long *pConvertFloat;
	pConvertFloat = (long *)&(pBuf->TDLines.fProjectionPercent);
	*pConvertFloat = ntohl(*pConvertFloat);

	pBuf->TDLinesEx.nNumTicks = ntohs(pBuf->TDLinesEx.nNumTicks);
	pBuf->TDLinesEx.nLookback = ntohs(pBuf->TDLinesEx.nLookback);
	pBuf->TDLinesEx.nMaxUsePoint = ntohs(pBuf->TDLinesEx.nMaxUsePoint);
	pBuf->TDLinesEx.nLevel = ntohs(pBuf->TDLinesEx.nLevel);
	pBuf->TDLinesEx.nProjections = ntohs(pBuf->TDLinesEx.nProjections);
	pBuf->TDLinesEx.nPointPrice = ntohs(pBuf->TDLinesEx.nPointPrice);

	ConvertLineParameters(pBuf->TDLinesEx.line_params, 4);

	m_fProjectionPercent = pBuf->TDLinesEx.fProjectionPercent;
#ifndef jfkDEBUG
	m_nProjections = (enum tagEnumShowProjections)pBuf->TDLinesEx.nProjections;
#endif
	m_nNumTicks = pBuf->TDLinesEx.nNumTicks;
	m_nLookback = pBuf->TDLinesEx.nLookback;
	m_nMaxUsePoint = pBuf->TDLinesEx.nMaxUsePoint;
	m_nLevel = pBuf->TDLinesEx.nLevel;
	m_bQualifier1 = (pBuf->TDLinesEx.bQualifier1 != 0);
	m_bQualifier2 = pBuf->TDLinesEx.bQualifier2;
	m_bQualifier3 = pBuf->TDLinesEx.bQualifier3;
	m_bUseTrueHighLow = pBuf->TDLinesEx.bUseTrueHighLow;
	m_bDisplayDemand = pBuf->TDLinesEx.bDisplayDemand;
	m_bDisplaySupply = pBuf->TDLinesEx.bDisplaySupply;
	m_bDisplayQualified = pBuf->TDLinesEx.bDisplayQualified;
	m_bDisplayDisqualified = pBuf->TDLinesEx.bDisplayDisqualified;
	m_bDisplayTDPoints = pBuf->TDLinesEx.bDisplayTDPoints;
	m_bCancel1 = pBuf->TDLinesEx.bCancel1;
	m_bCancel2 = pBuf->TDLinesEx.bCancel2;
	m_bCancel3 = pBuf->TDLinesEx.bCancel3;
	m_bDisplayCancel = pBuf->TDLinesEx.bDisplayCancel;
	m_nPointPrice = (enumPriceType)pBuf->TDLinesEx.nPointPrice;

	//overriding value of m_nNumTicks since it wasn't being used correctly. Will always be 2.
	//We want to look for a breakout value starting on the day after a TD Line endpoint, so
	//m_nNumTicks should always be set to 1 as in 1 day after TD Line endpoint begin search for
	// breakout.

	m_nNumTicks = 2;

	memcpy(&m_QualifiedDemandLineParms, &(pBuf->TDLinesEx.line_params[0]), sizeof(ChartLineParameters));
	memcpy(&m_QualifiedSupplyLineParms, &(pBuf->TDLinesEx.line_params[1]), sizeof(ChartLineParameters));
	memcpy(&m_DisqualifiedLineParms, &(pBuf->TDLinesEx.line_params[2]), sizeof(ChartLineParameters));
	memcpy(&m_CancelLineParms, &(pBuf->TDLinesEx.line_params[3]), sizeof(ChartLineParameters));
	return NOERROR;
}

HRESULT CTDLines::ConvertBuf(void * pBigBuf)
{
	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pBuf = (StudyMessagePtr)pBigBuf;
	if (pBuf->StudyHeader.msg_id == blgTDLinesEx)
		return ConvertBufEx(pBuf);

	long *pConvertFloat;
	pConvertFloat = (long *)&(pBuf->TDLines.fProjectionPercent);
	*pConvertFloat = ntohl(*pConvertFloat);

	pBuf->TDLines.nNumTicks = ntohs(pBuf->TDLines.nNumTicks);
	pBuf->TDLines.nLookback = ntohs(pBuf->TDLines.nLookback);
	pBuf->TDLines.nMaxUsePoint = ntohs(pBuf->TDLines.nMaxUsePoint);
	pBuf->TDLines.nLevel = ntohs(pBuf->TDLines.nLevel);
	pBuf->TDLines.nProjections = ntohs(pBuf->TDLines.nProjections);

	ConvertLineParameters(pBuf->TDLines.line_params, 4);

	m_fProjectionPercent = pBuf->TDLines.fProjectionPercent;
#ifndef jfkDEBUG
	m_nProjections = (enum tagEnumShowProjections)pBuf->TDLines.nProjections;
#endif
	m_nNumTicks = pBuf->TDLines.nNumTicks;
	m_nLookback = pBuf->TDLines.nLookback;
	m_nMaxUsePoint = pBuf->TDLines.nMaxUsePoint;
	m_nLevel = pBuf->TDLines.nLevel;
	m_bQualifier1 = (pBuf->TDLines.bQualifier1 != 0);
	m_bQualifier2 = pBuf->TDLines.bQualifier2;
	m_bQualifier3 = pBuf->TDLines.bQualifier3;
	m_bUseTrueHighLow = pBuf->TDLines.bUseTrueHighLow;
	m_bDisplayDemand = pBuf->TDLines.bDisplayDemand;
	m_bDisplaySupply = pBuf->TDLines.bDisplaySupply;
	m_bDisplayQualified = pBuf->TDLines.bDisplayQualified;
	m_bDisplayDisqualified = pBuf->TDLines.bDisplayDisqualified;
	m_bDisplayTDPoints = pBuf->TDLines.bDisplayTDPoints;
	m_bCancel1 = pBuf->TDLines.bCancel1;
	m_bCancel2 = pBuf->TDLines.bCancel2;
	m_bCancel3 = pBuf->TDLines.bCancel3;
	m_bDisplayCancel = pBuf->TDLines.bDisplayCancel;

	memcpy(&m_QualifiedDemandLineParms, &(pBuf->TDLines.line_params[0]), sizeof(ChartLineParameters));
	memcpy(&m_QualifiedSupplyLineParms, &(pBuf->TDLines.line_params[1]), sizeof(ChartLineParameters));
	memcpy(&m_DisqualifiedLineParms, &(pBuf->TDLines.line_params[2]), sizeof(ChartLineParameters));
	memcpy(&m_CancelLineParms, &(pBuf->TDLines.line_params[3]), sizeof(ChartLineParameters));
	return NOERROR;
}

BOOL CTDLines::IsTDPoint(float* fPrice, long nBar, long nLevel, BOOL bHigh)
{
	ASSERT(fPrice != NULL && nBar < m_nNumPoints - 1 && nBar >= 0);
	ASSERT(nLevel > 0);

	long nBarLeft  = nBar;
	long nBarRight = nBar;
	long nLeftCnt  = 0;
	long nRightCnt = 0;

	//We will first look at the left side of nBar
	while (nLeftCnt < nLevel && --nBarLeft >= 0)
	{
		if (fPrice[nBar] == fPrice[nBarLeft])
			continue;

		if (bHigh)
		{
			if (fPrice[nBar] > fPrice[nBarLeft])
				++nLeftCnt;
			else
				break;
		}
		else
		{
			if (fPrice[nBar] < fPrice[nBarLeft])
				++nLeftCnt;
			else
				break;

		}
	}

	//Then if the left side is OK, we will look at the right side
	if (nLeftCnt == nLevel)
	{
		while (nRightCnt < nLevel && ++nBarRight <= m_nNumPoints - 1)
		{
			if (fPrice[nBar] == fPrice[nBarRight])
				continue;

			if (bHigh)
			{
				if (fPrice[nBar] > fPrice[nBarRight])
					++nRightCnt;
				else
					break;
			}
			else
			{
				if (fPrice[nBar] < fPrice[nBarRight])
					++nRightCnt;
				else
					break;

			}
		}
	}

	if (nLeftCnt == nLevel && nRightCnt == nLevel)
		return TRUE;
	else
		return FALSE;

}
