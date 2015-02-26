#include <math.h>
#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
//#include "polylines.h"
#ifdef COMPILER_DOES_VECTORS
#include <algorithm>
#endif

HRESULT CTDMav1ex::ConvertBuf(void * pBigBuf)
{
	CBLPStudy::ConvertBuf(pBigBuf);
	StudyMessagePtr pHead = (StudyMessagePtr)pBigBuf;
	
	int i = 0;

    pHead->num_study = ntohs(pHead->num_study);
	m_nNumStudy = pHead->num_study;

	pHead->toggle_lookback = ntohs(pHead->toggle_lookback);
	m_nToggleLookback = pHead->toggle_lookback;

	for(i = 0; i < 2; i++){
	   pHead->period[i] = ntohs(pHead->period[i]);
	   pHead->high_factor[i] = ntohs(pHead->high_factor[i]);
	   pHead->low_factor[i] = ntohs(pHead->low_factor[i]);
	   pHead->lookback[i] = ntohs(pHead->lookback[i]);
	   pHead->bars_to_extend[i] = ntohs(pHead->bars_to_extend[i]);
	   pHead->projector[i] = ntohs(pHead->projector[i]);
	   pHead->toggle_switch[i][0] = ntohs(pHead->toggle_switch[i][0]);
	   pHead->toggle_switch[i][1] = ntohs(pHead->toggle_switch[i][1]);

	   ConvertLineParameters(pHead->line_params[i], 2);
	
	   if( pHead->period[i] != 0 ) 
		 m_nPeriod[i] = pHead->period[i];		
	   if( pHead->lookback[i] != 0 )
		 m_nLookback[i] = pHead->lookback[i];
	   if( pHead->bars_to_extend[i] != 0 )
		 m_nBarsToExtend[i] = pHead->bars_to_extend[i];
	   if( pHead->projector[i] != 0 )
		 m_nProjector[i] = pHead->projector[i];
	
	   m_nToggleSwitch[i][0] = pHead->toggle_switch[i][0];
	   m_nToggleSwitch[i][1] = pHead->toggle_switch[i][1];

	   m_nType[i] = (enum tagEnumMavg)pHead->type[i];
	   m_fHighFactor[i] = pHead->high_factor[i] / 100.0f; // Multiple of the high
	   m_fLowFactor[i] = pHead->low_factor[i] / 100.0f; // Multiple of the low

 	   memset(m_zHigherId[i], 0, FIELD_LEN + 1);
	   strncpy(m_zHigherId[i], pHead->higher_id[i], FIELD_LEN);
	   memset(m_zLowerId[i], 0, FIELD_LEN + 1);
	   strncpy(m_zLowerId[i], pHead->lower_id[i], FIELD_LEN);
    }

	m_pLineParameters = &pHead->line_params[0][0];

	return NOERROR;
}



HRESULT CTDMav1ex::CreateStudy(IBLPGraphStudyList * pGraph)
{

	ChartLineParameters LineParms[8] = 
	{
	{m_pLineParameters[0].line_width, XRT_LPAT_SOLID, m_pLineParameters[0].line_color},	
	{m_pLineParameters[1].line_width, XRT_LPAT_SOLID, m_pLineParameters[1].line_color},
	{1, XRT_LPAT_NONE, blgGreen},
	{1, XRT_LPAT_NONE, blgRed},
    {m_pLineParameters[2].line_width, XRT_LPAT_SOLID, m_pLineParameters[2].line_color},	
	{m_pLineParameters[3].line_width, XRT_LPAT_SOLID, m_pLineParameters[3].line_color}
	};

	ChartPointParameters PointParms[8] = 
	{
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_UPARROW, 1, blgGreen},
	{XRT_POINT_DOWNARROW, 1, blgRed},
	{XRT_POINT_NONE,0,0},
	{XRT_POINT_NONE,0,0}
	};

	int numstudies = 0;

	if( m_nNumStudy == 2 )
	  numstudies = 6;
	else
	  numstudies = 4;

	HRESULT rc = AddStudy(pGraph, numstudies, numstudies, LineParms, PointParms);

	if (FAILED(rc))
		return rc;

	CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
	CComPtr<IChart> iChart;
	if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
		AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, m_strButtonText, this);
		}

	return rc;
}


