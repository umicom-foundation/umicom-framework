/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_reference_command.c
 *
 * PURPOSE:
 *   Expose paged offline reference observations through one shared native console implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "comfy_command_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ReferenceUsage(FILE *stream)
{
    fputs("Umicom ComfyUI reference observations\n"
          "comfy references CONNECTION WORKFLOW OBJECT_INFO [--client-id ID] [--offset N] [--limit N]\n"
          "Use absolute paths. Offset defaults to 0; limit defaults to 256 (range 1..256).\n"
          "Candidate strings are printed and may be custom-node data. No graph validity or execution is implied.\n"
          "Exit 0: page emitted; 2: usage; 1: file, parse, page or output failure. No provider call.\n",stream);
    return ferror(stream)?1:0;
}
/* Parse an explicitly bounded decimal without locale, signs, exponent syntax,
 * overflow or truncation. Counts do not enter a shell or a filesystem path. */
/* Share the established overflow-safe numeric contract with queue pagination. Retain the original helper for review; reference command syntax and limits remain unchanged. The former implementation is retained for owner review. */
#if 0
static bool ReferenceNumber(const char *text,size_t maximum,size_t *out)
{
    if(text==NULL || *text=='\0')return false;
    size_t value=0U;
    for(const unsigned char *p=(const unsigned char *)text;*p!='\0';++p){
        if(*p<'0' || *p>'9')return false;
        size_t digit=(size_t)(*p-'0');
        if(value>maximum/10U || (value==maximum/10U && digit>maximum%10U))return false;
        value=value*10U+digit;
    }
    *out=value;return true;
}
#endif
static bool ReferenceNumber(const char *text,size_t maximum,size_t *out)
{
    return UmiComfyCommandNumber(text,maximum,out);
}

int UmiComfyReferenceInspectionCommand(int argc,char **argv)
{
    if(argc<0 || (argc>0 && argv==NULL))return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL)return 2;
    if(argc==2 && !strcmp(argv[0],"references") && !strcmp(argv[1],"--help"))return ReferenceUsage(stdout);
    if(argc<4 || argc>10 || (argc-4)%2!=0 || strcmp(argv[0],"references")){
        (void)ReferenceUsage(stderr);return 2;
    }
    UmiComfyInspectionPaths paths={0};
    paths.connection_document=argv[1];paths.workflow_json=argv[2];paths.node_catalogue_json=argv[3];
    const char *client="umicom-offline-inspection";
    size_t offset=0U,limit=UMI_COMFY_REFERENCE_PAGE_LIMIT;
    unsigned seen=0U;
    for(int i=4;i<argc;i+=2){
        unsigned flag=0U;
        if(!strcmp(argv[i],"--client-id")){flag=1U;client=argv[i+1];}
        else if(!strcmp(argv[i],"--offset")){
            flag=2U;if(!ReferenceNumber(argv[i+1],UMI_COMFY_REFERENCE_LIMIT,&offset))flag=0U;
        }else if(!strcmp(argv[i],"--limit")){
            flag=4U;if(!ReferenceNumber(argv[i+1],UMI_COMFY_REFERENCE_PAGE_LIMIT,&limit)||limit==0U)flag=0U;
        }
        if(flag==0U || (seen&flag)!=0U){(void)ReferenceUsage(stderr);return 2;}
        seen|=flag;
    }
    size_t length=strlen(client);
    if(length==0U || length>=sizeof paths.client_id){(void)ReferenceUsage(stderr);return 2;}
    memcpy(paths.client_id,client,length+1U);
    UmiComfyReferenceInventory *inventory=NULL;char *json=NULL;
    UmiStatus status=UmiComfyReferenceInspectionRead(&paths,NULL,&inventory);
    if(status!=UMI_STATUS_OK)goto done;
    json=malloc(UMI_COMFY_REFERENCE_JSON_CAPACITY);
    if(json==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    status=UmiComfyReferenceInspectionJson(inventory,offset,limit,json,UMI_COMFY_REFERENCE_JSON_CAPACITY,&length);
    if(status!=UMI_STATUS_OK)goto done;
    /* The explicit subcommand opts into candidate text. Unrelated input values
     * are not serialized, but this is not a secret-redaction service. */
    if(fputs("{\"origin\":\"offline-files\",\"report\":",stdout)==EOF ||
        fwrite(json,1U,length,stdout)!=length || fputs("}\n",stdout)==EOF || fflush(stdout)!=0)
        status=UMI_STATUS_IO_ERROR;
done:
    if(json!=NULL){umi_secret_clear(json,UMI_COMFY_REFERENCE_JSON_CAPACITY);free(json);}
    UmiComfyReferenceInventoryDestroy(inventory);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom reference inspection: %s\n",umi_status_text(status));return 1;}
    return 0;
}
