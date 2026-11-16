/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_nodes/main.c
 *
 * PURPOSE:
 *   Inspect a synthetic ComfyUI node catalogue using the installed native SDK without contacting a server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_nodes.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    static const char observation[]="{\"ExampleNode\":{\"input\":{\"required\":{\"text\":[\"STRING\"]}},\"output\":[\"IMAGE\"],\"output_node\":true}}";
    UmiProviderConnection connection={0};
    strcpy(connection.id,"example-comfy");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Offline SDK fixture");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;connection.enabled=true;
    UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyNodeInfo info;UmiComfyNodeInput input;
    UmiStatus status=UmiComfyNodeCatalogueCreate(&connection,1,observation,sizeof observation-1,NULL,&catalogue);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueAt(catalogue,0,&info);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueInputAt(catalogue,0,UMI_COMFY_INPUT_REQUIRED,0,&input);
    if(status==UMI_STATUS_OK && (strcmp(info.class_type,"ExampleNode") || strcmp(input.name,"text") ||
       info.required_count!=1U || info.output_count!=1U || input.schema_bytes!=strlen("[\"STRING\"]")))status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)puts("One synthetic node, one required input; no server, Python import or generation.");
    else fprintf(stderr,"Node catalogue example: %s\n",umi_status_text(status));
    UmiComfyNodeCatalogueDestroy(catalogue);return status==UMI_STATUS_OK?0:1;
}
