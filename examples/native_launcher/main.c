/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Keep the executable entry point thin; Framework owns every delivery action. */
#include "umicom/native_launcher/stage.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
/** Windows supplies UTF-16 arguments; conversion is explicit rather than
 * trusting the active ANSI code page to represent a user's directory name. */
int wmain(int argc,wchar_t **wideArguments)
{
    if(argc<1)return 2;
    char **arguments=calloc((size_t)argc+1U,sizeof *arguments);
    if(arguments==NULL)return 1;
    int result=1;
    for(int i=0;i<argc;++i) {
        int size=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wideArguments[i],-1,NULL,0,NULL,NULL);
        if(size<=0)goto done;
        arguments[i]=malloc((size_t)size);if(arguments[i]==NULL)goto done;
        if(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,wideArguments[i],-1,arguments[i],size,NULL,NULL)==0)goto done;
    }
    result=UmiNativeStageMain(argc,arguments);
done:
    for(int i=0;i<argc;++i)free(arguments[i]);
    free(arguments);return result;
}
#else
int main(int argc,char **argv) {return UmiNativeStageMain(argc,argv);}
#endif
