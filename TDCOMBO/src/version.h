// version.h for blpgraphstudy.dll
// Copyright (c) Bloomberg L.P. $Date: 2004/02/13 16:40:31 $
// $Header: /home/cvs2/external/version.h,v 1.51 2004/02/13 16:40:31 memani Exp $
//

#ifndef _VERSION_H
#define _VERSION_H 1

//
// CTRLSHELL_MAJOR_VERSION
// 
// major version number.
//
#define CTRLSHELL_MAJOR_VERSION  1

//
// CTRLSHELL_MINOR_VERSION
// 
// minor version number.
//
#define CTRLSHELL_MINOR_VERSION  291

// V1.05: "Dummy" class to get version into the class browser list

class CVersion
{
public:
	static int GetMajorVersion(void) { return CTRLSHELL_MAJOR_VERSION; }; 
	static int GetMinorVersion(void) { return CTRLSHELL_MINOR_VERSION; };
};

#endif // _VERSION_H
