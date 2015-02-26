#include <stdafx.h>
#include "blpstudydlltd.h"
#include "blpstudy.h"
#include "demarkstudies.h"
#include "fstring.h"

/*** TD D-Wave */

CTDDWave::CTDDWave()
{
	int i;
	for (i=0; i<MAX_TRENDLINES; i++)
		m_pTrendLineIds[i] = 0;

	//Initialising to default values. 
	//We don't need this. But, just to be on the safe side.

	strcpy(m_wave_direction,"U");
	strcpy(m_condition[0],"X");
	strcpy(m_condition[1],"X");
	
	m_level[0] = 21;
	m_level[1] =  8;
	m_level[2] =  5;
	m_level[3] = 13;
	m_level[4] =  8;
	m_level[5] = 21;
	m_level[6] = 13;
	m_level[7] =  8;
	m_level[8] = 21;

	for (i=0; i<DWAVE_MAX_WAVES; i++)
		m_color[i] = 13;	

	for (i=0; i<DWAVE_MAX_LINES; i++) 
	{	
		m_on[i] = ' ';
		m_from[i] = '2';
		m_to[i] = '3';
		m_times[i] = (float)1.618;
		m_dir[i] = 'U';
		m_at[i] = '4';
		m_stop[i] = '5';
		m_pcolor[i] = 13; 
	}
	
	strcpy(m_min_waves,"X");
	strcpy(m_wave_height,"W");
	strcpy(m_true_vals,"Y");
}


HRESULT CTDDWave::ConvertBuf(void * pBuf)
{
	if(pBuf==NULL)
		return E_POINTER;

	HRESULT hr=NOERROR;
	hr = CBLPStudy::ConvertBuf(pBuf);
	if (FAILED(hr))
		return hr;


	StudyMessagePtr pData = (StudyMessagePtr)pBuf;

	if(pData->wave_direction[0]!='D')
		pData->wave_direction[0]='U';
	m_wave_direction[0] = pData->wave_direction[0];

	if(pData->condition[0][0]!=' ')
		pData->condition[0][0] = 'X';
	m_condition[0][0] = pData->condition[0][0];

	if(pData->condition[1][0]!=' ')
		pData->condition[1][0] = 'X';
	m_condition[1][0] = pData->condition[1][0];
	
	if (pData->level[0] < 2) 
		pData->level[0] = 21;
	if (pData->level[1] < 2) 
		pData->level[1] = 8;
	if (pData->level[2] < 2) 
		pData->level[2] = 5;
	if (pData->level[3] < 2) 
		pData->level[3] = 13;
	if (pData->level[4] < 2) 
		pData->level[4] = 8;
	if (pData->level[5] < 2) 
		pData->level[5] = 21;
	if (pData->level[6] < 2) 
		pData->level[6] = 13;
	if (pData->level[7] < 2) 
		pData->level[7] = 8;
	if (pData->level[8] < 2) 
		pData->level[8] = 21;

	int i;

	for (i=0; i<DWAVE_MAX_WAVES; ++i) {
		pData->level[i] = ntohs(pData->level[i]);
		m_level[i] = pData->level[i];
	
		pData->color[i] = ntohs(pData->color[i]);
		m_color[i] = mapidtocolor(pData->color[i]);
	}
	
	for (i=0; i<DWAVE_MAX_LINES; i++)
	{
		if (pData->on[i] != 'X') 
			pData->on[i] = ' ';

		if (pData->from[i] != '0' && pData->from[i] != '1' && 
			pData->from[i] != '2' && pData->from[i] != '3' && 
			pData->from[i] != '4' && pData->from[i] != '5' &&
			pData->from[i] != 'A' && pData->from[i] != 'B' &&
			pData->from[i] != 'C') 
			pData->from[i] = '2';

	    if (pData->to[i] != '0' && pData->to[i] != '1' && 
			pData->to[i] != '2' && pData->to[i] != '3' &&
			pData->to[i] != '4' && pData->to[i] != '5' &&
			pData->to[i] != 'A' && pData->to[i] != 'B' &&
			pData->to[i] != 'C') 
			pData->to[i] = '3';

	    if (pData->dir[i] != 'D') 
			pData->dir[i] = 'U';

	    if (pData->at[i] != '0' && pData->at[i] != '1' &&
			pData->at[i] != '2' && pData->at[i] != '3' &&
			pData->at[i] != '4' && pData->at[i] != '5' &&
			pData->at[i] != 'A' && pData->at[i] != 'B' &&
			pData->at[i] != 'C') 
			pData->at[i] = '4';

	    if (pData->stop[i] != '0' && pData->stop[i] != '1' && 
			pData->stop[i] != '2' && pData->stop[i] != '3' &&
			pData->stop[i] != '4' && pData->stop[i] != '5' &&
			pData->stop[i] != 'A' && pData->stop[i] != 'B' &&
			pData->stop[i] != 'C') 
			pData->stop[i] = '5';
	
		pData->pcolor[i] = ntohs(pData->pcolor[i]);
		m_pcolor[i] = mapidtocolor(pData->pcolor[i]);
		
		if(pData->times[i]<=0.0)
			pData->times[i]=(float)1.618;
		m_times[i] = pData->times[i];
	}

	memcpy(m_on,pData->on,DWAVE_MAX_LINES);
	memcpy(m_from,pData->from,DWAVE_MAX_LINES);
	memcpy(m_to,pData->to,DWAVE_MAX_LINES);
	memcpy(m_dir,pData->dir,DWAVE_MAX_LINES);
	memcpy(m_at,pData->at,DWAVE_MAX_LINES);
	memcpy(m_stop,pData->stop,DWAVE_MAX_LINES);

	if (pData->min_waves[0] != ' ')
		pData->min_waves[0] = 'X';
	m_min_waves[0] = pData->min_waves[0];

	if (pData->wave_height[0] != 'O' && pData->wave_height[0] != 'H' && 
		pData->wave_height[0] != 'L' && pData->wave_height[0] != 'C' && 
		pData->wave_height[0] != 'W') 
		pData->wave_height[0] = 'W';  
	m_wave_height[0] = pData->wave_height[0];

	if (pData->true_vals[0] != 'N')
	    pData->true_vals[0] = 'Y';
	m_true_vals[0] = pData->true_vals[0];

	return NOERROR;
}



