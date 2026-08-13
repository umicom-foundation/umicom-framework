/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/provider.c
 *
 * PURPOSE:
 *   Validate provider ABI records and centralise safe provider dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/provider.h"

#include <stdlib.h>

UmiStatus umi_document_provider_validate(const UmiDocumentProviderV1 *provider)
{
    if (provider == NULL || provider->struct_size != sizeof(*provider) ||
        provider->abi_version != UMI_DOCUMENT_PROVIDER_ABI_VERSION ||
        provider->provider_id == NULL || provider->provider_id[0] == '\0' ||
        provider->scheme == NULL || provider->scheme[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_READ) != 0U &&
        (provider->read == NULL || provider->release_bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_WRITE) != 0U &&
        provider->write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_STAT) != 0U &&
        provider->stat == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus umi_document_provider_read(const UmiDocumentProviderV1 *provider,
                                     const char *resource,
                                     unsigned char **out_bytes,
                                     size_t *out_size)
{
    UmiStatus status = umi_document_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;
    if (resource == NULL || out_bytes == NULL || out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_READ) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->read(provider->instance, resource, out_bytes, out_size);
}

UmiStatus umi_document_provider_write(const UmiDocumentProviderV1 *provider,
                                      const char *resource,
                                      const void *bytes,
                                      size_t size,
                                      int atomic_replace)
{
    UmiStatus status = umi_document_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;
    if (resource == NULL || (bytes == NULL && size > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_WRITE) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (atomic_replace &&
        (provider->flags & UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->write(provider->instance, resource, bytes, size,
                           atomic_replace);
}

UmiStatus umi_document_provider_stat(const UmiDocumentProviderV1 *provider,
                                     const char *resource,
                                     UmiDocumentFileInfo *out_info)
{
    UmiStatus status = umi_document_provider_validate(provider);
    if (status != UMI_STATUS_OK) return status;
    if (resource == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_STAT) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->stat(provider->instance, resource, out_info);
}

void umi_document_provider_release_bytes(const UmiDocumentProviderV1 *provider,
                                         void *bytes)
{
    if (bytes == NULL) return;
    if (provider != NULL && provider->release_bytes != NULL) {
        provider->release_bytes(provider->instance, bytes);
    } else {
        free(bytes);
    }
}
