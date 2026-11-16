/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_node_pages_file.c
 *
 * PURPOSE:
 *   Compose captured connection and catalogue admission with the existing copied node selector.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_node_pages.h"
#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/security/secrets.h"
#include "comfy_response_internal.h"
UmiStatus UmiComfyNodeSelectionFileRead(const char *document,const char *object_info,
    const UmiComfyNodeQuery *query,const UmiCancellationToken *cancel,UmiComfyNodeSelection **out)
{
    if(query==NULL || out==NULL || *out!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiOutputFileValidatePath(document);
    if(status==UMI_STATUS_OK)status=UmiOutputFileValidatePath(object_info);
    if(status!=UMI_STATUS_OK)return status;
    UmiProviderConnection connection={0};uint64_t revision=0U;
    unsigned char *bytes=NULL;size_t length=0U;
    UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyNodeSelection *selection=NULL;
    status=UmiProviderConnectionDocumentFileRead(document,cancel,&connection,&revision);
    if(status==UMI_STATUS_OK)status=UmiComfyConnectionCheck(&connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)status=UmiInputFileRead(object_info,UMI_COMFY_NODES_MAX_BYTES,&bytes,&length);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueCreate(&connection,revision,bytes,length,cancel,&catalogue);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeSelect(catalogue,query,cancel,&selection);
    /* The selector owns only copied public metadata. Clear the larger raw
     * capture and catalogue now, so callers cannot accidentally retain spans. */
    if(bytes!=NULL){umi_secret_clear(bytes,length+1U);UmiInputFileFree(bytes);}
    UmiComfyNodeCatalogueDestroy(catalogue);umi_secret_clear(&connection,sizeof connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){*out=selection;selection=NULL;}
    UmiComfyNodeSelectionDestroy(selection);return status;
}
