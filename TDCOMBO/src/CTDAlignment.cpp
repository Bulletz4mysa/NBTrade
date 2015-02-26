#include "stdafx.h"
#include "compileropts.h"
#include "demarkstudies.h"
#include "CTDAlignment.h"
#include "trace.h"

/*
 * Purpose: Convert Child study and Alignment study messages.
 *          The child study must be CBLP2Study and support IBLPOscillator interface.
 *          Otherwise, the child study will not be added.
 */
HRESULT CTDAlignment::ConvertBuf(void *pBuf)
{       
    StudyMessagePtr pHead = (StudyMessagePtr)pBuf;
        
    /*
     * If it is a child study message, the study message starts after the
     * ChildStudyHeader message, parse the child study message id, create
     * child study, convert buffer, create study, and do the calculation of
     * the entire study.
     * If is TD alignment message, convert TD alignment study message,
     * assign to member variables. 
     */
    if (pHead->msg_id == blgPassthruStudy)
    {
        char* p = (char *)pBuf;
        int nHeaderSize = sizeof (ChildStudyHeader);
//        int nIndex = m_vStudyList.size();
        pHead = (StudyMessagePtr)(&p[nHeaderSize]);
        short nMsgId = ntohs(pHead->msg_id);
        CBLPStudy2 *pStudy = CGraphStudyFactoryStudyDll::CreateCBLPStudy2(nMsgId);

        /*
         * After creating the study successfully, See if the CBLPStudy2
         * instance also implement the IBLPOscillator interface. If
         * not, release it. If it is, add to the TD Alignment study list.
         */
        if (pStudy)
        {
			IBLPOscillator *iOs;
            HRESULT hr = pStudy->QueryInterface(IID_IBLPOscillator, (LPVOID *)(&iOs));
            if (SUCCEEDED(hr))
            {
                pStudy->ConvertBuf((void *)&p[nHeaderSize]);
                hr = pStudy->CreateInvisibleStudy(m_pGraph);
				if (FAILED(hr)) {
					 pStudy->Release();
					return hr;
					}

                m_vStudyList.push_back(iOs);
                DoStudy(m_pGraphData);
            }
        }
        else
        {
            return E_HANDLE;
        }
                
    }
    else
    {
        CBLPStudy::ConvertBuf(pBuf);

        ConvertLineParameters(&pHead->alignment, 1);
        ConvertLineParameters(&pHead->zero, 1);
        memcpy(&m_AlignmentLine, &pHead->alignment, sizeof (ChartLineParameters));
        memcpy(&m_ZeroLine, &pHead->zero, sizeof (ChartLineParameters));
        m_nTotal = ntohs(pHead->total);
    }

    return S_OK;    
}

/*
 * Purpose: Create Study. Define the appearance of the study. 
 */
HRESULT CTDAlignment::CreateStudy(IBLPGraphStudyList *pGraph)
{
    HRESULT hr;
    /*
     * We only create study once so that if m_vCharts is not empty that
     * means createStudy has already been called, and it should not be
     * called again.
     */
    if (m_vCharts.empty())
    {
        ChartLineParameters LineParams[] = {{1, XRT_LPAT_NONE, blgWhite}};
        ChartPointParameters PointParams[] = {{XRT_POINT_NONE, 1, blgWhite}};

        hr = AddStudy(pGraph, 0, 0, LineParams, PointParams);
        if (FAILED(hr)) 
            return hr;

        CChartLine *align = new CChartLine;
        align->SetLineParams(m_AlignmentLine);
        m_vChartLine.push_back(align);

        CChartLine *zero = new CChartLine;
        zero->SetLineParams(m_ZeroLine);
        m_vChartLine.push_back(zero);

        m_fMax = m_nTotal + 1;
        m_fMin = -m_nTotal - 1;

        CComQIPtr<IGraphCharts, &IID_IGraphCharts> iGraphCharts(pGraph);
        if (iGraphCharts->CreateOscillatorChart(m_nInstanceId, 1, this, &m_AlignChart) == S_OK) {
            AddChart(m_AlignChart);
        }

        CComQIPtr<IBLPGraphToolbar, &IID_IBLPGraphToolbar> iGraphToolbar(pGraph);
        iGraphToolbar->AddStudyButton(0, "TD Alignment", this);

        m_pGraphData = m_pGraph = pGraph;
        return S_OK;
    }
    else
        return S_FALSE;
        
}


