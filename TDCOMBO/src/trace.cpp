#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include "trace.h"

int g_nTraceLevel = 0;

void CheckTraceLevel()
{
    char szFileName[512] = "blpgraph.dll.trace.level";
    FILE *in = fopen(szFileName, "rt");
    if (in != NULL)
    {
        fscanf(in, "%d", &g_nTraceLevel);
        fclose(in);
    }
}

void CreateTraceFileName(char *log_name)
{
    char szFileName[512];
	strcpy(szFileName, "\\blpgraphstudy.log");											// DW 11/02/99

    char *name;
    name = strrchr(szFileName, '\\');
    if (name == NULL)
        return;

    char szTempDir[512];
    GetTempPath(sizeof szTempDir, szTempDir);

    strcat(szTempDir, name);

    strcpy(log_name, szTempDir);
}


void trace_to_file(const char *format, ...)
{
    static FILE *trace_file = NULL;

	static int linenumber= 0;

    if (trace_file == NULL)
    {
        char szFileName[512];
        CreateTraceFileName(szFileName);

        trace_file = fopen(szFileName, "r+t");
        if (trace_file == NULL)
        {
            trace_file = fopen(szFileName, "wt");
        }

        if (trace_file == NULL)
            return;
    }
	



    //position at the end of file
    fseek(trace_file, 0, SEEK_END);

#ifdef _DEBUG
    if (ftell(trace_file) > 1000000) // DW
#else
    if (ftell(trace_file) > 64000)   // DW
#endif
    {
        //the file grew too long, truncate it
        fclose(trace_file);

        char szFileName[512];
        CreateTraceFileName(szFileName);

        trace_file = fopen(szFileName, "wt");
        
        if (trace_file == NULL)
            return;
    }


    va_list arg_ptr;
    va_start(arg_ptr, format);

	fprintf(trace_file, "%6.6d. ", ++linenumber);    
    vfprintf(trace_file, format, arg_ptr);

    va_end(arg_ptr);

    fflush(trace_file);
}