HRESULT CTDDWave::CreateStudy(IBLPGraphStudyList * pGraph)
{
	m_pnCountdownDisplay = NULL;
	
    ChartLineParameters LineParms[] = 
	{
		{1, XRT_LPAT_SOLID, m_color[0]},
		{1, XRT_LPAT_SOLID, m_color[1]},
		{1, XRT_LPAT_SOLID, m_color[2]},
		{1, XRT_LPAT_SOLID, m_color[3]},
		{1, XRT_LPAT_SOLID, m_color[4]},
		{1, XRT_LPAT_SOLID, m_color[5]},
		{1, XRT_LPAT_SOLID, m_color[6]},
		{1, XRT_LPAT_SOLID, m_color[7]},
		{1, XRT_LPAT_SOLID, m_color[8]},
		{1, XRT_LPAT_SOLID, m_color[1]},
		{1, XRT_LPAT_SOLID, m_color[2]},
		{1, XRT_LPAT_SOLID, m_color[3]},
		{1, XRT_LPAT_SOLID, m_color[4]},
		{1, XRT_LPAT_SOLID, m_color[5]},
		{1, XRT_LPAT_SOLID, m_color[6]},
		{1, XRT_LPAT_SOLID, m_color[7]},
		{1, XRT_LPAT_SOLID, m_color[8]}
	};

//these will have to be reversed for down slope
	short firstposition, secondposition;
	if (m_wave_direction[0] == 'U') {
		firstposition = XRT_POINT_INDEX_BELOW;
		secondposition = XRT_POINT_INDEX_ABOVE;
	} else {
		firstposition = XRT_POINT_INDEX_ABOVE;
		secondposition = XRT_POINT_INDEX_BELOW;
	}
    ChartPointParameters PointParms[] =
	{
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0},
		{secondposition, 0, 0},
		{firstposition, 0, 0}
	};

	int nIndex;
	for (nIndex=0; nIndex<DWAVE_NUM_WAVES; ++nIndex) {
        PointParms[nIndex].point_style += 100 * m_nCountdownDisplayIndex;
	}
    
    HRESULT rc = AddStudy(pGraph, DWAVE_NUM_WAVES, DWAVE_NUM_WAVES, LineParms, PointParms);
    //If there is no data for this security, it will fail at this step
    if (FAILED(rc))
        return rc;

    CComPtr<IChart> iChart;
    CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
    if (pSD->QueryInterface(IID_IChart, (LPVOID *)&iChart) == S_OK) {
        AddChart(iChart);
		iGraphToolbar->AddStudyButton(0, "TD D-Wave", this);
    }
    
    return NOERROR;
}


/*------------------------------------------------------------- 
  Function Name: DoStudy
	
  Purpose:	Does the Tom Demark DWave Study. Identifies Minimum Waves, 
			Waves, Wave Projections and	draws them.

  Implementation Note(s):
			We find one minimum wave at a time iteratively.	Once a minimum wave
			is found, we identify preceding actual wave. Then we do wave relabelling 
			if any. During these steps we store all Minimum Waves and Actual Waves
			in arrays. We continue this process untill we are done with all 
			data points. Atlast, we identify/draw wave projections based on 
			the identified waves.
  
  Author:	Coll Kaverill

  Input:	Pointer IBLPGraphData object

  Output:	Draws the identified Minimum Waves, Waves & Wave Projections

  Return:	Return Code

--------------------------------------------------------------*/ 

