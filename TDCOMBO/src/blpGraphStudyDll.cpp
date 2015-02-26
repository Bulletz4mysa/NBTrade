// blpGraphStudyDll.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for blpGraphStudyDll.idl by adding the following 
//      files to the Outputs.
//          blpGraphStudyDll_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f blpGraphStudyDllps.mk in the project directory.

#include "stdafx.h"
#include "compileropts.h"
#include "resource.h"
#include "blpgraphstudydll.h"
#include "blpstudydll.h"
#include "blpstudy.h"
//#include "blpstudydll2.h"
#include "version.h"

#include "dlldatax.h"
#include "trace.h"


#include "blpGraphStudyDll_i.c"

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

///////////////////////////////////////////////////////////////////////////////////////////
//
// BOOL __stdcall ControlFactoryX( SHORT ucid, IUnknown **ppUnk, void *reserved )
//
// ControFactoryX is the main entry point to the dll from the open terminal via the 
// control black box.  This function will be called when the client wants a new control.
// The client recieves the IUnknown pointer, and will ask for the interfaces as 
// needed.
//
BOOL __stdcall ControlFactoryX( SHORT ucid, IUnknown **ppUnk, void *reserved )
{
	if (!ppUnk)
		return FALSE;
 	CheckTraceLevel();

	//
	// Create a user object, or say no if not a ucid for us
	//
  switch ( ucid ) {
#define BLPGRAPHSTUDY	0x005C	// Study Object Factory
  case BLPGRAPHSTUDY:
	  {
		  IBIGObjectFactory *pFactory = new CGraphStudyFactoryStudyDll;
		  HRESULT hr = pFactory->QueryInterface(IID_IBIGObjectFactory, (void **) ppUnk );
		  ASSERT(SUCCEEDED(hr));
		  return SUCCEEDED(hr);
	  }
		  
  default:
		// Not a ucid for us - so say no
		*ppUnk = NULL;
		return FALSE;
   }
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// BOOL __stdcall Version(  SHORT ucid, DWORD *main, DWORD *minor)
//
//  called by client to get the version of the control in question
//

BOOL __stdcall Version(  SHORT ucid, DWORD *main, DWORD *minor)
{
	//
	// do what you need to do here.  ucid is provided if you support multiple
	// controls
	//

#ifdef BLPUNIX
	*main = CTRLSHELL_MAJOR_VERSION;
	*minor = CTRLSHELL_MINOR_VERSION;
#else
#define MAX_VERSION_LENGTH 512
	*main = 0;
	*minor = 0;
        char version_buffer[MAX_VERSION_LENGTH];
	DWORD dummySize;
	DWORD ignore = 0;
	LPVOID lpMsgBuf;
	// N.B. This used to contain the full pathname to the standard
	// location of the dll i.e. "C:\\blp\\wintrv\\blpgraph.dll"
	// however that doesn't work if the BLP install is in a
	// non-standard location. wintrv sets the curent dir to it's
	// install dir, so just using the short filename should
	// work. If it fails, we return the defined version strings
	// CTRLSHELL_MAJOR_VERSION and CTRLSHELL_MINOR_VERSION now
	// away (see below). This is about the best we can do I think.
	LPTSTR filename = TEXT("blpgraphstudy.dll");
	DWORD size = GetFileVersionInfoSize(filename,&dummySize);

	FormatMessage( 
	    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
	    FORMAT_MESSAGE_FROM_SYSTEM | 
	    FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    GetLastError(),
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &lpMsgBuf,
	    0,
	    NULL 
	    );

	if (!size) {
	    *main = CTRLSHELL_MAJOR_VERSION;
	    *minor = CTRLSHELL_MINOR_VERSION;
	} else {
	    size = min(size,MAX_VERSION_LENGTH);
	    GetFileVersionInfo(filename,ignore,size,version_buffer);
	    
	    LPVOID pVS_FIXEDFILEINFO;
	    unsigned int nBytes;
	    BOOL gotVers = VerQueryValue(&version_buffer,
					 TEXT("\\"),
					 &pVS_FIXEDFILEINFO,
					 &nBytes);
	    if (gotVers) {
		VS_FIXEDFILEINFO* pFixedInfo = (VS_FIXEDFILEINFO*) pVS_FIXEDFILEINFO;
		DWORD fileMS = pFixedInfo->dwFileVersionMS;
		DWORD fileLS = pFixedInfo->dwFileVersionLS;
		unsigned short* temp = (unsigned short*) &fileMS;
		unsigned short v2 = *temp++;
		unsigned short v1 = *temp;
		temp = (unsigned short*) &fileLS;
		unsigned short v4 = *temp++;
		unsigned short v3 = *temp;
		LocalFree(lpMsgBuf);
		*main  = (DWORD)v1;
		*minor = (DWORD)v2;
		if ((v3!=0)||(v4!=0)) {
#ifdef DEBUG
		    MessageBox(NULL, "WARNING : Non-zero version number in ignored fields.", "blpGraphStudies", MB_OK);
#endif
		}
	    } else {
		*main = CTRLSHELL_MAJOR_VERSION;
		*minor = CTRLSHELL_MINOR_VERSION;
	    }
	}
#endif
	return TRUE;
}
