/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_node_pages_file.c
 *
 * PURPOSE:
 *   Verify file admission, cancellation and retained output ownership for copied node selections.
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
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
/* The former narrow main passed code-page argv bytes to UTF-8 path APIs on Windows. Shared native argument capture now preserves the original path; all file and ownership assertions remain. Retain the original entry point for review. The former implementation is retained for owner review. */
#if 0
int main(int argc,char **argv)
{
    CHECK(argc==4);const char *mode=argv[1],*document=argv[2],*input=argv[3];
    UmiComfyNodeQuery query={0};query.order=UMI_COMFY_NODE_ORDER_CLASS;
    UmiComfyNodeSelection *selection=NULL;UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);}
    if(!strcmp(mode,"invalid-query"))strcpy(query.category,"image//edit");
    if(!strcmp(mode,"filter"))strcpy(query.category,"image");
    if(!strcmp(mode,"occupied"))CHECK(UmiComfyNodeSelectionFileRead(document,input,&query,NULL,&selection)==UMI_STATUS_OK);
    UmiComfyNodeSelection *before=selection;
    UmiStatus result=UmiComfyNodeSelectionFileRead(!strcmp(mode,"null-document")?NULL:document,
        !strcmp(mode,"null-input")?NULL:!strcmp(mode,"relative")?"relative.json":input,
        !strcmp(mode,"null-query")?NULL:&query,cancel,!strcmp(mode,"null-output")?NULL:&selection);
    bool success=!strcmp(mode,"complete")||!strcmp(mode,"empty")||!strcmp(mode,"unicode-path")||!strcmp(mode,"filter");
    if(success){
        CHECK(result==UMI_STATUS_OK && selection!=NULL);UmiComfyNodeSelectionReview review;
        CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK);
        CHECK(review.catalogue.connection_revision==7U);
        CHECK(review.source_count==(!strcmp(mode,"empty")?0U:4U));
        CHECK(review.selected_count==(!strcmp(mode,"empty")?0U:!strcmp(mode,"filter")?2U:4U));
        if(review.selected_count){UmiComfyNodeInfo item;CHECK(UmiComfyNodeSelectionAt(selection,0U,&item)==UMI_STATUS_OK);CHECK(!strcmp(item.class_type,"A"));}
    }else{
        CHECK(result!=UMI_STATUS_OK && selection==before);
        if(!strcmp(mode,"cancel"))CHECK(result==UMI_STATUS_CANCELLED);
        if(!strcmp(mode,"occupied")||!strcmp(mode,"null-query")||!strcmp(mode,"null-output")||!strcmp(mode,"invalid-query"))CHECK(result==UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiComfyNodeSelectionDestroy(selection);umi_cancellation_token_destroy(cancel);return 0;
}

#endif
static int RunNodeFileCheck(int argc,char **argv)
{
    CHECK(argc==4);const char *mode=argv[1],*document=argv[2],*input=argv[3];
    UmiComfyNodeQuery query={0};query.order=UMI_COMFY_NODE_ORDER_CLASS;
    UmiComfyNodeSelection *selection=NULL;UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);}
    if(!strcmp(mode,"invalid-query"))strcpy(query.category,"image//edit");
    if(!strcmp(mode,"filter"))strcpy(query.category,"image");
    if(!strcmp(mode,"occupied"))CHECK(UmiComfyNodeSelectionFileRead(document,input,&query,NULL,&selection)==UMI_STATUS_OK);
    UmiComfyNodeSelection *before=selection;
    UmiStatus result=UmiComfyNodeSelectionFileRead(!strcmp(mode,"null-document")?NULL:document,
        !strcmp(mode,"null-input")?NULL:!strcmp(mode,"relative")?"relative.json":input,
        !strcmp(mode,"null-query")?NULL:&query,cancel,!strcmp(mode,"null-output")?NULL:&selection);
    bool success=!strcmp(mode,"complete")||!strcmp(mode,"empty")||!strcmp(mode,"unicode-path")||!strcmp(mode,"filter");
    if(success){
        CHECK(result==UMI_STATUS_OK && selection!=NULL);UmiComfyNodeSelectionReview review;
        CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK);
        CHECK(review.catalogue.connection_revision==7U);
        CHECK(review.source_count==(!strcmp(mode,"empty")?0U:4U));
        CHECK(review.selected_count==(!strcmp(mode,"empty")?0U:!strcmp(mode,"filter")?2U:4U));
        if(review.selected_count){UmiComfyNodeInfo item;CHECK(UmiComfyNodeSelectionAt(selection,0U,&item)==UMI_STATUS_OK);CHECK(!strcmp(item.class_type,"A"));}
    }else{
        CHECK(result!=UMI_STATUS_OK && selection==before);
        if(!strcmp(mode,"cancel"))CHECK(result==UMI_STATUS_CANCELLED);
        if(!strcmp(mode,"occupied")||!strcmp(mode,"null-query")||!strcmp(mode,"null-output")||!strcmp(mode,"invalid-query"))CHECK(result==UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiComfyNodeSelectionDestroy(selection);umi_cancellation_token_destroy(cancel);return 0;
}

/* Capture the original Windows Unicode command line before passing paths
 * to UTF-8 file APIs. This exercises the same argument owner as the CLI. */
int main(int argc, char **argv)
{
    int result = 2;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, RunNodeFileCheck, &result);
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Argument capture failed with status %d\n", (int)status);
    return status == UMI_STATUS_OK ? result : 2;
}