HRESULT CTDDWave::DoStudy(IBLPGraphData *pGraph)
{
	long nNumPoints = m_nNumPoints;
	pGraph->get_NumPoints(&m_nNumPoints);
	if (m_nNumPoints < m_level[0])
		return E_HANDLE;  // not enough data points

	if ((m_pnCountdownDisplay == NULL) || (nNumPoints != m_nNumPoints)) 
	{
		if(m_pnCountdownDisplay!=NULL) 
			delete [] m_pnCountdownDisplay;
		m_pnCountdownDisplay = new int[m_nNumPoints];
		if(m_pnCountdownDisplay == NULL) 
			return E_OUTOFMEMORY;
	}
	
	//Reset the Countdown
	memset(m_pnCountdownDisplay, 0, m_nNumPoints * sizeof(int));

	//Holds raw data, Open, High, Low, Close, True High, True Low
	float *pfDataArray[6]; 
	HRESULT hr = NOERROR;
	hr = GetData(pfDataArray, pGraph);
    if (FAILED(hr))
        return hr;

	//Holds info on each wave if its going up or down
	short pbUpOrDown[DWAVE_MAX_WAVES]; 
	SetUpOrDown(pbUpOrDown);

	//Previous-Minimum-Wave: The starting point for the next (min)wave search.
	long  nPrevMinWave=m_level[0]-2;

	//Current Minimum Wave (position)
	long  nMinWave=0;
	//Current Minimum Wave Number
	short nMinWaveNumber=0;	

	//Minumum Wave positions
	long  *pnMinWaves = new long[m_nNumPoints];
	if(pnMinWaves == NULL) 
		return E_OUTOFMEMORY;

	//Minimum Wave Numbers
	short *pnMinWaveNums = new short[m_nNumPoints];
	if(pnMinWaveNums == NULL) 
		return E_OUTOFMEMORY;

	//Index of the next available slot in the above two  arrays
	long nMinWaveIndex=0;

	//Current Minimum Wave (position)
	long  nWave=0;
	//Current Minimum Wave Number
	short nWaveNumber=0;

	//Wave positions
	long  *pnWaves = new long[m_nNumPoints];
	if(pnWaves == NULL) 
		return E_OUTOFMEMORY;

	//Wave Numbers
	short *pnWaveNums = new short[m_nNumPoints];
	if(pnWaveNums == NULL) 
		return E_OUTOFMEMORY;

	//Index of the next available slot in the above two arrays
	long  nWaveIndex=0;

	TRACE("--------------------------------------------------------------\n");

	//Find next minimum wave
	while((nMinWave = FindWaveMinimum(nMinWaveNumber, nPrevMinWave, pbUpOrDown, 
					pnWaves, nWaveIndex, pfDataArray))!= DWAVE_EOD)
	{
		//Update the arrays
		pnMinWaves[nMinWaveIndex] = nMinWave;
		pnMinWaveNums[nMinWaveIndex] = nMinWaveNumber;

		TRACE("%d Min. %ld  %lf %lf\n", nMinWaveNumber, nMinWave, 
			pfDataArray[DTHIGH][nMinWave], pfDataArray[DTLOW][nMinWave]);

		if(nMinWaveNumber==0) //For Wave0, min wave is the actual wave
		{
			pnWaves[nWaveIndex] = nMinWave;
			pnWaveNums[nWaveIndex] = nMinWaveNumber; 
			nWaveIndex++;
		}
		
		if(nMinWaveIndex>1) //We have atleast two min waves, we can find an actual wave 
		{
			nWave = FindWave(nWaveNumber, nPrevMinWave, nMinWave, pbUpOrDown, 
					   pnWaves, nWaveIndex, pfDataArray);

			if(nWave<0) //Wave4 overlapped Wave1
			{
				nWaveIndex--;
				nMinWaveIndex--;
				nWaveNumber = 3;
				nMinWaveNumber = 4;
				nPrevMinWave = -nWave;
				continue;
			}
			
			//Update Arrays
			pnWaves[nWaveIndex] = nWave;
			pnWaveNums[nWaveIndex] = nWaveNumber;

			TRACE("%d %ld %lf %lf\n", nWaveNumber, nWave,
				pfDataArray[DTHIGH][nWave], pfDataArray[DTLOW][nWave]);

			long nSaveWaveIndex;

			nSaveWaveIndex = nWaveIndex;

			AdjustPrevWaves(pnWaves, pnWaveNums, nWaveIndex, pbUpOrDown, pfDataArray);
			if(nSaveWaveIndex!=nWaveIndex)
			{	
				TRACE("%d moved\n", pnWaveNums[nWaveIndex]);

				nWave = pnWaves[nWaveIndex];
				nWaveNumber = pnWaveNums[nWaveIndex];
				nMinWaveIndex = nWaveIndex+1;
				nMinWaveNumber = pnMinWaveNums[nMinWaveIndex];
				nPrevMinWave = pnWaves[nWaveIndex];
				continue;
			}

			//Do any wave relabelling. 
			//If any wave relabelling is done, the routine sets all the loop 
			//control variabels appropriately. Thus, we 'continue'.
			if(DoWaveRelabeling(nWaveNumber, nMinWaveNumber, nPrevMinWave,
								pnWaves, pnWaveNums, nWaveIndex,
							pnMinWaves, pnMinWaveNums, nMinWaveIndex,
								pbUpOrDown,	pfDataArray)==true)
			{
				TRACE("%d getting relabelled\n", nWaveNumber);
				continue;
			}
			nWaveIndex++;
		}

		//Reset all the loop control variables for the next iteration
		nPrevMinWave = nMinWave;
		nWaveNumber = nMinWaveNumber;

		nMinWaveIndex++;
		nMinWaveNumber = (nMinWaveNumber + 1)%8;
		if(nMinWaveNumber == 0) 
			nMinWaveNumber = 8;

		//  PS: We treat WaveA, WaveB, WaveC as Wave6, Wave7, Wave8 respectively
		//	    This is to facilitate programming
		//      We do put them as A, B, C when drawing on screen
	}

	AdjustLastMin(pnMinWaves, pnMinWaveNums, nMinWaveIndex, 
					pnWaves,  nWaveIndex, pbUpOrDown, pfDataArray);

	
	//Draw the found waves
	hr = NOERROR;
	hr = DrawWaves(pnMinWaves, pnMinWaveNums, nMinWaveIndex, 
				   pnWaves, pnWaveNums, nWaveIndex, pbUpOrDown, pfDataArray);

    if (FAILED(hr))
        return hr;

	//Find (and draw) wave projections
	hr = NOERROR;
	hr = FindProjections(pnWaves, pnWaveNums, nWaveIndex, pfDataArray, pGraph);
    if (FAILED(hr))
        return hr;

	delete pnMinWaves;
	delete pnMinWaveNums;
	delete pnWaves;
	delete pnWaveNums;
	
	return NOERROR;
}

//Gets Datapointers
HRESULT CTDDWave::GetData(float *pfDataArray[6], IBLPGraphData *pGraph)
{
	pfDataArray[DOPEN]  = GetStudyDataLine(pGraph, Open);
	if(pfDataArray[DOPEN]==NULL)
		return E_POINTER;

	pfDataArray[DCLOSE] = GetStudyDataLine(pGraph, Close);
	if(pfDataArray[DCLOSE]==NULL)
		return E_POINTER;

	pfDataArray[DHIGH]  = GetStudyDataLine(pGraph, High);
	if(pfDataArray[DHIGH]==NULL)
		return E_POINTER;

	pfDataArray[DLOW]   = GetStudyDataLine(pGraph, Low);
	if(pfDataArray[DLOW]==NULL)
		return E_POINTER;

	pfDataArray[DTHIGH] = GetStudyDataLine(pGraph, TrueHigh);
	if(pfDataArray[DTHIGH]==NULL)
		return E_POINTER;

	pfDataArray[DTLOW]  = GetStudyDataLine(pGraph, TrueLow);
	if(pfDataArray[DTLOW]==NULL)
		return E_POINTER;

	return NOERROR;
}

