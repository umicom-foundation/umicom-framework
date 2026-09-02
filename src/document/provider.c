/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/provider.c
 *
 * PURPOSE:
 *   Validate provider ABI records and centralise safe provider dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/provider.h"

#include <stdlib.h>

/* Check that document provider satisfies its contract before another service relies on it. */
UmiStatus umi_document_provider_validate(const UmiDocumentProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || provider->struct_size != sizeof(*provider) ||
        provider->abi_version != UMI_DOCUMENT_PROVIDER_ABI_VERSION ||
        provider->provider_id == NULL || provider->provider_id[0] == '\0' ||
        provider->scheme == NULL || provider->scheme[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_READ) != 0U &&
        (provider->read == NULL || provider->release_bytes == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_WRITE) != 0U &&
        provider->write == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_STAT) != 0U &&
        provider->stat == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Read document provider into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_document_provider_read(const UmiDocumentProvider *provider,
                                     const char *resource,
                                     unsigned char **out_bytes,
                                     size_t *out_size)
{
    UmiStatus status = umi_document_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resource == NULL || out_bytes == NULL || out_size == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_READ) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->read(provider->instance, resource, out_bytes, out_size);
}

/*
 * Write document provider in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_document_provider_write(const UmiDocumentProvider *provider,
                                      const char *resource,
                                      const void *bytes,
                                      size_t size,
                                      int atomic_replace)
{
    UmiStatus status = umi_document_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resource == NULL || (bytes == NULL && size > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_WRITE) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (atomic_replace &&
        (provider->flags & UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->write(provider->instance, resource, bytes, size,
                           atomic_replace);
}

/*
 * Provide the document provider stat operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_provider_stat(const UmiDocumentProvider *provider,
                                     const char *resource,
                                     UmiDocumentFileInfo *out_info)
{
    UmiStatus status = umi_document_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resource == NULL || out_info == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if ((provider->flags & UMI_DOCUMENT_PROVIDER_STAT) == 0U) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return provider->stat(provider->instance, resource, out_info);
}

/*
 * Provide the document provider release bytes operation used by this module and its client
 * applications.
 */
void umi_document_provider_release_bytes(const UmiDocumentProvider *provider,
                                         void *bytes)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider != NULL && provider->release_bytes != NULL) {
        provider->release_bytes(provider->instance, bytes);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        free(bytes);
    }
}
