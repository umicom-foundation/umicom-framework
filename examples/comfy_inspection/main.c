/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_inspection/main.c
 *
 * PURPOSE:
 *   Build a small installed-SDK console over Framework-owned offline workflow inspection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
static int Inspect(int argc,char **argv)
{
/* Accept the same comfy prefix as production hosts so the installed SDK can run its shared console qualification driver; the direct example spelling remains supported. The former implementation is retained for owner review. */
#if 0
    return UmiComfyInspectionCommand(argc-1,argv+1);
#endif
    int skip=(argc>1 && !strcmp(argv[1],"comfy")) ? 2 : 1;
    return UmiComfyInspectionCommand(argc-skip,argv+skip);
}
int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Inspect,&result);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom inspection: %s\n",umi_status_text(status));return 1;}
    return result;
}
