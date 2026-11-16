/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_node_pages_command.c
 *
 * PURPOSE:
 *   Route native node browsing options into the shared selector and bounded JSON page service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_node_pages.h"
#include "umicom/security/secrets.h"
#include "comfy_command_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int NodesUsage(FILE *stream)
{
    fputs("Umicom captured ComfyUI node catalogue\n"
        "comfy nodes CONNECTION OBJECT_INFO [--search TEXT] [--category TEXT]\n"
        "  [--order source|class|display|category] [--require-true MASK]\n"
        "  [--require-false MASK] [--offset N] [--limit N]\n"
        "Absolute paths; defaults: class order, offset 0, limit 32 (1..64).\n"
        "Flags: output=1, deprecated=2, experimental=4, API=8; unknown is not false.\n"
        "Category matches itself and slash-delimited descendants. Search folds ASCII only.\n"
        "Match revision and all three digests before joining pages.\n"
        "Captured metadata does not execute nodes or establish generation readiness.\n"
        "Exit 0: page emitted; 2: usage; 1: read, selection or output failure.\n",stream);
    return fflush(stream)==0 && !ferror(stream)?0:1;
}
int UmiComfyNodeSelectionFileCommand(int argc,char **argv)
{
    if(argc<0 || (argc>0 && argv==NULL))return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL)return 2;
    if(argc==2 && !strcmp(argv[0],"nodes") && !strcmp(argv[1],"--help"))return NodesUsage(stdout);
    if(argc<3 || argc>17 || (argc-3)%2!=0 || strcmp(argv[0],"nodes")){
        (void)NodesUsage(stderr);return 2;
    }
    UmiComfyNodeQuery query={0};query.order=UMI_COMFY_NODE_ORDER_CLASS;
    size_t offset=0U,limit=32U;unsigned seen=0U;
    for(int i=3;i<argc;i+=2){
        const char *option=argv[i],*value=argv[i+1];unsigned flag=0U;size_t number=0U;
        if(!strcmp(option,"--search") || !strcmp(option,"--category")){
            bool search=!strcmp(option,"--search");size_t size=strlen(value);
            flag=search?1U:2U;
            if(size>=sizeof query.search)flag=0U;
            else memcpy(search?query.search:query.category,value,size+1U);
        }else if(!strcmp(option,"--order")){
            flag=4U;
            if(!strcmp(value,"source"))query.order=UMI_COMFY_NODE_ORDER_SOURCE;
            else if(!strcmp(value,"class"))query.order=UMI_COMFY_NODE_ORDER_CLASS;
            else if(!strcmp(value,"display"))query.order=UMI_COMFY_NODE_ORDER_DISPLAY;
            else if(!strcmp(value,"category"))query.order=UMI_COMFY_NODE_ORDER_CATEGORY;
            else flag=0U;
        }else if(!strcmp(option,"--require-true") || !strcmp(option,"--require-false")){
            bool positive=!strcmp(option,"--require-true");flag=positive?8U:16U;
            if(!UmiComfyCommandNumber(value,15U,&number))flag=0U;
            else if(positive)query.required_true_flags=(unsigned)number;
            else query.required_false_flags=(unsigned)number;
        }else if(!strcmp(option,"--offset")){
            flag=32U;if(!UmiComfyCommandNumber(value,UMI_COMFY_NODES_LIMIT,&offset))flag=0U;
        }else if(!strcmp(option,"--limit")){
            flag=64U;if(!UmiComfyCommandNumber(value,UMI_COMFY_NODE_PAGE_LIMIT,&limit)||limit==0U)flag=0U;
        }
        if(flag==0U || (seen&flag)!=0U){(void)NodesUsage(stderr);return 2;}seen|=flag;
    }
    if((query.required_true_flags&query.required_false_flags)!=0U){(void)NodesUsage(stderr);return 2;}
    UmiComfyNodeSelection *selection=NULL;char *json=NULL;size_t length=0U;
    UmiStatus status=UmiComfyNodeSelectionFileRead(argv[1],argv[2],&query,NULL,&selection);
    if(status!=UMI_STATUS_OK)goto done;
    json=malloc(UMI_COMFY_NODE_PAGE_JSON_CAPACITY);
    if(json==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    status=UmiComfyNodeSelectionJson(selection,offset,limit,json,UMI_COMFY_NODE_PAGE_JSON_CAPACITY,&length);
    if(status!=UMI_STATUS_OK)goto done;
    if(fputs("{\"origin\":\"offline-files\",\"report\":",stdout)==EOF ||
       fwrite(json,1U,length,stdout)!=length || fputs("}\n",stdout)==EOF ||
       fflush(stdout)!=0)status=UMI_STATUS_IO_ERROR;
done:
    if(json!=NULL){umi_secret_clear(json,UMI_COMFY_NODE_PAGE_JSON_CAPACITY);free(json);}
    UmiComfyNodeSelectionDestroy(selection);
    if(status!=UMI_STATUS_OK){fprintf(stderr,"Umicom node observation: %s\n",umi_status_text(status));return 1;}
    return 0;
}
