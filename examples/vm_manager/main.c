/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/vm_manager/main.c
 * PURPOSE:
 *   Expose saved virtual-machine profiles and reviewed operations through the console.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "umicom/vm_manager/manager.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
int wmain(int argc,wchar_t **wide) {
    char **argv=calloc((size_t)argc+1U,sizeof *argv);
    if(!argv)return 1;
    int result=1;
    for(int i=0;i<argc;++i){
        int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,NULL,0,NULL,NULL);
        if(n<=0)goto done;
        argv[i]=malloc((size_t)n);
        if(!argv[i]||!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,argv[i],n,NULL,NULL))goto done;
    }
    result=UmiVmMain(argc,argv);
    done:for(int i=0;i<argc;++i)free(argv[i]);
    free(argv);
    return result;
}
#else
int main(int argc,char **argv){
    return UmiVmMain(argc,argv);
}
#endif