/*------------------------------------------------------------- 
  Function Name: SetUpOrDown

  Purpose:	Finds out which wave is moving up and which wave is moving down.
			Sets the input/output array appropriately
 
  Author:	Coll Kaverill
 
  Input/Output:
			Array of shorts. The array will be populated with 
			DWAVE_UPs and DWAVE_DOWNs by the end
 
  Return:	None

--------------------------------------------------------------*/ 

void CTDDWave::SetUpOrDown(short *pbUpOrDown)
{
	short bUpOrDown;

	if (m_wave_direction[0]=='D') 
		bUpOrDown=DWAVE_UP; //In an downward trend wave, Wave 0 will start upwards
	else	
		bUpOrDown=DWAVE_DOWN; // In an upward trend wave, Wave 0 will start downwards
	

	//We alternate between Up and Down
	for (short i=0;i<DWAVE_MAX_WAVES; i++)
	{
		pbUpOrDown[i] = bUpOrDown;
		
		if(bUpOrDown==DWAVE_UP)
			bUpOrDown = DWAVE_DOWN;
		else	
			bUpOrDown = DWAVE_UP;
	}
}

/*------------------------------------------------------------- 
  Function Name: FindWaveMinimum

  Purpose:	Identifies the next minimum wave

  Implementation Note(s):
			A minimum wave is identified as a bar where the high is higher than 
			'n'  subsequent bars or where the low is lower than 'n' subsequent bars
			depending on if we are identifying a downward or an upward wave.
			The 'n' is a user inputtable number which comes from the big side.
			In the process, we make sure 4>1 and 5>3>1 rules are met.

  Author:	Coll Kaverill

  Input:
			nMinWaveNumber	--> Minimum Wave to be identified

			nPrevMinWave	-->	Previous Minimum Wave. Acts like a starting point 
								for the search

			pbUpOrDown		-->	Helps to identify if its an upward/downward wave

			pnWaves			--> Waves identified so far. We need this to make sure
								the current wave abides by 4>1 and 5>3>1 rules if needed

			nWaveIndex		--> Helps to identify the previous waves in the pnWaves array

			pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

  Output/Return: The identified  Minimum Wave

--------------------------------------------------------------*/ 
long CTDDWave::FindWaveMinimum(short nMinWaveNumber, long nPrevMinWave, short *pbUpOrDown,
							   long *pnWaves, long nWaveIndex, 
							   float *pfDataArray[6])
{
	short flag=0;	
	//We start to find the next wave minimum from the current one.
	long nMinWave = nPrevMinWave+1; 
	short i;
	
	while(flag==0&&nMinWave<m_nNumPoints)
	{
		//We skip invaild data points
		if(IsInvalid(pfDataArray[DOPEN],pfDataArray[DHIGH],
			pfDataArray[DLOW], pfDataArray[DCLOSE], nMinWave)==1)
		{
			nMinWave++;
			continue;
		}

		//We start out hoping this is our next minimum wave.
		flag = 1;

		for (i=1; i<m_level[nMinWaveNumber]; i++) 
		{
			//Our current min wave is going upwards
		    if(pbUpOrDown[nMinWaveNumber]==DWAVE_UP) 
			{	
				//If the data point is below previous i'th point we fail.
				if(pfDataArray[DTHIGH][nMinWave]<=pfDataArray[DTHIGH][nMinWave-i]) 
					{ flag = 0; break; }

				//If we can't meet 4>1 condition, we fail.
				if(nMinWaveNumber==4&&m_condition[0][0]!=' ')
					if(pfDataArray[DTHIGH][nMinWave] >= 
							pfDataArray[DTLOW][pnWaves[nWaveIndex-2]])
					{ flag = 0; break; }

				//If we can't meet 5>3>1 condtion, we fail.
				if((nMinWaveNumber==5||nMinWaveNumber==3)&&m_condition[1][0]!=' ')
					if(pfDataArray[DTHIGH][nMinWave] <= 
							pfDataArray[DTHIGH][pnWaves[nWaveIndex-1]])
					{ flag = 0; break; }
			}
			else //Our current min wave is going downwards
			{
				//If the data point is above previous i'th point we fail.
				if(pfDataArray[DTLOW][nMinWave]>=pfDataArray[DTLOW][nMinWave-i]) 
					{ flag = 0; break; }

				//If we can't meet 4<1 condition, we fail.
				if(nMinWaveNumber==4&&m_condition[0][0]!=' ')
					if(pfDataArray[DTLOW][nMinWave] <=
							pfDataArray[DTHIGH][pnWaves[nWaveIndex-2]])
					{ flag = 0; break; }

				//If we can't meet 5<3<1 condtion, we fail.
				if((nMinWaveNumber==5||nMinWaveNumber==3)&&m_condition[1][0]!=' ')
					if(pfDataArray[DTLOW][nMinWave] >= 
							pfDataArray[DTLOW][pnWaves[nWaveIndex-1]])
					{ flag = 0; break;}
			}
		}
	//If we don't fail by the end of the loop. This point is our minimum wave and we exit.
	//If we did fail, we iterate with the next data point.
		nMinWave++;
	}

	if (flag==0) 
		return DWAVE_EOD; //End Of Data
	
	return nMinWave-1;
}

