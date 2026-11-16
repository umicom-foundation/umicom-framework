/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/comfy_nodes/literals.c
 *
 * PURPOSE:
 *   Inspect captured prompt and Boolean input declarations using an installed Framework SDK without generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_literal_input.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiProviderConnection connection={0};
    strcpy(connection.id,"literal-example");strcpy(connection.provider_id,"comfyui");
    strcpy(connection.label,"Captured prompt options");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;connection.enabled=true;
    const char captured[]=
        "{\"Example\":{\"input\":{\"required\":{"
        "\"prompt\":[\"STRING\",{\"default\":\"A {red|blue} sailboat\\nAt sunrise\",\"multiline\":true,\"dynamicPrompts\":true}],"
        "\"enabled\":[\"BOOLEAN\",{\"default\":false}],"
        "\"unspecified\":[\"BOOLEAN\"]}},\"output\":[\"IMAGE\"]}}";
    UmiComfyNodeCatalogue *catalogue=NULL;
    UmiStatus status=UmiComfyNodeCatalogueCreate(&connection,1U,captured,sizeof captured-1U,NULL,&catalogue);
    UmiComfyLiteralInput prompt={0},enabled={0},unspecified={0};
    char copied[128]={0};
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueLiteralInput(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,0U,&prompt);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueLiteralInput(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,1U,&enabled);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueLiteralInput(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,2U,&unspecified);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueStringDefault(catalogue,0U,UMI_COMFY_INPUT_REQUIRED,0U,copied,sizeof copied);
    /* Only copied metadata survives owner destruction. Observed template syntax
     * is kept literal; applying form defaults or expanding it needs host policy. */
    UmiComfyNodeCatalogueDestroy(catalogue);
    if(status!=UMI_STATUS_OK || !prompt.default_known ||
       prompt.enabled_flags!=(UMI_COMFY_LITERAL_MULTILINE|UMI_COMFY_LITERAL_DYNAMIC_PROMPTS) ||
       !enabled.default_known || enabled.boolean_default || unspecified.default_known ||
       strcmp(copied,"A {red|blue} sailboat\nAt sunrise"))
        return 1;
    if(printf("Captured prompt (not expanded):\n%s\n",copied)<0 ||
       puts("enabled: explicit false; unspecified: default unknown. No generation requested.")==EOF)
        return 1;
    return fflush(stdout)==0?0:1;
}
