#ifndef _BLPSTUDYDLL2_H_
#define _BLPSTUDYDLL2_H_

#include "blpstudy.h"
//#include "graphdata.h"

//#define CTRLSHELL_MAJOR_VERSION  4
//#define CTRLSHELL_MINOR_VERSION  50



class CStudyDll : public CBLPStudy2
{
public:
	typedef enum tag_enumMsgIds {	blgStudyDllFirst = 1200,
									blgStudyDllExponentialMAVG = 1200,
									blgStudyDllOBV = 1201,
									blgStudyDllSimpleMAVG = 1202,
									blgStudyDllRSI = 1203,
									blgStudyDllTAS = 1204,
									blgStudyDllCHKO = 1205,
									blgStudyDllROC = 1206,
									blgStudyDllKLTN = 1207, // TREQ 106718
									blgStudyDllLast  = 1207
				} StudyDllMsgIds;

	typedef struct tagStructStudyMessageBuf 
	{
		union
		{	  
			struct 
			{
				//--------version 0
				short msg_id;   //	ExponMAVG 1200
				short instance_id;
				short base_id;
				short period;
				char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
				ChartLineParameters line_params[1];
				BYTE version;
				//--------version 1
				BYTE skip[3];
				ChartPointParameters point_params[1];
			    //--------version 2
				short window_owner_id;
				char window_index;
				char rreserved;
			} ExponMAVG;



			struct 
			{
				short msg_id;   //	SimpleMAVG 1202
				short instance_id;
				short base_id;
				short period;
				char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
				ChartLineParameters line_params[1];
				BYTE version;
				//--------version 1
				BYTE skip[3];
				ChartPointParameters point_params[1];
			    //--------version 2
				short window_owner_id;
				char window_index;
				char rreserved;
			} SimpleMAVG;

			struct
			{
				short msg_id;    // RSI  1203
				short instance_id;
				short base_id;
				short period;
				char field_id[FIELD_LEN]; // high=2,low=3,open=4,close=5
				ChartLineParameters line_params[1];
				OscillatorParameters zone;
				short numStartBar;
				BYTE version;
				//--------version 1
				BYTE skip[3];
				ChartPointParameters point_params[1];
				//--------version 2
				short window_owner_id;
				char window_index;
				char rreserved;
			} RSI;
			struct
			{
				short msg_id;  // 1204
				short instance_id;
				short base_id;
			    short period1;
				short period2;
				short period3;
				short period4;
				short show_tas_fast;
				short show_tas_slow;
				short show_tas_d_ds;
    			ChartLineParameters line_params[4];
				OscillatorParameters zone;
				BYTE version;
    			//--------version 0
    			BYTE skip[3];
				Chart_Color_Scheme chart_colors;
				Chart_Color_Scheme second_chart_colors;
				Chart_Color_Scheme third_chart_colors;
    			ChartPointParameters point_params[4];
    			//--------version 1
    			short window_owner_id;
    			char window_index;
    			char rreserved;
				//--------version 2
			}TAS;

			struct
			{
				short msg_id;
				short instance_id;
				short base_id;
				short period1;
				short period2;
				short period3;
				short period4;
				short mode;
				short lead_lag;
				char field_id[FIELD_LEN];
				short window_id;
				short dummy;
 			    ChartLineParameters line_params[4];
			    BYTE version;
			    //--------version 0
			    BYTE skip[3];
				Chart_Color_Scheme chart_colors;
			    ChartPointParameters point_params[4];
			    //--------version 1
			    short window_owner_id;
			    char window_index;
			    char rreserved;
			    //--------version 2
			}ROC;


		}; // end of study specific union


	//		long reserved[4]; //reserved tail for all study messages
	} StudyMessagePtr;
	
public:
	static BOOL IsBLPGraphStudyDll(short nMessageId)
	{
		if  ( (nMessageId >= blgStudyDllFirst) && (nMessageId <= blgStudyDllLast) ) 
			return TRUE;
		else
			return FALSE;
	}

// Instantiation
public:
	static CBLPStudy2 *CreateStudyInstance(unsigned long nMsgId);
//	static int **m_ppnCountdownDisplay[10];

protected:
	virtual HRESULT RemoveStudy(IBLPGraphStudyList *pGraph)
	{
		return 0;
	}
};


#endif
