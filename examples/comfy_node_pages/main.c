/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_node_pages/main.c
 *
 * PURPOSE:
 *   Host the shared captured node browsing command using installed native Unicode argument ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_node_pages.h"
#include "umicom/platform/native_arguments.h"
#include <stdio.h>
#include <string.h>
static int Browse(int argc,char **argv)
{
    if(argc<2 || strcmp(argv[1],"comfy")){
        fputs("Use: comfy nodes --help\n",stderr);return 2;
    }
    return UmiComfyNodeSelectionFileCommand(argc-2,argv+2);
}
int main(int argc,char **argv)
{
    int result=1;
    /* Capture Windows UTF-16 only at the process entry; the command receives a
     * borrowed UTF-8 vector and never owns global locale or working directory. */
    UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,Browse,&result);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom node browser: %s\n",umi_status_text(status));return 1;}
    return result;
}
