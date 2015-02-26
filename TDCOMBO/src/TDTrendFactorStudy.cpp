#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif
#include <math.h>



HRESULT CTDTrendFactor::DoRetracement(long x_value, BOOL bDirection, IBLPGraphData *pGraph)
{

	ITrendlineManager *iTrendlineMgr;
	HRESULT hr = pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineMgr);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	long	N;
	pGraph->get_NumPoints(&N);

	float *pfHigh = GetStudyDataLine(pGraph, High);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_HANDLE;

	float *pfLow = GetStudyDataLine(pGraph, Low);
	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_HANDLE;

	float *pfProjectionPrice = GetStudyDataLine(pGraph, m_nProjectionPrice);
	ASSERT(pfProjectionPrice != NULL);
	if (pfProjectionPrice == NULL)
		return E_HANDLE;

	float *pfClose = GetStudyDataLine(pGraph, Close);
	ASSERT(pfClose != NULL);
	if (pfClose == NULL)
		return E_HANDLE;

	float *pfOpen = GetStudyDataLine(pGraph, Open);
	ASSERT(pfOpen != NULL);
	if (pfOpen == NULL)
		return E_HANDLE;


	int i;
	CBLPPrice fHigh, fLow;
	float fProjectionPrice = pfProjectionPrice[x_value];

	long *pnRetracementId = &m_nRetracementIds[0];

	long nLookback = x_value - m_nMaxLookback;
	if (nLookback < 0)
		nLookback = 0;

	// calculate whether we have a valid point for retracement
	// see {IDOC #935145<GO>} point 14
	if (!bDirection) {
		//calculate for trend factor up
		for (i = x_value; i >= nLookback; i--) {
			if ((CBLPPrice)pfHigh[i] > fHigh ) {
				fHigh = pfHigh[i];
				if ((fHigh - pfLow[x_value]) / fHigh > 0.0556f)
					break;
			}

			if (pfLow[i] < pfLow[x_value]) {
				i=nLookback-1;
			}
		}
	} else {
		// trend factor down
		for (i = x_value; i >= nLookback; i--) {
			if ((CBLPPrice)pfLow[i] < fLow ) {
				fLow = pfLow[i];
				if ((pfHigh[x_value] - fLow) / fLow > 0.0556f)
					break;
			}
			
			if (pfHigh[i] > pfHigh[x_value]) {
				i=nLookback-1;
			}
		}
	}


	// calculate retracement levels
	float fRetracement = fProjectionPrice;
	if (i >= nLookback) {
		int nLevel;
		int nIntersectBar;
		float fFirstProjection;
		float fUpIncrement = fProjectionPrice * (m_bDisplayHalfLevels ? 0.0556f/ 2 : 0.0556f);
		short nNumLevels = m_bDisplayHalfLevels ? 9 : 5;

		for (nLevel = 0; nLevel < nNumLevels; nLevel++) {
			BOOL bDisplayHalfLevels = (nLevel > 0) ? m_bDisplayHalfLevels : FALSE;
			if (bDirection) {				
				fRetracement = (float) (fRetracement * (bDisplayHalfLevels ? sqrt(1.0f - 0.0556f) : 1.0f - 0.0556f));
			} else {
				fRetracement += (nLevel > 0) ? fUpIncrement : fProjectionPrice * 0.0556f;				
			}

			// Calculate first level - first full projection only is based from close
			// see {IDOC #935145<GO>} point 11
			if(nLevel == 0) {
				// save fRetracement to temp variable
				fFirstProjection = fRetracement;
				// calculate new fRetracement
				if(bDirection) {
					// first full projection only is based from close	
					if ((pfClose[x_value] < pfClose[x_value-1]) && (pfHigh[x_value] > pfHigh[x_value-1]))		
						fRetracement = pfClose[x_value] * (1.0f - 0.0556f);
				} else {
					// first full projection only is based from close	
					if ((pfClose[x_value] > pfClose[x_value-1]) && (pfLow[x_value] < pfLow[x_value-1]))
						fRetracement = pfClose[x_value] + (pfClose[x_value] * 0.0556f);
				}
			}


			// Calculate x intercept 
			float fHiLo;
			if(bDirection) {
				fHiLo=pfLow[x_value + 1];
			} else {
				fHiLo=pfHigh[x_value + 1];
			}
									  


			// see {IDOC #935145<GO>} points 12-13 for details on calculation of x
			// intercept
			for (nIntersectBar = x_value + 1; nIntersectBar < N - 1; nIntersectBar++) {
				if (bDirection) {
					if(pfLow[nIntersectBar] < fHiLo) {
						fHiLo = pfLow[nIntersectBar];
					}

					if( ((pfHigh[nIntersectBar] - fHiLo) / fHiLo) > 0.0556f ) {
						break;
					}

					if ( (CBLPPrice)pfClose[nIntersectBar] < fRetracement 
						&& pfOpen[nIntersectBar + 1] < pfClose[nIntersectBar]
						&& pfLow[nIntersectBar + 1] < pfOpen[nIntersectBar + 1])
						break;
				} else {
					if(pfHigh[nIntersectBar] > fHiLo) {
						fHiLo = pfHigh[nIntersectBar];
					}
					
					if( ((fHiLo - pfLow[nIntersectBar]) / fHiLo) > 0.0556f ) {
						break;
					}

					if ( (CBLPPrice)pfClose[nIntersectBar] > fRetracement 
						&& pfOpen[nIntersectBar + 1] > pfClose[nIntersectBar]
						&& pfHigh[nIntersectBar + 1] > pfOpen[nIntersectBar + 1])
						break;
				}
			}

			if (nIntersectBar == N) {
				nIntersectBar = N - 1;
				m_LineParms[0].line_style = XRT_LPAT_SOLID;
			}
			else
				m_LineParms[0].line_style = QualifyBreakout(pGraph, this, !bDirection, nIntersectBar, fRetracement) ?  XRT_LPAT_SOLID : XRT_LPAT_LONG_DASH;

			

			if(m_bShowLevel[nLevel]) {
				iTrendlineMgr->AddStudyTrendLine(x_value, fRetracement, nIntersectBar, fRetracement, m_LineParms, //5
									  line_type_study_retracement, pnRetracementId++);
			}

			if(nLevel == 0) {
				// restore fRetracement from temp variable
				fRetracement = fFirstProjection;
			}

		}
	}

	iTrendlineMgr->UpdateTrendLines();
	iTrendlineMgr->Release();

	return NOERROR;
}