/*------------------------------------------------------------- 
  Function Name: FindWave

  Purpose:	Identifies the next wave. 
  
  Implementation Note(s):	
			A wave is the true-highest-high or true-lowest-low between
			Previous Minimum Wave and the Present Minimum Wave. We would 
			be finding highest-high and lowest-low instead of their 
			true counterparts if the user asks for. We also make sure 
			4>1 rule is met for Wave4. Rule 5>3>1 is completely taken care 
			of during the Minimum Wave identification phase itself. 

  Author:	Coll Kaverill

  Input:
			nWaveNumber		--> Wave to be identified

			nPrevMinWave	-->	Previous Minimum Wave
			
			nMinWave		--> Current Minimum Wave

			pbUpOrDown		-->	Helps to identify if its an upward/downward wave

			pnWaves			--> Waves identified so far. We need this to make sure
								the current wave abides by 4>1 rule.

			nWaveIndex		--> Helps to identify the previous waves in the pnWaves array. 

			pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

  Output/Return: The identified Wave

--------------------------------------------------------------*/ 
long CTDDWave::FindWave(short nWaveNumber, long nPrevMinWave, long nMinWave,
						short *pbUpOrDown, long *pnWaves, long nWaveIndex,
						float*pfDataArray[6])

{
	int ii; //Loop control variable

	long nWave = nPrevMinWave;
	short nHigh=DTHIGH, nLow=DTLOW;

	//If the user doesn't want true values to be used for wave identification -
	//we go with plain High/Low values.
	if(m_true_vals[0]=='N'||m_true_vals[0]=='n')
	{
		nHigh = DHIGH;
		nLow = DLOW;
	}

	for(ii=nPrevMinWave; ii<nMinWave; ii++)
	{
		//We skip invaild data points
		if(IsInvalid(pfDataArray[DOPEN],pfDataArray[DHIGH],
			pfDataArray[DLOW], pfDataArray[DCLOSE], ii)==1)
		continue;	

		//Current wave is going upwards
		if(pbUpOrDown[nWaveNumber] == DWAVE_UP)
		{
			//Just trying to find highest high between nPrevMinWave & nMinWave
			if(pfDataArray[nHigh][ii] >= pfDataArray[nHigh][nWave])
			{
				//Making sure we abide by 4>1 rule
				if(nWaveNumber==4&&m_condition[0][0]!=' ')
				{
					if(pfDataArray[nHigh][ii] < pfDataArray[nLow][pnWaves[nWaveIndex-3]])
						nWave = ii;
					else 
						return -ii;
				}
				else
					nWave = ii;
			}
		}
		else //Current wave is going downwards
		{
			//Just trying to find lowest low between nPrevMinWave & nMinWave
			if(pfDataArray[nLow][ii] <= pfDataArray[nLow][nWave])
			{
				//Making sure we abide by 4<1 rule
				if(nWaveNumber==4&&m_condition[0][0]!=' ')
				{
					if(pfDataArray[nLow][ii] > pfDataArray[nHigh][pnWaves[nWaveIndex-3]])
						nWave = ii;
					else
						return -ii;
				}
				else
					nWave = ii;
			}
		}
	}

	return nWave;
}

long CTDDWave::AdjustPrevWaves(long *pnWaves, short *pnWaveNums, long &nWaveIndex, 
							   short *pbUpOrDown, float *pfDataArray[6])
{
	long  start;
	long  end;
	short nWaveNumber;
	long  nWave;
	short bUpOrDown;
	long  nTempWaveIndex = nWaveIndex;

	for(long ii=nTempWaveIndex-1; ii>0; ii--)
	{
		nWave = pnWaves[ii];
		nWaveNumber = pnWaveNums[ii];
		bUpOrDown = pbUpOrDown[nWaveNumber];
		start = pnWaves[ii]+1;
		end = pnWaves[ii+1];

		for(long jj=start; jj<end; jj++)
		{
			if(IsInvalid(pfDataArray[DOPEN],pfDataArray[DHIGH], 
				 pfDataArray[DLOW], pfDataArray[DCLOSE], jj)==1)
				continue;
		
			if(bUpOrDown==DWAVE_UP) 
			{	
				if(pfDataArray[DTHIGH][jj] >= pfDataArray[DTHIGH][nWave])
					nWave = jj;
			}
			else
			{
				if(pfDataArray[DTLOW][jj] <= pfDataArray[DTLOW][nWave])
					nWave = jj;
			}
		}

		if(pnWaves[ii]!=nWave)
		{
			pnWaves[ii]=nWave;
			nWaveIndex--;
		}
		else
			break;
	}

	return NOERROR;
}


/*------------------------------------------------------------- 
  Function Name: DoWaveRelabeling

  Purpose:	Do (any) wave relabelling. 

  Implementation Note(s):	
		Variables are passed in by reference. This is by design to enable this routine 
		to alter loop control variables of DoStudy routine (if relabelling is done).

  Author:	Coll Kaverill

  Input/Output:
			
			  nWaveNumber		-->	Current wave number
			  nMinWaveNumber	--> Current min wave number
			  nPrevMinWave		--> Previous Min. Wave (position)
			  
			  pnWaves			--> Array of wave positions identified so far
			  pnWaveNums		--> Array of wave numbers identified so far
			  nWaveIndex		--> Index of above two array(s) filled so far

			  pnMinWaves		--> Array of min wave positions identified so far
			  pnMinWaveNums		--> Array of min wave numbers identified so far
			  nMinWaveIndex		--> Index of above two array(s) filled so far

			  pbUpOrDown		--> Helps to identify if its an upward/downward wave
			  pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
									for all data points drawn on screen
	
  Return: 'true' if any relabelling is done, 'false' otherwise.

--------------------------------------------------------------*/ 

