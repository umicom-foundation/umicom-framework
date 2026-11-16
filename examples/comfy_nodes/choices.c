/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_nodes/choices.c
 *
 * PURPOSE:
 *   Use an installed Framework SDK to inspect captured sampler choices without selecting or executing a workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_choice_input.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiProviderConnection connection = {0};
    strcpy(connection.id,"choice-example");
    strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Captured choices");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms=30000;
    connection.enabled=true;
    const char captured[] =
        "{\"ExampleSampler\":{\"input\":{\"required\":{"
        "\"sampler\":[[\"euler\",\"heun\",\"dpmpp_2m\"],{\"tooltip\":\"Observed list\"}],"
        "\"checkpoint\":[[]]}},\"output\":[\"IMAGE\"]}}";
    UmiComfyNodeCatalogue *catalogue=NULL;
    UmiStatus status=UmiComfyNodeCatalogueCreate(&connection,1U,captured,sizeof captured-1U,NULL,&catalogue);
    UmiComfyChoiceInput sampler={0},checkpoint={0};
    char copied[3][32]={{0}};
    if(status==UMI_STATUS_OK)
        status=UmiComfyNodeCatalogueChoiceInput(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,0U,&sampler);
    if(status==UMI_STATUS_OK)
        status=UmiComfyNodeCatalogueChoiceInput(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,1U,&checkpoint);
    if(status==UMI_STATUS_OK && sampler.choice_count!=3U)
        status=UMI_STATUS_PARSE_ERROR;
    for(size_t index=0U;index<3U && status==UMI_STATUS_OK;++index)
        status=UmiComfyNodeCatalogueChoiceAt(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,0U,
            index,copied[index],sizeof copied[index]);
    /* Copy before destroying the observation owner. A report count is not a
     * selected row; an empty checkpoint list stays empty rather than inventing
     * a model. Any later workflow needs fresh review and server validation. */
    UmiComfyNodeCatalogueDestroy(catalogue);
    if(status!=UMI_STATUS_OK || checkpoint.choice_count!=0U || sampler.uninterpreted_options!=1U ||
       strcmp(copied[0],"euler") || strcmp(copied[1],"heun") || strcmp(copied[2],"dpmpp_2m"))
        return 1;
    if(puts("Captured sampler options (no automatic selection):")==EOF)
        return 1;
    for(size_t index=0U;index<3U;++index)
        if(printf("  %s\n",copied[index])<0) return 1;
    if(puts("No checkpoint options were observed. No generation requested.")==EOF)
        return 1;
    return fflush(stdout)==0?0:1;
}
