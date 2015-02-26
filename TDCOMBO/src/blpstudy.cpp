// BLPStudy.cpp : implementation file
//
// Copyright (c) Bloomberg L.P. $Date: 2003/01/24 18:08:27 $
//
// $Header: /home/cvs2/external/blpstudy.cpp,v 1.13 2003/01/24 18:08:27 jkreuter Exp $
//

#include "stdafx.h"
#include  <math.h>
#include <stdarg.h>
#include "trace.h"
#define MAXARGS     31

#define _BLPSTUDY_

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "demark studies.h"
#include "blpstudy.h"

/////////////////////////////////////////////////////////////////////////////
// CBLPStudy
// Wrappers for IStudyData
int CBLPStudy::GetStudyButtonId()
	{
		ASSERT(pSD != NULL);
		int nButtonId;
		HRESULT hr = pSD->get_StudyButtonId(&nButtonId);
		ASSERT(SUCCEEDED(hr));
		return nButtonId;
	}

	float *CBLPStudy::GetStudyDataPoints(unsigned short index)
	{
		ASSERT(pSD != NULL);
		float **pDataPoints;
		HRESULT hr = pSD->get_StudyDataPoints(&pDataPoints);
		ASSERT(SUCCEEDED(hr));
		return pDataPoints[index];
	}



/////////////////////////////////////////////////////////////////////////////
// CBLPStudy Helpers


/*
 * Author: Yusong Li
 * Purpose: Getting any Line on any Chart of an input study. It is useful for
 * the super study to get the data lines from any child study. It does require
 * that the super study has certain knowledge of the child study, the specific
 * ChartIndex and LineIndex, which indicates the coupling issue, but it appears
 * to be the most flexible solution we can have under our current
 * implementation. This problem won't get resolved until we can have some kind
 * of ID to indentify the study lines instead of using the Index.
 *
 * Input: pStudy - CBLPStudy, a CBLPStudy instance.
 *        nChartIndex - int, the index of the chart you want to query.
 *        nLineIndex - int, the index of the chart you want to query.
 *
 * Return: float pointer to the actually data lines on the
 * chart. set_StudyDataLine is supposed to be called inside pStudy before
 * calling this function. The nChartIndex and nLineIndex is supposed to be
 * valid. If the preconditions are not met, NULL pointer will be returned.
 */
float* CBLPStudy::GetStudyChartLine(IBLPStudy *iStudy, int nChartIndex,
									int nLineIndex)
{
    HRESULT hr;
    CComPtr<IChartList> iList;
    hr = iStudy->QueryInterface(IID_IChartList, (LPVOID *)&iList);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return NULL;

    CComPtr<IChart> iChart;

    hr = iList->get_Chart(nChartIndex, &iChart);
    if (FAILED(hr))
        return NULL;
                
    CComPtr<IStudyData> iSD;

    hr = iChart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return NULL;

    float **pDataPoints;

    hr = iSD->get_StudyDataPoints(&pDataPoints);

    ASSERT(SUCCEEDED(hr));
    if (FAILED(hr))
        return NULL;
                
    return pDataPoints[nLineIndex];
}

int CBLPStudy::IsInvalid(float val) {

	if(val==HOLEF || val==INTERPOLATEF) return 1;
	return 0;
}


int CBLPStudy::IsInvalid(float v1,float v2,float v3,float v4) {
	
	if(IsInvalid(v1)) return 1;
	if(IsInvalid(v2)) return 1;
	if(IsInvalid(v3)) return 1;
	if(IsInvalid(v4)) return 1;
	return 0;
}

BOOL CBLPStudy::IsInvalid(float pfData1[], float pfData2[], float pfData3[], float pfData4[], long index)
{
	return IsInvalid(pfData1[index], pfData2[index], pfData3[index], pfData4[index]);
}


void CBLPStudy::GetDataRange(int nNumpoints, float pfData[], float * pfMin, float * pfMax) {
	int i=0,start;
	
	while(IsInvalid(pfData[i]) && i<nNumpoints) i++;
	if(i>=nNumpoints) return;

	start=i;
	*pfMin=*pfMax=pfData[start];

	for(i=start;i<nNumpoints;i++) {
		if(IsInvalid(pfData[i])) continue;
		if(pfData[i]<*pfMin) *pfMin=pfData[i];
		else if(pfData[i]>*pfMax) *pfMax=pfData[i];
	}

}

//Refer to prototype in blpstudy.h
long CBLPStudy::GetBarOfMax(float pfData[], long nStart, long nEnd)
{
    ASSERT(nStart >= 0 && nEnd >= nStart);
    ASSERT(nEnd <= m_nNumPoints - 1);

    long i = nStart, nBarMax;
    while (IsInvalid(pfData[i]) && i <= nEnd)
	++i;

    if (i > nEnd)
    {
	nBarMax = -1;
    }
    else
    {
	nBarMax = i;
	++i;
	
	while (i <= nEnd)
	{
	    float fMax = pfData[nBarMax];
	    if (!IsInvalid(pfData[i]))
	    {
		nBarMax = (fMax >= pfData[i]) ? nBarMax : i;
	    }
		++i;
	    
	}
    }
    
    return nBarMax;
}