bool CTDDWave::DoWaveRelabeling(short &nWaveNumber, short &nMinWaveNumber, 
								long &nPrevMinWave,
								long *pnWaves, short *pnWaveNums, long &nWaveIndex, 
								long *pnMinWaves, short *pnMinWaveNums, long &nMinWaveIndex,
								short *pbUpOrDown, float *pfDataArray[6])
{
	//Relabelling happens only at waves 2, 4, B
	if(nWaveNumber==2||nWaveNumber==4||nWaveNumber==7)
	{
		if( //In an upward wave if current wave > current-2'nd wave, relabelling happens
			(pbUpOrDown[nWaveNumber]==DWAVE_UP &&
			 pfDataArray[DHIGH][pnWaves[nWaveIndex]] > 
			 pfDataArray[DHIGH][pnWaves[nWaveIndex-2]]) ||

			//In an downward wave if current wave < current-2'nd wave, relabelling happens
			(pbUpOrDown[nWaveNumber]==DWAVE_DOWN &&
			 pfDataArray[DLOW][pnWaves[nWaveIndex]] < 
			 pfDataArray[DLOW][pnWaves[nWaveIndex-2]])  )
		{
			
			if(nWaveNumber==7) //WaveB surpassing Wave5
			{
				nWaveNumber = 6; 
				nMinWaveNumber = 1; 
				nPrevMinWave = pnWaves[nWaveIndex-1];
				nWaveIndex--;
				nMinWaveIndex--;
				return true;
			}

			if(nWaveIndex==2) // Wave2 surpassing Wave0
			{
				pnWaves[0] = pnWaves[nWaveIndex]; //Wave2 becomes Wave0 : Wave0 slides down/up
				pnMinWaves[0] = pnWaves[nWaveIndex]; //For Wave0, Min Wave is same as actual wave

				nPrevMinWave = pnWaves[nWaveIndex]; //Setting loop control variables
				//We need to idetify Min.Wave1 and thus Wave1 in next iteration
				nMinWaveNumber = 1;
				nWaveNumber =  1;
				nWaveIndex = 1;
				nMinWaveIndex = 1;
				return true;
			}

			//All other relabelling scenarios
			//Wave4 surpassing Wave2
			//Wave2 surpassing WaveA
			//Wave2 surpassing WaveC

			//We slide up/down the subsequent wave only if it makes sense
			if(  
				(pbUpOrDown[nWaveNumber-1]==DWAVE_UP &&
				 pfDataArray[DHIGH][pnWaves[nWaveIndex-1]] >=
				 pfDataArray[DHIGH][pnWaves[nWaveIndex-3]]) ||
	
				(pbUpOrDown[nWaveNumber-1]==DWAVE_DOWN &&
				 pfDataArray[DLOW][pnWaves[nWaveIndex-1]] <=
				 pfDataArray[DLOW][pnWaves[nWaveIndex-3]])  )
			{
				 pnWaves[nWaveIndex-3] = pnWaves[nWaveIndex-1];
				 nPrevMinWave = pnWaves[nWaveIndex-1];
				 nWaveNumber = pnWaveNums[nWaveIndex-3];
				 nMinWaveNumber = pnWaveNums[nWaveIndex-2];
				 nWaveIndex -= 3;
			     nMinWaveIndex -= 3;
				 return true;
			}
	
			//Wave sliding
			pnWaves[nWaveIndex-2] = pnWaves[nWaveIndex];

			//Setting loop control variables of DoStudy routine appropriately
			nPrevMinWave = pnWaves[nWaveIndex];
			nMinWaveNumber = nWaveNumber-1;
			nWaveNumber = pnMinWaveNums[nWaveIndex-2];
			nWaveIndex -= 2;
			nMinWaveIndex -= 2;
			return true;
		}
	}

	return false;
}




/*------------------------------------------------------------- 
  Function Name: AdjustLastMin

  Purpose:	Draw the identified min-waves/waves

  Author:	Coll Kaverill

  Input:				  
			pnMinWaves		--> Array of min wave positions identified
			pnMinWaveNums	--> Array of min wave numbers identified
			nMinWaveIndex	--> Index of above two array(s) filled

			pnWaves			--> Array of wave positions identified
			pnWaveNums		--> Array of wave numbers identified
			pnWaveIndex		--> Index of above two array(s) filled

			pbUpOrDown		-->	Helps to figure if wave marking should be done above
								or below the data point/bar.

			pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

  Output/Return: Return Code

--------------------------------------------------------------*/ 

HRESULT CTDDWave::AdjustLastMin(long  *pnMinWaves, short *pnMinWaveNums, long &nMinWaveIndex,
				  long  *pnWaves, long &nWaveIndex, short *pbUpOrDown, float *pfDataArray[6])
{

	if(nMinWaveIndex==0)
		return NOERROR;


	long start = pnMinWaves[nMinWaveIndex-1]+1;
	long end = m_nNumPoints;
	short nMinWaveNumber = pnMinWaveNums[nMinWaveIndex-1];
	short bUpOrDown = pbUpOrDown[nMinWaveNumber];
	long  nMinWave = pnMinWaves[nMinWaveIndex-1];

	for(long ii=start; ii<end; ii++)
	{
		if(IsInvalid(pfDataArray[DOPEN],pfDataArray[DHIGH], 
					 pfDataArray[DLOW], pfDataArray[DCLOSE], ii)==1)
				continue;
		
		if(bUpOrDown==DWAVE_UP) 
		{	
			if(pfDataArray[DTHIGH][ii] >= pfDataArray[DTHIGH][nMinWave])
				nMinWave = ii;
		}
		else
		{
			if(pfDataArray[DTLOW][ii] <= pfDataArray[DTLOW][nMinWave])
				nMinWave = ii;
		}
	}

	if(nMinWaveNumber==4&&m_condition[0][0]!=' ')
	{
		if(bUpOrDown==DWAVE_UP) 
		{	
			if(pfDataArray[DTHIGH][nMinWave] >= pfDataArray[DTHIGH][pnWaves[nWaveIndex-2]])
			{
				pnMinWaves[nMinWaveIndex-2] = pnWaves[nWaveIndex-1];
				nMinWaveIndex--;
				nWaveIndex--;
				return NOERROR;			
			}
		}
		else
		{
			if(pfDataArray[DTLOW][nMinWave] <= pfDataArray[DTLOW][pnWaves[nWaveIndex-2]])
			{
				pnMinWaves[nMinWaveIndex-2] = pnWaves[nWaveIndex-1];
				nMinWaveIndex--;
				nWaveIndex--;
				return NOERROR;
			}
		}
	}

	pnMinWaves[nMinWaveIndex-1] = nMinWave;
	return NOERROR;
}