HRESULT CTDTrendFactor::ConvertBuf(void * pBigBuf)
{		
	CTDRetracement::ConvertBuf(pBigBuf);
	StudyMessagePtr pBuf = (StudyMessagePtr)pBigBuf;

	pBuf->nProjectionPrice = (enumPriceType)ntohl(pBuf->nProjectionPrice);
	pBuf->nMaxLookback = ntohl(pBuf->nMaxLookback);

	ConvertLineParameters(pBuf->line_params, 1);

	pBuf->nVersion = ntohl(pBuf->nVersion);
	
	int i;
	for(i=0;i<9;i++) {
		pBuf->bShowLevel[i] = ntohl(pBuf->bShowLevel[i]);
	}

	m_bSaveTrendline = 0;
	m_bDirection = pBuf->bDirection;
	m_nProjectionPrice = pBuf->nProjectionPrice;
	m_nMaxLookback = pBuf->nMaxLookback;
	m_bQualifier1 = pBuf->bQualifier1;
	m_bQualifier2 = pBuf->bQualifier2;
	m_bQualifier3 = pBuf->bQualifier3;
	m_bCancel1 = pBuf->bCancel1;
	m_bCancel2 = pBuf->bCancel2;
	m_bCancel3 = pBuf->bCancel3;
	m_bDisplayProjections = pBuf->bDisplayProjections;
	m_bDisplayHalfLevels = pBuf->bDisplayHalfLevels;

	m_strButtonText = pBuf->zButtonText;
	m_strErrorText = pBuf->zErrorText;

	memcpy(m_LineParms, pBuf->line_params, sizeof(m_LineParms));

	m_nVersion = pBuf->nVersion;
	if(m_nVersion == 0) {
		for(i=0;i<9;i++) {
			m_bShowLevel[i] = 1;
		}
	} else {
		m_bDisplayHalfLevels = TRUE;
		for(i=0;i<9;i++) {
			m_bShowLevel[i] = pBuf->bShowLevel[i];
		}
	}

	return NOERROR;
}
