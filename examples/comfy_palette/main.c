/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_palette/main.c
 *
 * PURPOSE:
 *   Use the installed SDK to select copied node metadata while keeping unknown flags explicit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_palette.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    const char *json="{\"Unknown\":{\"input\":{},\"output\":[]},\"Stable\":{\"category\":\"image\",\"input\":{},\"output\":[],\"deprecated\":false}}";
    UmiProviderConnection connection={0};strcpy(connection.id,"palette-example");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Palette example");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.enabled=true;connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyNodeSelection *selection=NULL;
    UmiStatus status=UmiComfyNodeCatalogueCreate(&connection,1U,json,strlen(json),NULL,&catalogue);
    UmiComfyNodeQuery query={0};query.required_false_flags=UMI_COMFY_NODE_DEPRECATED;
    query.order=UMI_COMFY_NODE_ORDER_DISPLAY;
    if(status==UMI_STATUS_OK)status=UmiComfyNodeSelect(catalogue,&query,NULL,&selection);
    /* The palette retains copies, not pointers into the catalogue. */
    UmiComfyNodeCatalogueDestroy(catalogue);
    UmiComfyNodeSelectionReview review;UmiComfyNodeInfo item;
    if(status==UMI_STATUS_OK)status=UmiComfyNodeSelectionInspect(selection,&review);
    if(status==UMI_STATUS_OK && review.selected_count!=1U)status=UMI_STATUS_INVALID_STATE;
    if(status==UMI_STATUS_OK)status=UmiComfyNodeSelectionAt(selection,0U,&item);
    if(status==UMI_STATUS_OK)printf("Observed non-deprecated node: %s; unknown flags were excluded.\n",item.class_type);
    else fprintf(stderr,"Umicom palette: %s\n",umi_status_text(status));
    UmiComfyNodeSelectionDestroy(selection);
    return status==UMI_STATUS_OK?0:1;
}