/*------------------------------------------------------------- 
  Function Name: DrawWaves

  Purpose:	Draw the identified min-waves/waves

  Author:	Coll Kaverill

  Input:				  
			pnMinWaves		--> Array of min wave positions identified
			pnMinWaveNums	--> Array of min wave numbers identified
			nMinWaveIndex	--> Index of above two array(s) filled

			pnWaves			--> Array of wave positions identified
			pnWaveNums		--> Array of wave numbers identified
			pnWaveIndex		--> Index of above two array(s) filled

			pbUpOrDown		-->	Helps to figure if wave marking should be done above
								or below the data point/bar.

			pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

  Output/Return: Return Code

--------------------------------------------------------------*/ 

HRESULT CTDDWave::DrawWaves(long  *pnMinWaves, short *pnMinWaveNums, long nMinWaveIndex,
						 long  *pnWaves, short *pnWaveNums, long nWaveIndex, 
						 short *pbUpOrDown, float *pfDataArray[6])
{
	long i, j;
	float *pfTDDWaves[DWAVE_NUM_WAVES];
	char label[] = "012345ABC+";

	float min=0.0, 
		  max=0.0, 
		  temp=0.0;

	GetDataRange(m_nNumPoints,pfDataArray[DHIGH],&temp,&max);
	GetDataRange(m_nNumPoints,pfDataArray[DLOW],&min,&temp);

	temp = (max - min) *(float)0.02; // offset for placing characters

	/* Waves 0, 1, 2, 3, 4, 5, A, B, C,
			 1 Min., 2 Min., 3 Min., 4 Min., 5 Min.,
			 A Min., B Min., C Min., */
	for (i=0;i<DWAVE_NUM_WAVES; i++)
	{
		pfTDDWaves[i] = new float[m_nNumPoints];
		if(pfTDDWaves[i] == NULL) 
			return E_OUTOFMEMORY;
	}

	for (i=0;i<DWAVE_NUM_WAVES; i++)
		for (j=0;j<m_nNumPoints; j++) 
			pfTDDWaves[i][j] = HOLEF; //Initialising

	// Draw  min. waves only if asked for
	if(nMinWaveIndex>0)
	{
		//Set the input array and contdown display
			j = nMinWaveIndex-1;
			if(pbUpOrDown[pnMinWaveNums[j]]==DWAVE_UP)
				pfTDDWaves[pnMinWaveNums[j]+8][pnMinWaves[j]] = 
							pfDataArray[DTHIGH][pnMinWaves[j]]+temp;
			else
				pfTDDWaves[pnMinWaveNums[j]+8][pnMinWaves[j]] = 
							pfDataArray[DTLOW][pnMinWaves[j]]-temp;
	
			m_pnCountdownDisplay[pnMinWaves[j]] = -label[9];		
	}

	//Draw em'
	for(i=9; i<DWAVE_NUM_WAVES; i++)
	    SetStudyDataLine(i, pfTDDWaves[i]);
	
	// Drawing the found actual waves
	for (j=0;j<nWaveIndex; j++)
	{	//Set the input array and contdown display
		if(pbUpOrDown[pnWaveNums[j]]==DWAVE_UP)
			pfTDDWaves[pnWaveNums[j]][pnWaves[j]] = pfDataArray[DTHIGH][pnWaves[j]]+temp;
		else
			pfTDDWaves[pnWaveNums[j]][pnWaves[j]] = pfDataArray[DTLOW][pnWaves[j]]-temp;

		m_pnCountdownDisplay[pnWaves[j]] = -label[pnWaveNums[j]];
	}
	
	//Draw em'
	for(i=0; i<DWAVE_MAX_WAVES; i++)
	    SetStudyDataLine(i, pfTDDWaves[i]);


	//Done drawing. We don't need these arrays anymore
	for(i=0; i<DWAVE_NUM_WAVES; i++)
	{
		delete [] pfTDDWaves[i];
	}

	return NOERROR;
}

/*------------------------------------------------------------- 
  Function Name: FindProjections

  Purpose:	Finds wave projections among identified waves.
			Calls DrawProjections to draw the identified projections.

  Author:	Coll Kaverill

  Input:				  
			pnWaves			--> Array of wave positions
			pnWaveNums		--> Array of wave numbers
			pnWaveIndex		--> Index of above two array(s) filled

			pfDataArray		--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

			pGraph			--> Pointer to IBLPGraphData interface, has routines to 
								drawing lines.

  Output/Return: None

--------------------------------------------------------------*/ 

