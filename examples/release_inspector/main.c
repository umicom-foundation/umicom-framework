/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native entry point. Windows arguments are converted from UTF-16 exactly once. */
#include "umicom/release_inspector/inspection.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
int wmain(int argc,wchar_t **wide)
{
    char **args=calloc((size_t)argc+1U,sizeof *args);if(!args)return 1;
    int converted=0,result=1;
    for(int i=0;i<argc;++i) {
        int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,NULL,0,NULL,NULL);
        if(n<=0)break;
        args[i]=malloc((size_t)n);if(!args[i])break;
        if(!WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wide[i],-1,args[i],n,NULL,NULL))break;
        ++converted;
    }
    if(converted==argc)result=UmiReleaseInspectorMain(argc,args);
    for(int i=0;i<argc;++i)free(args[i]);
    free(args);return result;
}
#else
int main(int argc,char **argv){return UmiReleaseInspectorMain(argc,argv);}
#endif
