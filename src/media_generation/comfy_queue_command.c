/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_queue_command.c
 *
 * PURPOSE:
 *   Expose captured queue pages through one native command shared by CLI, Media and SDK consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/security/secrets.h"
#include "comfy_command_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int QueueUsage(FILE *stream)
{
    fputs("Umicom captured ComfyUI queue observations\n"
        "comfy queue CONNECTION QUEUE_JSON [--offset N] [--limit N]\n"
        "Use absolute paths; defaults: offset 0, limit 256 (1..256).\n"
        "IDs may be sensitive. Queue numbers are not execution rank or progress.\n"
        "No provider request or queue mutation; absence does not mean completion.\n"
        "Exit 0: page emitted; 2: usage; 1: file, parse, page or output failure.\n",stream);
    return fflush(stream)==0 && !ferror(stream)?0:1;
}
int UmiComfyQueueFileCommand(int argc,char **argv)
{
    if(argc<0 || (argc>0 && argv==NULL))return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL)return 2;
    if(argc==2 && !strcmp(argv[0],"queue") && !strcmp(argv[1],"--help"))return QueueUsage(stdout);
    if(argc<3 || argc>7 || (argc-3)%2!=0 || strcmp(argv[0],"queue")){
        (void)QueueUsage(stderr);return 2;
    }
    size_t offset=0U,limit=UMI_COMFY_QUEUE_PAGE_LIMIT;unsigned seen=0U;
    for(int i=3;i<argc;i+=2){
        unsigned flag=0U;
        if(!strcmp(argv[i],"--offset")){
            flag=1U;if(!UmiComfyCommandNumber(argv[i+1],UMI_COMFY_QUEUE_LIMIT,&offset))flag=0U;
        }else if(!strcmp(argv[i],"--limit")){
            flag=2U;if(!UmiComfyCommandNumber(argv[i+1],UMI_COMFY_QUEUE_PAGE_LIMIT,&limit)||limit==0U)flag=0U;
        }
        if(flag==0U || (seen&flag)!=0U){(void)QueueUsage(stderr);return 2;}seen|=flag;
    }
    UmiComfyQueue *queue=NULL;char *json=NULL;size_t length=0U;
    UmiStatus status=UmiComfyQueueFileRead(argv[1],argv[2],NULL,&queue);
    if(status!=UMI_STATUS_OK)goto done;
    json=malloc(UMI_COMFY_QUEUE_JSON_CAPACITY);
    if(json==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    status=UmiComfyQueueJson(queue,offset,limit,json,UMI_COMFY_QUEUE_JSON_CAPACITY,&length);
    if(status!=UMI_STATUS_OK)goto done;
    if(fputs("{\"origin\":\"offline-files\",\"report\":",stdout)==EOF ||
       fwrite(json,1U,length,stdout)!=length || fputs("}\n",stdout)==EOF ||
       fflush(stdout)!=0)status=UMI_STATUS_IO_ERROR;
done:
    if(json!=NULL){umi_secret_clear(json,UMI_COMFY_QUEUE_JSON_CAPACITY);free(json);}
    UmiComfyQueueDestroy(queue);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom queue observation: %s\n",umi_status_text(status));return 1;}
    return 0;
}