HRESULT CTDMav1ex::DoStudy(IBLPGraphData *pGraph)
{

	int i = 0, j = 0;
	int num_graphs = 4;
	long N;

	float *pfTDMAVG1High1 = NULL, *pfTDMAVG1Low1 = NULL, *pfTDMAVG1High2 = NULL, *pfTDMAVG1Low2 = NULL;

	HRESULT result;

	pGraph->get_NumPoints(&N); //m_GraphData.number_of_points;

	if( m_nNumStudy == 2 ){
	  pfTDMAVG1High1 = new float[N];
	  pfTDMAVG1Low1 = new float[N];
	  pfTDMAVG1High2 = new float[N];
	  pfTDMAVG1Low2 = new float[N];
	}
	
	for(i = 0; i < m_nNumStudy; i++){
	   if( i == 0 )
         result = GetTDmav1exStudy(pGraph, i, num_graphs, pfTDMAVG1High1, pfTDMAVG1Low1);
	   else
		 result = GetTDmav1exStudy(pGraph, i, num_graphs, pfTDMAVG1High2, pfTDMAVG1Low2);

	   if( result != NOERROR )
		 return result;
	}

	if( m_nNumStudy == 2 ){
	  Get2GetTDmav1exStudy(pGraph, pfTDMAVG1High1, pfTDMAVG1Low1, pfTDMAVG1High2, pfTDMAVG1Low2);

	  delete[] pfTDMAVG1High1;
	  delete[] pfTDMAVG1Low1;
	  delete[] pfTDMAVG1High2;
	  delete[] pfTDMAVG1Low2;
	}

	return NOERROR;//CBLPStudy::DoStudy(pGraph);
}

HRESULT CTDMav1ex::Get2GetTDmav1exStudy(IBLPGraphData *pGraph, float *pfTDMAVG1High1, float *pfTDMAVG1Low1, 
		                  float *pfTDMAVG1High2, float *pfTDMAVG1Low2)
{

	int i = 0;
	long N;
	float temp;
	float *pfClose, *pfOpen;
	float *pfTDMAVG1Low_tmp; 
	float *pfTDMAVG1High_tmp;
	float *pfHigh_price, *pfLow_price;
	float max = 0, min = 0;
	
	pGraph->get_NumPoints(&N); //m_GraphData.number_of_points;

	pGraph->get_CloseData(N, &pfClose);
	pGraph->get_OpenData(N, &pfOpen);

	pGraph->get_HighData(N, &pfHigh_price);
	pGraph->get_LowData(N, &pfLow_price);

	if ( !(pfTDMAVG1High_tmp = GetStudyDataPoints(2)) )
		return E_HANDLE;
	
	if ( !(pfTDMAVG1Low_tmp = GetStudyDataPoints(3)) )
		return E_HANDLE;

	GetDataRange(N,pfHigh_price,&temp,&max);
	GetDataRange(N,pfLow_price,&min,&temp);

	temp=max-min;
	temp=temp*(float)0.025; // offset for where to place the arrow

//If both high moving averages are less than the close and the next days open is greater
//than the close and the next days high price is greater than the open then put an arrow.
	if( m_nToggleSwitch[1][0] == 1 ){
	  pfTDMAVG1High_tmp[0]= HOLEF;
	  
	  for(i = 0; i < N - 1; i++){
	     pfTDMAVG1High_tmp[i+1]= HOLEF;

         if( pfClose[i] > pfTDMAVG1High1[i] && pfClose[i] > pfTDMAVG1High2[i] &&
		     pfOpen[i+1] > pfClose[i] &&
			 pfHigh_price[i+1] > pfOpen[i+1] &&
		     !IsInvalid(pfTDMAVG1High1[i]) && !IsInvalid(pfTDMAVG1High2[i]) &&
			 !IsInvalid(pfLow_price[i+1]) )
		   pfTDMAVG1High_tmp[i+1] = pfLow_price[i+1] - temp;
		   
	  }
	}
	else{
      for(i = 0; i < N; i++)   
	     pfTDMAVG1High_tmp[i] = HOLEF;
	}

	if( m_nToggleSwitch[1][1] == 1 ){

	  pfTDMAVG1Low_tmp[0]= HOLEF;
	  
	  for(i = 0; i < N - 1; i++){
	     pfTDMAVG1Low_tmp[i+1]= HOLEF;

         if( pfClose[i] < pfTDMAVG1Low1[i] && pfClose[i] < pfTDMAVG1Low2[i] &&
		     pfOpen[i+1] < pfClose[i] &&
			 pfLow_price[i+1] < pfOpen[i+1] &&
		     !IsInvalid(pfTDMAVG1Low1[i]) && !IsInvalid(pfTDMAVG1Low2[i]) &&
			 !IsInvalid(pfHigh_price[i+1]) )
		   pfTDMAVG1Low_tmp[i+1] = pfHigh_price[i+1] + temp;
	  }
	}
	else{
      for(i = 0; i < N; i++)   
	     pfTDMAVG1Low_tmp[i] = HOLEF;
	}

//Only display up or down arrows for a specified number of bars back 
//from the end of the graph. 
	if( m_nToggleLookback > 0 )
	    for(i = 0; i < N - m_nToggleLookback; i++)   
	       pfTDMAVG1High_tmp[i] = HOLEF;

	if( m_nToggleLookback > 0 )
	    for(i = 0; i < N - m_nToggleLookback; i++)   
	       pfTDMAVG1Low_tmp[i] = HOLEF;

	return NOERROR;
}