//Refer to prototype in blpstudy.h
long CBLPStudy::GetBarOfMin(float pfData[], long nStart, long nEnd)
{
    ASSERT(nStart >= 0 && nEnd >= nStart);
    ASSERT(nEnd <= m_nNumPoints - 1);

    long i = nStart, nBarMin;
    while (IsInvalid(pfData[i]) && i <= nEnd)
	++i;

    if (i > nEnd)
    {
	nBarMin = -1;
    }
    else
    {
	nBarMin = i;
	++i;
	
	while (i <= nEnd)
	{
	    float fMin = pfData[nBarMin];
	    if (!IsInvalid(pfData[i]))
	    {
		nBarMin = (fMin <= pfData[i]) ? nBarMin : i;
	    }
		++i;	
	}
    }
    
    return nBarMin;    
}

void CBLPStudy::ScreenForMissing(float **ppfData,
				 long nNumLines,
				 long nNumPoints,
				 long *pnBadIndex,
				 long *pnBadNumPoints)
{
	    long nPoint;
	    long nBadPoint = 0;
	    long nGoodPoint = 0;

	    for (nPoint = 0; nPoint < nNumPoints; ++nPoint)
	    {
		long nLine;
		bool bGood = true;

		for (nLine = 0; nLine < nNumLines && bGood; ++nLine)
		{
		    if (IsInvalid(ppfData[nLine][nPoint]))
			bGood = false;
		}

		if (bGood)
		{
		    for (nLine = 0; nLine < nNumLines; ++nLine)
			ppfData[nLine][nGoodPoint] = ppfData[nLine][nPoint];
		    ++nGoodPoint;
		}
		else
		{
		    pnBadIndex[nBadPoint] = nPoint;
		    ++nBadPoint;
		}
	    }

	    *pnBadNumPoints = nBadPoint;
}


void CBLPStudy::MergeGoodValuesWithMissing(float **pComputedData,
					   long nNumLines,
					   long nNumPoints,
					   long *pnBadIndex,
					   long nBadNumPoints)
{
	    long nLine = 0;
	    for (nLine = 0; nLine < nNumLines; ++nLine)
		MergeGoodValuesWithMissing(pComputedData[nLine], nNumPoints, pnBadIndex, nBadNumPoints);
    
}


void CBLPStudy::MergeGoodValuesWithMissing(float *pfComputedData,
					   long nNumPoints,
					   long *pnBadIndex,
					   long nBadNumPoints)
{
	    if (nBadNumPoints == 0)
		return;

	    long nPoint;
	    long nGoodPoint;
	    long nBadPoint;

	    nPoint = nNumPoints - 1;
	    nGoodPoint = nNumPoints - nBadNumPoints - 1;
	    for (nBadPoint = nBadNumPoints - 1; nBadPoint >= 0; --nBadPoint)
	    {
		long nBadPointIndex = pnBadIndex[nBadPoint];
	
		for (; nPoint > nBadPointIndex ; --nPoint, --nGoodPoint)
		    pfComputedData[nPoint] = pfComputedData[nGoodPoint];

		pfComputedData[nPoint] = HOLEF;
		--nPoint;
	    }
}

void CBLPStudy::MergeGoodValuesWithMissing(int *pnComputedData,
					   long nNumPoints,
					   long *pnBadIndex,
					   long nBadNumPoints)
{
	    if (nBadNumPoints == 0)
		return;

	    long nPoint;
	    long nGoodPoint;
	    long nBadPoint;

	    nPoint = nNumPoints - 1;
	    nGoodPoint = nNumPoints - nBadNumPoints - 1;
	    for (nBadPoint = nBadNumPoints - 1; nBadPoint >= 0; --nBadPoint)
	    {
		long nBadPointIndex = pnBadIndex[nBadPoint];
	
		for (; nPoint > nBadPointIndex ; --nPoint, --nGoodPoint)
		    pnComputedData[nPoint] = pnComputedData[nGoodPoint];

		pnComputedData[nPoint] = 0;
		--nPoint;
	    }
}
	
long CBLPStudy::GetMapedIndex(long nNumPoints,
				  long *pnBadIndex,
			      long nBadNumPoints,
			      long nGoodPointIndex)
{

		if (nBadNumPoints + nGoodPointIndex + 1 > nNumPoints ||  nBadNumPoints >= nNumPoints)
			return -1;

		long nBadPointIndex;
		long nCount = 0;
		long nLastBadPoint = -1;

	    for (nBadPointIndex = 0; nBadPointIndex < nBadNumPoints; ++nBadPointIndex)
	    {
			long nNumGoodPoints = pnBadIndex[nBadPointIndex] - nLastBadPoint - 1;

			if (nCount + nNumGoodPoints >= nGoodPointIndex + 1)
				break;
		
			nCount = nCount + nNumGoodPoints;
			nLastBadPoint = pnBadIndex[nBadPointIndex];
	    }

	    long nLeft = (nGoodPointIndex + 1) - nCount;

	    return nLastBadPoint + nLeft;
}

#if 0
void CBLPStudy::Normalize(int nCount, float pfData[], float fFactor)
{
	float fHigh = HOLEF, fLow = HOLEF;
        int i;

	for (i = 0; i < nCount; i++) {
		if (pfData[i] != HOLEF) {
			if ( (pfData[i] > fHigh) || (fHigh == HOLEF) )
				fHigh = pfData[i];
			if ( (pfData[i] < fLow) || (fLow == HOLEF) )
				fLow = pfData[i];
			}
		}

	float fRatio = fFactor / (fHigh - fLow);
	for (i = 0; i < nCount; i++) {
		if (pfData[i] != HOLEF) {
			pfData[i] -= fLow;
			pfData[i] *= fRatio;
			}
		}
}
#endif
