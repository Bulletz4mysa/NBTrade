#ifndef __CTDALIGNMENT_H__
#define __CTDALIGNMENT_H__
#include "guids.h"
#include "blpvector.h"

class CTDAlignment : public CBLPOscillatorStudy{
 public:
	CTDAlignment(){}
    virtual HRESULT ConvertBuf(void *pBuf);
    virtual HRESULT CreateStudy(IBLPGraphStudyList *pGraph);
    virtual HRESULT DoStudy(IBLPGraphData *pGraph);
 private:
    BLPVector< CComQIPtr<IBLPStudy, &IID_IBLPStudy> > m_vStudyList;
	CComPtr<IChart> m_AlignChart;
	CComPtr<IBLPGraphStudyList> m_pGraph;
	CComQIPtr<IBLPGraphData, &IID_IBLPGraphData> m_pGraphData;
	ChartLineParameters m_ZeroLine;
	ChartLineParameters m_AlignmentLine;
	short m_nTotal;
 protected:

	HRESULT get_OverboughtOversold(IBLPStudy *pStudy, float *pnOverbought, float *pnOversold);
	HRESULT cal_OverboughtOversold(float *pData, int nSize, float fOverbougt, float fOversold, float *pResult);

    typedef struct tagStructStudyMessageBuf {
		short msg_id;
		short instance_id;
		short base_id;
		short version;

		ChartLineParameters alignment;
		ChartLineParameters zero;
		short total;
    } *StudyMessagePtr;
   
};



#endif