HRESULT CTDDWave::FindProjections(long *pnWaves, short *pnWaveNums, long nWaveIndex, 
								  float *pfDataArray[6], IBLPGraphData *pGraph)
{

	CComPtr<ITrendlineManager> iTrendlineMgr;
	HRESULT hr = pGraph->QueryInterface(IID_ITrendlineManager, (LPVOID *)&iTrendlineMgr);
	ASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return hr;

	m_nTrendLineIndex = 0;

	int nFromWave=-1, nToWave=-1, nAtWave=-1, nStopWave=-1;
	CFastString tempstr("12345ABC");
	short nProjectionNumber;

	for (nProjectionNumber=0; nProjectionNumber<DWAVE_MAX_LINES; nProjectionNumber++)
	{
		//Draw projection only if its set
		if (m_on[nProjectionNumber] != 'X') continue;
		
		//Convert input chars to programmable ints
		// '0' --> 0
		// '1' --> 1
		//  ...
		// '5' --> 5
		// 'A' --> 6
		// 'B' --> 7
		// 'C' --> 8
		nFromWave = tempstr.Find(m_from[nProjectionNumber]);
		if(nFromWave==-1) continue;
		nFromWave++;

		nToWave = tempstr.Find(m_to[nProjectionNumber]);
		if(nToWave==-1) continue;
		nToWave++;

		nAtWave = tempstr.Find(m_at[nProjectionNumber]);
		if(nAtWave==-1) continue;
		nAtWave++;

		nStopWave = tempstr.Find(m_stop[nProjectionNumber]);
		if(nStopWave==-1) continue;
		nStopWave++;

		long nFromPos=-1, nToPos=-1, nAtPos=-1, nStopPos=-1;
		
		//Find them in sequence in our array of waves
		for(long j=0; j<nWaveIndex; j++)
		{
			if(nFromPos==-1)
			{
				if(nFromWave==pnWaveNums[j])
					nFromPos = pnWaves[j];
			}
			else if(nToPos==-1)
			{
				if(nToWave==pnWaveNums[j])
					nToPos = pnWaves[j];
			}
			else if(nAtPos==-1)
			{
				if(nAtWave==pnWaveNums[j])
					nAtPos = pnWaves[j];
			}
			else if(nStopPos==-1)
			{
				if(nStopWave==pnWaveNums[j])
					nStopPos = pnWaves[j];
				else if(j==nWaveIndex-1)
					nStopPos = m_nNumPoints;
			}

			if(nStopPos!=-1)
			{
				//Draw the found projections
				DrawProjection(nFromPos, nToPos, nAtPos, nStopPos, 
					nProjectionNumber, pfDataArray, iTrendlineMgr);

				//Go back to finding 
				nFromPos=-1; 
				nToPos=-1;
			  	nAtPos=-1;
				nStopPos=-1;
			}
		}
	}

	return NOERROR;
}


/*------------------------------------------------------------- 
  Function Name: DrawProjection

  Purpose:	Draw found projection

  Implementation Note(s): 
			The veriticle distance between nFromPos and nToPos times
			a user inputtable number forms the height of the projection.
			The line is drawn starting from nAtPos till nStopPos.

  Author:	Coll Kaverill

  Input:
		nFromPos			--> From position
		nToPos				--> To position
		nAtPos				--> At position
		nStopPos			--> Stop position

		nProjectionNumber	--> Helps to identify the color choice
		pfDataArray			--> Contains Open, High, Low, Close, True High, True Low 
								for all data points drawn on screen

		iTrendlineMgr		--> Interface for drawing lines

  Output/Return: None

--------------------------------------------------------------*/ 
void CTDDWave::DrawProjection(long nFromPos, long nToPos, long  nAtPos, long nStopPos, 
short nProjectionNumber, float *pfDataArray[6], CComPtr<ITrendlineManager> iTrendlineMgr)
{
	float fdiff=0.0;
	float fLineValue=0.0;

	//We know the postions of From, To, At, Stop. 
	//We need to find the actual values for drawing

	//Finding the wave height the way user asked it
	switch (m_wave_height[0])
	{
		case 'O' :
			fdiff = pfDataArray[DOPEN][nToPos] - pfDataArray[DOPEN][nFromPos];
			break;
		case 'H' :
			fdiff = pfDataArray[DHIGH][nToPos] - pfDataArray[DHIGH][nFromPos];
			break;
		case 'L' :
			fdiff = pfDataArray[DLOW][nToPos] - pfDataArray[DLOW][nFromPos];
			break;
		case 'C' :
			fdiff = pfDataArray[DCLOSE][nToPos] - pfDataArray[DCLOSE][nFromPos];
			break;
		default :
			if(m_dir[nProjectionNumber]!='D')
				fdiff = pfDataArray[DHIGH][nToPos] - pfDataArray[DLOW][nFromPos];
			else
				fdiff = pfDataArray[DLOW][nToPos] - pfDataArray[DHIGH][nFromPos];
	}

	fdiff = m_times[nProjectionNumber] * fdiff;
	if (fdiff < 0.0) fdiff = - fdiff;

	//Findout the end. It could be  sooner than expected.
	//Basically, we don't want our projections cutting in the actual graph
	long nStopPosSave = nStopPos;
	if(m_dir[nProjectionNumber]!='D')
	{
		fLineValue = pfDataArray[DLOW][nAtPos] + fdiff;

		for(nStopPos=nAtPos+1; nStopPos<nStopPosSave; nStopPos++)
		{
			if(IsInvalid(pfDataArray[DHIGH][nStopPos])==0&& 
				fLineValue <= pfDataArray[DHIGH][nStopPos])
			break;
		}
	}
	else
	{
	    fLineValue = pfDataArray[DHIGH][nAtPos] - fdiff;

		for(nStopPos=nAtPos+1; nStopPos<nStopPosSave; nStopPos++)
		{
			if(IsInvalid(pfDataArray[DHIGH][nStopPos])==0 && 
				fLineValue >= pfDataArray[DLOW][nStopPos])
			break;
		}
	}

	
	ChartLineParameters pLineParms[] =
	{
		{1, XRT_LPAT_SOLID, 0}
	};

	pLineParms[0].line_color = m_pcolor[nProjectionNumber];

	//Draw the projection
	iTrendlineMgr->AddStudyTrendLine(nAtPos, fLineValue, nStopPos, fLineValue, pLineParms, 
				line_type_study_price_projection, &(m_pTrendLineIds[m_nTrendLineIndex++]));
}

