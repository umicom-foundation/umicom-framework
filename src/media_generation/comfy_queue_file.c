/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_queue_file.c
 *
 * PURPOSE:
 *   Reuse provider-document and regular-file ownership before publishing a captured queue observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/security/secrets.h"
#include "comfy_response_internal.h"
UmiStatus UmiComfyQueueFileRead(const char *document,const char *queue_path,
    const UmiCancellationToken *cancel,UmiComfyQueue **out)
{
    if(out==NULL || *out!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiOutputFileValidatePath(document);
    if(status==UMI_STATUS_OK)status=UmiOutputFileValidatePath(queue_path);
    if(status!=UMI_STATUS_OK)return status;
    UmiProviderConnection connection={0};uint64_t revision=0U;
    unsigned char *bytes=NULL;size_t length=0U;UmiComfyQueue *queue=NULL;
    status=UmiProviderConnectionDocumentFileRead(document,cancel,&connection,&revision);
    if(status==UMI_STATUS_OK)status=UmiComfyConnectionCheck(&connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)status=UmiInputFileRead(queue_path,UMI_COMFY_QUEUE_MAX_BYTES,&bytes,&length);
    if(status==UMI_STATUS_OK)status=UmiComfyQueueCreate(&connection,revision,bytes,length,cancel,&queue);
    /* Retire workflow and extra-data bytes before publication. Never replace a
     * read/parse failure with a later cancellation notification. */
    if(bytes!=NULL){umi_secret_clear(bytes,length+1U);UmiInputFileFree(bytes);}
    umi_secret_clear(&connection,sizeof connection);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){*out=queue;queue=NULL;}
    UmiComfyQueueDestroy(queue);return status;
}
