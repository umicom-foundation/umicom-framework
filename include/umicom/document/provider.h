/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/provider.h
 *
 * PURPOSE:
 *   Define the provider-neutral document storage ABI used by local, memory,
 *   remote, archive and future application-specific resource backends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_PROVIDER_H
#define UMICOM_DOCUMENT_PROVIDER_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_PROVIDER_ABI_VERSION 1U

/**
 * List the named document provider flag values accepted by this public contract.
 */
typedef enum UmiDocumentProviderFlag {
    UMI_DOCUMENT_PROVIDER_READ = 1U << 0,
    UMI_DOCUMENT_PROVIDER_WRITE = 1U << 1,
    UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE = 1U << 2,
    UMI_DOCUMENT_PROVIDER_STAT = 1U << 3,
    UMI_DOCUMENT_PROVIDER_REMOVE = 1U << 4,
    UMI_DOCUMENT_PROVIDER_RENAME = 1U << 5
} UmiDocumentProviderFlag;

/**
 * Represent the document provider data shared with callers of this public contract.
 */
typedef struct UmiDocumentProvider {
    uint32_t struct_size;
    uint32_t abi_version;
    const char *provider_id;
    const char *scheme;
    uint32_t flags;
    void *instance;
    UmiStatus (*read)(void *instance,
                      const char *resource,
                      unsigned char **out_bytes,
                      size_t *out_size);
    UmiStatus (*write)(void *instance,
                       const char *resource,
                       const void *bytes,
                       size_t size,
                       int atomic_replace);
    UmiStatus (*stat)(void *instance,
                      const char *resource,
                      UmiDocumentFileInfo *out_info);
    UmiStatus (*remove)(void *instance, const char *resource);
    UmiStatus (*rename)(void *instance,
                        const char *source,
                        const char *destination);
    void (*release_bytes)(void *instance, void *bytes);
} UmiDocumentProvider;

/**
 * Check that document provider satisfies its contract before another service relies on it.
 */
UmiStatus umi_document_provider_validate(const UmiDocumentProvider *provider);
/**
 * Read document provider into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_document_provider_read(const UmiDocumentProvider *provider,
                                     const char *resource,
                                     unsigned char **out_bytes,
                                     size_t *out_size);
/**
 * Write document provider in its stable representation and report capacity or input
 * failures to the caller.
 */
UmiStatus umi_document_provider_write(const UmiDocumentProvider *provider,
                                      const char *resource,
                                      const void *bytes,
                                      size_t size,
                                      int atomic_replace);
/**
 * Provide the document provider stat operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_provider_stat(const UmiDocumentProvider *provider,
                                     const char *resource,
                                     UmiDocumentFileInfo *out_info);
/**
 * Provide the document provider release bytes operation used by this module and its client
 * applications.
 */
void umi_document_provider_release_bytes(const UmiDocumentProvider *provider,
                                         void *bytes);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_PROVIDER_H */