HRESULT CTDMav1ex::GetTDmav1exStudy(IBLPGraphData *pGraph, int snum, int num_graphs,
									float *pfTDMAVG1High_hold, float *pfTDMAVG1Low_hold)
{

  	// Calculate both 5-day moving averages on highs and lows.
	// For each day, if the current high (low) is higher (lower) than the 
	// last 12 days' highs (lows), the study value is the high (low) for that 
	// day and the next 3 days.
	float	*pfHigh, *pfLow, *pfClose, *pfOpen, *pfTDMAVG1High, *pfTDMAVG1Low;
	float   *pfTDMAVG1High_tmp, *pfTDMAVG1Low_tmp, temp, max, min;
	float   *pfTDMAVG1AboveHigh, *pfTDMAVG1BelowLow;
	float   *pfHigh_price, *pfLow_price;
	BOOL	bUseHigh, bUseLow, *pbUseHigh, *pbUseLow;//, bTDMAVG1Smoothing;
	long	N, i, j;
	short   num_projector_bars;
	unsigned short graph_line;

	pGraph->get_NumPoints(&N); //m_GraphData.number_of_points;

	pGraph->get_CloseData(N, &pfClose);
	pGraph->get_OpenData(N, &pfOpen);

	pGraph->get_HighData(N, &pfHigh_price);
	pGraph->get_LowData(N, &pfLow_price);
	
	pfHigh = GetStudyCalcRoute(pGraph, m_zHigherId[snum]);
	ASSERT(pfHigh != NULL);
	if (pfHigh == NULL)
		return E_INVALIDARG;

	pfLow = GetStudyCalcRoute(pGraph, m_zLowerId[snum]);

	ASSERT(pfLow != NULL);
	if (pfLow == NULL)
		return E_INVALIDARG;

	if( m_nNumStudy == 1 ){
	  graph_line = snum*4;
	  if ( !(pfTDMAVG1High = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;
	
	  graph_line = snum*4 + 1;
	  if ( !(pfTDMAVG1Low = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;

	  graph_line = snum*4 + 2;
	  if ( !(pfTDMAVG1AboveHigh = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;
	
	  graph_line = snum*4 + 3;
	  if ( !(pfTDMAVG1BelowLow = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;
	}
	else{
	  graph_line = snum*4;
	  if ( !(pfTDMAVG1High = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;
	
	  graph_line = snum*4 + 1;
	  if ( !(pfTDMAVG1Low = GetStudyDataPoints(graph_line)) )
		 return E_HANDLE;

	  pfTDMAVG1AboveHigh = new float[N];
	  pfTDMAVG1BelowLow = new float[N];
	}

	if ( m_nPeriod[snum] < 1 )
	   return E_HANDLE;

	switch (m_nType[snum]) { // Take the moving average specfied by the BIG
	  case none:
		  for (i = 0; i < N; i++) {
			pfTDMAVG1Low[i] = pfLow[i];
			pfTDMAVG1High[i] = pfHigh[i];
			}
		  break;

	  case exponential:
		pGraph->ExponentialAverage(m_nPeriod[snum], N, pfHigh, &pfTDMAVG1High);
		pGraph->ExponentialAverage(m_nPeriod[snum], N, pfLow, &pfTDMAVG1Low);
		break;

	  case weighted:
		for (i = m_nPeriod[snum] - 1; i < N; i++) {
			int nBarsBack;
			pfTDMAVG1Low[i] = 0;
			pfTDMAVG1High[i] = 0;
			long nLowSumWeights = 0, nHighSumWeights = 0;
			for (nBarsBack = 0; nBarsBack < m_nPeriod[snum]; nBarsBack++) {
				if (pfLow[i - nBarsBack] != HOLEF) {
					pfTDMAVG1Low[i] += pfLow[i - nBarsBack] * (m_nPeriod[snum] - nBarsBack);
					nLowSumWeights += (m_nPeriod[snum] - nBarsBack);
					}
				if (pfHigh[nBarsBack] != HOLEF) {
					pfTDMAVG1High[i] += pfHigh[i - nBarsBack] * (m_nPeriod[snum] - nBarsBack);
					nHighSumWeights += (m_nPeriod[snum] - nBarsBack);
					}
				}

			pfTDMAVG1Low[i] /= nLowSumWeights;
			pfTDMAVG1High[i] /= nHighSumWeights;
			}
		break;

	  case simple:
	  default:
		pGraph->MovingAverage(m_nPeriod[snum], N, pfHigh, &pfTDMAVG1High);
		pGraph->MovingAverage(m_nPeriod[snum], N, pfLow, &pfTDMAVG1Low);
		break;
	}


	pbUseHigh = new BOOL[N];
	pbUseLow = new BOOL[N];

	for (i = 0; i < m_nPeriod[snum] + m_nLookback[snum]; i++) {
		pfTDMAVG1High[i] = HOLEF;
		pfTDMAVG1Low[i] = HOLEF;
		}
	
	for (i = 0; i < N; i++) {
		pbUseHigh[i] = FALSE;
		pbUseLow[i] = FALSE;
		}

//Reset pfHigh and pfLow for the lookback comparisons
//	pfHigh = GetStudyDataLine(pGraph, High);
//	pfLow = GetStudyDataLine(pGraph, Low);

	for ( i = m_nPeriod[snum] + m_nLookback[snum]; i < N; i++ )
	{
		// If the current high is lower than the last Lookback highs,
		// use the mavg of the highs. If the current low is higher than
		// the last Lookback lows, use the mavg of the lows.
		bUseHigh = bUseLow = TRUE;
		for ( j = i - m_nLookback[snum]; j < i; j++ ) {
			bUseHigh = bUseHigh && ((CBLPPrice)pfHigh[i] < pfHigh[j]);
			bUseLow = bUseLow && ((CBLPPrice)pfLow[i] > pfLow[j]);
			// Speed up slightly - once both have failed, we can skip.
			if ( !bUseHigh && !bUseLow )
				break;
			}

		if ( bUseHigh ) 
			// set next TDMAVG1Projection study points to MAVGWidth-moving average of pfHigh
			for ( j = 0; j < m_nBarsToExtend[snum]; j++ ) {
				if ((i + j) < N)
					pbUseHigh[i + j] = TRUE;
				}

		if (pbUseHigh[i])
			pfTDMAVG1High[i] *= m_fHighFactor[snum];
		else
			pfTDMAVG1High[i] = HOLEF;

		if ( bUseLow )
			for ( j = 0; j < m_nBarsToExtend[snum]; j++ ) {
				if ((i + j) < N)
					pbUseLow[i + j] = TRUE;
				}

		if (pbUseLow[i])
			pfTDMAVG1Low[i] *= m_fLowFactor[snum];
		else
			pfTDMAVG1Low[i] = HOLEF;

	}

    pfTDMAVG1Low_tmp = new float[N];
	pfTDMAVG1High_tmp = new float[N];

	if( m_nProjector[snum] > 0 ){
      num_projector_bars = (short) ( (m_nProjector[snum]/100.0) * m_nPeriod[snum] + .5);

	  for(i = 0; i < num_projector_bars; i++){
         pfTDMAVG1Low_tmp[i] = HOLEF;
	     pfTDMAVG1High_tmp[i] = HOLEF;
	  }
	  for(i = 0; i < N - num_projector_bars; i++){
         pfTDMAVG1Low_tmp[i + num_projector_bars] = pfTDMAVG1Low[i];
	     pfTDMAVG1High_tmp[i + num_projector_bars] = pfTDMAVG1High[i];
	  }
      for(i = 0; i < N; i++){
         pfTDMAVG1Low[i] = pfTDMAVG1Low_tmp[i];
	     pfTDMAVG1High[i] = pfTDMAVG1High_tmp[i];
	  }
	}

	GetDataRange(N,pfHigh_price,&temp,&max);
	GetDataRange(N,pfLow_price,&min,&temp);

	temp=max-min;
	temp=temp*(float)0.025; // offset for where to place the arrow

	if( m_nToggleSwitch[snum][0] == 1 ){
	  pfTDMAVG1AboveHigh[0]= HOLEF;
	  
	  for(i = 0; i < N - 1; i++){
	     pfTDMAVG1AboveHigh[i+1]= HOLEF;

         if( pfClose[i] > pfTDMAVG1High[i] && pfOpen[i+1] > pfClose[i] 
			 && pfHigh_price[i+1] > pfOpen[i+1]
			 && !IsInvalid(pfLow_price[i+1]) && !IsInvalid(pfTDMAVG1High[i]) )
		   pfTDMAVG1AboveHigh[i+1] = pfLow_price[i+1] - temp;
	  }

	  if( m_nToggleLookback > 0 )
	    for(i = 0; i < N - m_nToggleLookback; i++)   
	       pfTDMAVG1AboveHigh[i] = HOLEF;
	}
	else{
      for(i = 0; i < N; i++)
	     pfTDMAVG1AboveHigh[i]= HOLEF;
	}

	if( m_nToggleSwitch[snum][1] == 1 ){
	  pfTDMAVG1BelowLow[0] = HOLEF;
	  
	  for(i = 0; i < N - 1; i++){
	     pfTDMAVG1BelowLow[i+1] = HOLEF;

	     if( pfClose[i] < pfTDMAVG1Low[i] && pfOpen[i+1] < pfClose[i] 
			 && pfLow_price[i+1] < pfOpen[i+1]
			 && !IsInvalid(pfHigh_price[i+1]) && !IsInvalid(pfTDMAVG1Low[i]) )
           pfTDMAVG1BelowLow[i+1] = pfHigh_price[i+1] + temp;
	  }
	  
	  if( m_nToggleLookback > 0 )
	    for(i = 0; i < N - m_nToggleLookback; i++)   
	       pfTDMAVG1BelowLow[i] = HOLEF;
	}
	else{
      for(i = 0; i < N; i++)   
	     pfTDMAVG1BelowLow[i] = HOLEF;
	}

	if( m_nNumStudy == 2 )
	  for(i = 0; i < N; i++){
	     pfTDMAVG1High_hold[i] = pfTDMAVG1High[i];
		 pfTDMAVG1Low_hold[i] = pfTDMAVG1Low[i];
	  }

	delete[] pbUseHigh;
	delete[] pbUseLow;

	delete[] pfTDMAVG1Low_tmp;
	delete[] pfTDMAVG1High_tmp;

	if( m_nNumStudy == 2 ){
	  delete[] pfTDMAVG1AboveHigh;
	  delete[] pfTDMAVG1BelowLow;
	}

	return NOERROR;
}
