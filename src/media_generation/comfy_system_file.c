/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_system_file.c
 *
 * PURPOSE:
 *   Compose existing connection-document and regular-file admission into copied system observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_system_file.h"
#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/security/secrets.h"
#include "comfy_response_internal.h"
UmiStatus UmiComfySystemStatsFileRead(const char *document,const char *system_path,
    const UmiCancellationToken *cancel,UmiComfySystemStats **out)
{
    if(out==NULL || *out!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiOutputFileValidatePath(document);
    if(status==UMI_STATUS_OK)status=UmiOutputFileValidatePath(system_path);
    if(status!=UMI_STATUS_OK)return status;
    UmiProviderConnection connection={0};uint64_t revision=0U;
    unsigned char *bytes=NULL;size_t length=0U;UmiComfySystemStats *stats=NULL;
    status=UmiProviderConnectionDocumentFileRead(document,cancel,&connection,&revision);
    if(status==UMI_STATUS_OK)status=UmiComfyConnectionCheck(&connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)status=UmiInputFileRead(system_path,UMI_COMFY_SYSTEM_MAX_BYTES,&bytes,&length);
    if(status==UMI_STATUS_OK)status=UmiComfySystemStatsCreate(&connection,revision,bytes,length,cancel,&stats);
    /* Temporary argv and extension fields do not survive copied admission.
     * Preserve an earlier read/parse error instead of masking it with late stop. */
    if(bytes!=NULL){umi_secret_clear(bytes,length+1U);UmiInputFileFree(bytes);}
    umi_secret_clear(&connection,sizeof connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){*out=stats;stats=NULL;}
    UmiComfySystemStatsDestroy(stats);return status;
}
