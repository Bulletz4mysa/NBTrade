#ifndef __BLPSTUDYDLLTD_H__
#define __BLPSTUDYDLLTD_H__

#include "blpstudy.h"

class CStudyDllTD : public CBLPStudy2
{
public:
	typedef enum tag_enumMsgIds {	blgStudyDllFirst = 1301,
									blgStudyDllTDCamouflage = 1301,
									blgStudyDllTDDifferential = 1302,
									blgStudyDllTDReverseDifferential = 1303,
									blgStudyDllTDAntiDifferential = 1304,
									blgStudyDllTDMAVEX = 1305,
									blgStudyDllTDROCStudy = 1306,
									blgTDAlignment = 1307,
									blgStudyDllLast  = 1307
				} StudyDllMsgIds;


	static BOOL IsBLPGraphStudyDllTD(short nMessageId)
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


class CStudyDllTDCamouflage : public CStudyDllTD
{
public:

	typedef struct tagStructStudyMessageBuf 
	{
		short msg_id;   //	TDCamouflage 1301
		short instance_id;
		short base_id;
		int showback;
		short qualifier[2]; // 0 means <, > ; 1 means <=, >=
		short version;
		//--------version 1
		char signal_on[4]; 
		char condition4_on[2];
		char condition5_on[2];
		long color[4];	
	} StudyMessagePtr;

	virtual ~CStudyDllTDCamouflage(){}
	// IBLPStudyDll overrides
	virtual HRESULT ConvertBuf(void * pBuf);
	// Interface - PURE virtual functions must be overrridden by derived classes
	virtual HRESULT DoStudy(IBLPGraphData *pGraph);
	virtual HRESULT CreateStudy(IBLPGraphStudyList * pGraph);
protected:
	int m_ShowBack;
	short m_Qualifier[2];
	short m_version;
	char m_signal_on[4];
	char m_condition4_on[2];
	char m_condition5_on[2];
	long m_color[4];
private:	
};

#endif // #define __BLPSTUDYDLLTD_H__

// end of file
