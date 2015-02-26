#include <stdafx.h>
#include "blpstudydll2.h"
#include "blpstudydll.h"
#include "blpstudydlltd.h"
#include "legacystudies.h"
#include "demarkstudies.h"


// CGraphStudyFactory used by ControlFactoryX to create new Study Instances
IMPLEMENT_IUnknown(CGraphStudyFactoryStudyDll)
LPUNKNOWN CGraphStudyFactoryStudyDll::GetInterfaceHook(const void* piid)
{
	IID riid = *((IID*)piid);
	if (riid == IID_IUnknown || riid == IID_IBIGObjectFactory)
	{
		return (IBIGObjectFactory*)this;
	}

	return NULL;
}


CBLPStudy2 *CGraphStudyFactoryStudyDll::CreateCBLPStudy2(short MsgId)
{

	CBLPStudy2 *pStudy;

	if (CTDStudy::IsDeMarkStudy(MsgId)) {
		pStudy = CTDStudy::CreateStudyInstance(MsgId);
	}else if (CStudyDll::IsBLPGraphStudyDll((short)MsgId)) 
	{
		pStudy = CStudyDll::CreateStudyInstance(MsgId);
	}
	else if(CStudyDllTD::IsBLPGraphStudyDllTD((short)MsgId))
	{
		pStudy = CStudyDllTD::CreateStudyInstance(MsgId);
	}
	else 
	{   // legacy studies ported from core graph and converted to interface architecture
		switch (MsgId) {

		  case blgTE: {
			pStudy = new CTE;
			break;
			}

		  case blgBOLL: {
			pStudy = new CBollingerBands;
			break;
			}

		  case blgDMI: {
			pStudy = new CDirectionalMovementIndicator;
			break;
			}

		  case blgPTPS: {
			pStudy = new CParabolicTimePrice;
			break;
			}

		  case blgChaikin: {
			pStudy = new CStudyDllCHKO;
			break;
			}

		  case blgMACD: {
			pStudy = new CMAvgConvDiv;
			break;
			}

		  case blgTAS: {
			pStudy = new CStudyDllTAS;
			break;
			}

		  case blgGOC:{
			pStudy = new CGOCChart;
			break;
			}

		  case blgROC:{
			pStudy = new CRateOfChange;
			break;
			}

		  case blgVolume: {
			pStudy = new CVolumeChart;
			break;
			}

		  case blgTriangularMAVG: {
			pStudy = new CTriangularMavg;
			break;
			}

		  case blgWeightedMAVG: {
			pStudy = new CWeightedMavg;
			break;
			}

		  case blgVariableMAVG: {
			pStudy = new CVariableMavg;
			break;
			}

		  case blgExponentialMAVG: {
			pStudy = new CExponentialMavg;
			break;
			}

		  case blgSimpleMAVG: {
			pStudy = new CSimpleMavg;
			break;
			}

		  case blgRSI: {
			pStudy = new CRelativeStrengthIndex;
			break;
			}

		  case blgCMCI: {
			pStudy = new CCMCI;
			break;
			}

		  case blgWilliam: {
			pStudy = new CWilliam;
			break;
			}

		  case blgHURST: {
			pStudy = new CHurst;
			break;
			}

		  default:
			return NULL;
		}
	}
	return pStudy;
}

HRESULT CGraphStudyFactoryStudyDll::CreateInstance(unsigned long MsgId, IUnknown **piStudy)
{
	CBLPStudy2 *pStudy = CreateCBLPStudy2((short)MsgId);
	if (pStudy)
	{
		HRESULT hr = pStudy->QueryInterface(IID_IBLPStudy, (LPVOID *)piStudy);
		ASSERT(SUCCEEDED(hr));
		return hr;
	}
	else
		return S_FALSE;

}

CBLPStudy2 *CStudyDll::CreateStudyInstance(unsigned long nMsgId)
{
	CBLPStudy2 *pStudy;

	switch (nMsgId) 
	{
		case CStudyDll::blgStudyDllExponentialMAVG://1200
			pStudy = new CStudyDllExponentialMAVG;
			break;
		case CStudyDll::blgStudyDllOBV://1201
			pStudy = new CStudyDllOBV;
			break;
		case CStudyDll::blgStudyDllSimpleMAVG: //1202
			pStudy = new CStudyDllSimpleMAVG;
			break;
		case CStudyDll::blgStudyDllRSI: //1203
			pStudy = new CStudyDllRSI;
			break;
		case CStudyDll::blgStudyDllTAS: //1204
			pStudy = new CStudyDllTAS;
			break;
		case CStudyDll::blgStudyDllCHKO: //1205
			pStudy = new CStudyDllCHKO;
			break;
		case CStudyDll::blgStudyDllROC: //1206
			pStudy = new CStudyDllROC;
			break;
		case CStudyDll::blgStudyDllKLTN: //1207 TREQ 106718
			pStudy = new CKeltnerBands;
			break;
		default:
			return NULL;
	}
	return pStudy;
}



