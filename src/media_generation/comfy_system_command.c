/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_system_command.c
 *
 * PURPOSE:
 *   Expose captured device and system reports through the shared native command boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/



#include "umicom/media_generation/comfy_system_file.h"
#include "umicom/security/secrets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int SystemUsage(FILE *stream)
{
    fputs("Umicom captured ComfyUI system observations\n"
        "comfy system CONNECTION SYSTEM_JSON\n"
        "Use absolute paths. Reports are captured files, not live capacity checks.\n"
        "Unknown fields are null; counters and revisions are exact decimal strings.\n"
        "No provider request, model loading, device allocation or generation.\n"
        "Exit 0: report emitted; 2: usage; 1: file, parse or output failure.\n",stream);
    return fflush(stream)==0 && !ferror(stream)?0:1;
}
int UmiComfySystemStatsFileCommand(int argc,char **argv)
{
    if(argc<0 || (argc>0 && argv==NULL))return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL)return 2;
    if(argc==2 && !strcmp(argv[0],"system") && !strcmp(argv[1],"--help"))return SystemUsage(stdout);
    if(argc!=3 || strcmp(argv[0],"system")){(void)SystemUsage(stderr);return 2;}
    UmiComfySystemStats *stats=NULL;char *json=NULL;size_t length=0U;
    UmiStatus status=UmiComfySystemStatsFileRead(argv[1],argv[2],NULL,&stats);
    if(status!=UMI_STATUS_OK)goto done;
    json=malloc(UMI_COMFY_SYSTEM_JSON_CAPACITY);
    if(json==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    status=UmiComfySystemStatsJson(stats,json,UMI_COMFY_SYSTEM_JSON_CAPACITY,&length);
    if(status!=UMI_STATUS_OK)goto done;
    if(fputs("{\"origin\":\"offline-files\",\"report\":",stdout)==EOF ||
       fwrite(json,1U,length,stdout)!=length || fputs("}\n",stdout)==EOF ||
       fflush(stdout)!=0)status=UMI_STATUS_IO_ERROR;
done:
    if(json!=NULL){umi_secret_clear(json,UMI_COMFY_SYSTEM_JSON_CAPACITY);free(json);}
    UmiComfySystemStatsDestroy(stats);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom system observation: %s\n",umi_status_text(status));return 1;}
    return 0;
}
