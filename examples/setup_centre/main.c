/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/setup_centre/main.c
 *
 * PURPOSE:
 *   Provide a thin executable entry point over shared native setup services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/setup_centre/setup.h"
#ifdef UMICOM_SETUP_HAS_PROCESS
#include "umicom/setup_centre/execution.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
/* The native execution context now belongs to setup_process so every CLI
 * host uses the same timeout and output ownership. Preserve the former
 * executable-owned implementation for review; the fallback is unchanged. */
#if 0
static int Run(int argc,char **argv)
{

#ifdef UMICOM_SETUP_HAS_PROCESS
    UmiProcessResult *result=calloc(1,sizeof *result);
    if(!result)return 1;

    UmiSetupProcessContext context= {
        NULL,300000U,result
    };

    if (argc > 2 && strcmp(argv[1], "vm-run") == 0)
        for (int i=2; i+1<argc; i+=2)
            if (strcmp(argv[i], "--interactive") == 0 && strcmp(argv[i+1], "yes") == 0)
                context.timeoutMs=0U;
    int code=UmiSetupMainWithExecutor(argc,argv,UmiSetupExecuteProcess,&context);

    if(result->output[0])fputs(result->output,stderr);

    free(result);
    return code;

#else
    return UmiSetupMain(argc,argv);

#endif
}

#endif
static int Run(int argc, char **argv)
{
#ifdef UMICOM_SETUP_HAS_PROCESS
    return UmiSetupNativeMain(argc, argv);
#else
    return UmiSetupMain(argc, argv);
#endif
}

#ifdef _WIN32
int wmain(int argc,wchar_t **wide)
{

    char **argv=calloc((size_t)argc+1U,sizeof *argv);
    if(!argv)return 1;
    int result=1;

    for(int i=0; i<argc; ++i) {
        int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,NULL,0,NULL,NULL);
        if(n<=0)goto done;
        argv[i]=malloc((size_t)n);
        if(!argv[i]||!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,argv[i],n,NULL,NULL))goto done;
    }

    result=Run(argc,argv);

    done: for(int i=0; i<argc; ++i)free(argv[i]);
    free(argv);
    return result;

}

#else
int main(int argc,char **argv) {
    return Run(argc,argv);
}

#endif
