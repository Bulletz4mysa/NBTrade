/*
Change Log
	1999/06/15 - LM - Changed signature for HurstExponents. 
				Doesn't need to return number of points. 
				This is to fix the "scrolling"/"rescaling" that was happening
				with the Hurst study, which was caused by passing HurstExponenents
				the address of the number of chart points as the number of output 
				points. Since the study has one fewer points than the chart, 
				the graph would redraw with every new tick.
*/

#include "stdafx.h" // For precompiled headers, etc.
//#include "graphdatatypes.h" // For HOLE/HOLE2
#include "legacystudies.h"
#include <math.h>	// for sqrt(), log()

float lcalc( double *Ret, short Now, short LookBack );
short lin_reg( short n, double *x, double *y, double *slope, double *intcpt );

// Remove HOLE and zero data points.
void HurstClean(	long	NumberOfInputPoints, 
					float*	InputData, 
					long*	NumberOfOutputPoints, 
					double*	OutputData );

void CHurst::HurstExponents(	short	LookBack, 
						long	NumberOfInputPoints, 
						float*	InputData,
						float*	OutputData ) {

	double	*HurstInput = NULL;
	long	NumberOfOutputPoints;
	int		i;

	if ( ( HurstInput = (double*)malloc( NumberOfInputPoints * sizeof(double) ) ) == NULL ) {
		TRACE( "HurstExponents() can't malloc() %d doubles.\n", NumberOfInputPoints );
		return;
	}
	HurstClean( NumberOfInputPoints, InputData, &NumberOfOutputPoints, HurstInput );
	// For now, just do the following. 
	// Later, I think I can speed up the process by taking advantage of 
	// what looks to be several redundant calculations. (Sort of like 
	// recomputing partial sums.) It'll probably come at the expense of 
	// readability. I'll do my best. I promise.
	for ( i = 0; i <= LookBack; i++ )
		OutputData[i] = HOLE;

	for ( i = LookBack + 1; i < NumberOfOutputPoints; i++ ) {
		OutputData[i] = lcalc( HurstInput, i, LookBack );
//		FTRACE( "Close(%d) = %f, HurstInput(%d) = %f, Hurst(%d) = %f\n", i, InputData[i], i, HurstInput[i], i, OutputData[i] );
	}
	free ( HurstInput );
}

float lcalc( double *ret, short now, short lookback )
{
	// See http://www.speedderivs.com/hurst.html for the formulas behind this code.
	// There's also references to books at the end of one of the KAOS help pages
	// on the Bloomberg.
	int		n, i, j, k, A, a, good_step, indx;
	double	*I[300];
	double	Xak, S, minR, maxR;
	double  H, intcpt, en[30];
	int		max_lookback = 0;
	static	double 	*x = NULL;
	double	e[300], R[300], RS[300];

	x = ret + now - lookback;

//	if ( now == 26 )
//		for ( i = 0; i < 100; i++ )
//			FTRACE( "ret[%d] = %f\n", i, ret[i] );

	for (A = 1, indx = 0; A < lookback/4; A *= 2)
	{
		/* Begin step 2 */
		n = lookback/A;
		good_step = 1;


		for (i = 0; i < A; i++) 
		{
			I[i] = x + i*n;

			e[i] = 0.0;
			for (j = 0; j < n; j++)
				e[i] += I[i][j];

			e[i] /= n;
		}
		/* End step 2 */

		/* Begin combined steps 3 and 4 */
		for (a = 0; a < A; a++)
		{
			Xak = 0.0;
			minR = maxR = 0.0;

			for (k = 0; k < n; k++)
			{
				Xak += I[a][k] - e[a];
				minR = min(minR,Xak);
				maxR = max(maxR,Xak);
			}

			R[a] = maxR - minR;
		}
		/* End steps 3 and 4 */

		for (a = 0; a < A; a++)
		{
			S = 0.0;

			/* Begin step 6 */
			for (k = 0; k < n; k++)
				S += (I[a][k] - e[a])*(I[a][k] - e[a]);
			/* End step 6 */

			/* Begin step 7 */
			if (S == 0.0)
			{
				good_step = 0;
				break;
			}

			R[a] /= sqrt(S / n);
		}

		if ( !good_step )
			continue;

		en[indx] = n;
		RS[indx] = 0.0;

		for (a = 0; a < A; a++)
			RS[indx] += R[a];

		RS[indx] /= A;
		indx++;
		/* End step 7 */
	}

	for (i = 0; i < indx; i++)
	{
		en[i] = log(en[i]);
		RS[i] = log(RS[i]);
	}

	// lin_reg will return non-zero if the slope is infinite - i.e. all the en[] values are identical.
	// Not too likely, but it doesn't hurt to check.
	if ( lin_reg( indx, en, RS, &H, &intcpt) ) return HOLE;
	return (float)H;
} // lcalc()

/*
*	linreg.c
*
*	Calculates a linear regression from two arrays, x and y, of length n.
*	Output in doubles slope and intcpt.
*
*/


short lin_reg( short n, double *x, double *y, double *slope, double *intcpt )
{
	double		sumx, sumy, sumx2, sumxy;
	double		top, bottom;
	int			i;

	sumx = sumy = sumx2 = sumxy = 0;

	for (i = 0; i < n; i++)
	{
		sumx += (double)x[i];
		sumy += (double)y[i];
		sumx2 += ( (double)x[i] )*( (double)x[i] );
		sumxy += (double)x[i]*(double)y[i];
	}

	bottom = n*sumx2 - sumx*sumx;
	if ( bottom == 0.0 ) return 1;

	top = n*sumxy - sumx*sumy;

	*slope = top/bottom;

	top = sumx2*sumy - sumx*sumxy;
	*intcpt = top/bottom;

	return 0;
}

void HurstClean(	long	NumberOfInputPoints, 
					float*	InputData, 
					long*	NumberOfOutputPoints, 
					double*	OutputData ) {
	short	i;
	double	dTemp1, dTemp2;

	*NumberOfOutputPoints = 0;

	for ( i = 1; i < NumberOfInputPoints; i++ ) {
//		FTRACE( "InputData[%d] = %f\n", i, InputData[i] );
		// Increment i until we find a point that doesn't fail the test.
		while ((( InputData[i] == HOLEF ) ||
				( InputData[i] == INTERPOLATEF ) ||
				( InputData[i] == 0.0f )) &&
				( i < NumberOfInputPoints ) )
			i++;
		OutputData[(*NumberOfOutputPoints)++] = InputData[i];
	}
	dTemp1 = OutputData[1];
	for ( i = 1; i < *NumberOfOutputPoints; i++ ) {
//		FTRACE( "OutputData[%d] = %f\n", i, OutputData[i] );
		dTemp2 = OutputData[i];			// Save current value
		OutputData[i] = ( OutputData[i] - dTemp1 )/OutputData[i];	// dTemp1 is the previous value, before it was transformed.
		dTemp1 = dTemp2;				// Store the current value for next step.
//		FTRACE( "Returns[%d] = %f\n", i, OutputData[i] );
	}
	(*NumberOfOutputPoints)++;
	OutputData[0] = HOLE;
}	// HurstClean