/*
 * Purpose: Calculate the study, put the study data line
 */
HRESULT CTDAlignment::DoStudy(IBLPGraphData *pGraph)
{
    HRESULT hr;
    int i;

	m_pGraphData = pGraph;

    /*
     * First go through all the child studies, and calculate all the child studies
     */
    for (i = 0; i < m_vStudyList.size(); ++i)
    {
        if (m_vStudyList[i])
        {
            hr = m_vStudyList[i]->DoStudy(pGraph);
            if (FAILED(hr))
                return hr;
        }
    }

    /*
     * Calculate TD Alignment
     */
    pGraph->get_NumPoints(&m_nNumPoints);
    BLPVector<float> align(m_nNumPoints, 0);
    BLPVector<float> over(m_nNumPoints, 0);
    for (i = 0; i < m_vStudyList.size(); ++i)
    {
        if (m_vStudyList[i])
        {
            float fOverbought, fOversold;

            float *pData = GetStudyChartLine(m_vStudyList[i], 0, 0);
            hr = get_OverboughtOversold(m_vStudyList[i], &fOverbought, &fOversold);
            if (SUCCEEDED(hr) && pData)
            {
                hr = cal_OverboughtOversold(pData, m_nNumPoints, fOverbought, fOversold, &over[0]);
                if (SUCCEEDED(hr))
                {
                    int j;
                    for (j = 0; j < m_nNumPoints; ++j)
                        align[j] += over[j];
                }
            }
        }
    }


    /*
     * Set the Data lines to the graph
     */
    CComPtr<IStudyData> iSD;
    hr = m_AlignChart->QueryInterface(IID_IStudyData, (LPVOID *)&iSD);
    if (FAILED(hr))
        return S_FALSE;

    BLPVector<float> zero(m_nNumPoints, 0.0);
    iSD->put_StudyDataLine(0, m_nNumPoints, &align[0]);
    iSD->put_StudyDataLine(1, m_nNumPoints, &zero[0]);

    return S_OK;    
}

/*
 * Get the oversold, overbought line if CBLPStudy2 also supports IBLPOscillator
 * interface. If it does not, the function will return an error code.
 */
HRESULT CTDAlignment::get_OverboughtOversold(IBLPStudy *pStudy, float *pnOverbought, float *pnOversold)
{
    if (!pStudy || !pnOverbought || !pnOversold)
        return S_FALSE;

    CComPtr< IBLPOscillator > iUkn;
    HRESULT hr = pStudy->QueryInterface(IID_IBLPOscillator, (LPVOID *)&iUkn);
    if (FAILED(hr))
        return hr;

    IChartLine *pLine;

    hr = iUkn->get_Overbought(pnOverbought, &pLine);
    if (FAILED(hr))
        return hr;


    hr = iUkn->get_Oversold(pnOversold, &pLine);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

/*
 * Calculate the oversold overbought line.
 * -1 represents oversold
 * 0 represents netural
 * +1 represents overbought
 */
HRESULT CTDAlignment::cal_OverboughtOversold(float *pData, int nSize, float fOverbought, float fOversold, float *pResult)
{
    if (pData == NULL || nSize <=0 || pResult == NULL)
        return E_INVALIDARG;

    if (IsInvalid(fOverbought) || IsInvalid(fOversold))
        return E_NOTIMPL;

    int i;

    for (i = 0; i < nSize; ++i)
    {
        if (IsInvalid(pData[i]))
            pResult[i] = 0;
        else if (pData[i] > fOverbought)
            pResult[i] = 1;
        else if (pData[i] < fOversold)
            pResult[i] = -1;
        else
            pResult[i] = 0;
    }

    return S_OK;
}


    
