/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/document_file.c
 *
 * PURPOSE:
 *   Compose regular-file ownership and canonical connection import before atomic caller publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/provider_connections/document_file.h"
#include "umicom/platform/input_file.h"
#include "umicom/security/secrets.h"
UmiStatus UmiProviderConnectionDocumentFileRead(const char *path,
    const UmiCancellationToken *cancel,UmiProviderConnection *out,uint64_t *out_revision)
{
    if(path==NULL || out==NULL || out_revision==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    unsigned char *bytes=NULL;size_t length=0U;
    UmiProviderConnection connection={0};uint64_t revision=0U;
    UmiStatus status=UmiInputFileRead(path,UMI_PROVIDER_CONNECTION_DOCUMENT_MAX_BYTES-1U,&bytes,&length);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)status=UmiProviderConnectionDocumentImport(bytes,length,&connection,&revision);
    /* Clear the extra terminating byte too. Caller files are never changed. */
    if(bytes!=NULL){umi_secret_clear(bytes,length+1U);UmiInputFileFree(bytes);}
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){*out=connection;*out_revision=revision;}
    umi_secret_clear(&connection,sizeof connection);return status;
}
