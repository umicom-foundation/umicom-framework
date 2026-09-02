/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/local_provider.c
 *
 * PURPOSE:
 *   Adapt Framework filesystem and atomic-file services to the provider ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/local_provider.h"

#include <stdio.h>

#include "umicom/document/file_identity.h"
#include "umicom/platform/atomic_file.h"
#include "umicom/platform/filesystem.h"

/* Read local into validated module state and return a status when input cannot be used. */
static UmiStatus local_read(void *instance,
                            const char *resource,
                            unsigned char **out_bytes,
                            size_t *out_size)
{
    (void)instance;
    return umi_fs_read_bytes(resource, out_bytes, out_size);
}

/*
 * Write local in its stable representation and report capacity or input failures to the
 * caller.
 */
static UmiStatus local_write(void *instance,
                             const char *resource,
                             const void *bytes,
                             size_t size,
                             int atomic_replace)
{
    (void)instance;
    return atomic_replace
        ? umi_atomic_file_write(resource, bytes, size)
        : umi_fs_write_bytes(resource, bytes, size);
}

/* Provide the local stat operation used by this module and its client applications. */
static UmiStatus local_stat(void *instance,
                            const char *resource,
                            UmiDocumentFileInfo *out_info)
{
    (void)instance;
    return umi_document_file_info(resource, out_info);
}

/* Remove local while keeping the remaining records in a valid and discoverable state. */
static UmiStatus local_remove(void *instance, const char *resource)
{
    (void)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return remove(resource) == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}

/* Provide the local rename operation used by this module and its client applications. */
static UmiStatus local_rename(void *instance,
                             const char *source,
                             const char *destination)
{
    (void)instance;
    return umi_fs_rename(source, destination);
}

/* Release or reset state held by local so the same storage can be reused safely. */
static void local_release(void *instance, void *bytes)
{
    (void)instance;
    umi_fs_free_bytes(bytes);
}

/*
 * Provide the document local provider operation used by this module and its client
 * applications.
 */
UmiDocumentProvider umi_document_local_provider(void)
{
    UmiDocumentProvider provider = {
        .struct_size = sizeof(UmiDocumentProvider),
        .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION,
        .provider_id = "umicom.document.provider.local",
        .scheme = "file",
        .flags = UMI_DOCUMENT_PROVIDER_READ |
                 UMI_DOCUMENT_PROVIDER_WRITE |
                 UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE |
                 UMI_DOCUMENT_PROVIDER_STAT |
                 UMI_DOCUMENT_PROVIDER_REMOVE |
                 UMI_DOCUMENT_PROVIDER_RENAME,
        .instance = NULL,
        .read = local_read,
        .write = local_write,
        .stat = local_stat,
        .remove = local_remove,
        .rename = local_rename,
        .release_bytes = local_release
    };
    return provider;
}
