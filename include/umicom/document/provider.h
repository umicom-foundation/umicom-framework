/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/provider.h
 *
 * PURPOSE:
 *   Define the provider-neutral document storage ABI used by local, memory,
 *   remote, archive and future application-specific resource backends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_PROVIDER_H
#define UMICOM_DOCUMENT_PROVIDER_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_PROVIDER_ABI_VERSION 1U

typedef enum UmiDocumentProviderFlag {
    UMI_DOCUMENT_PROVIDER_READ = 1U << 0,
    UMI_DOCUMENT_PROVIDER_WRITE = 1U << 1,
    UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE = 1U << 2,
    UMI_DOCUMENT_PROVIDER_STAT = 1U << 3,
    UMI_DOCUMENT_PROVIDER_REMOVE = 1U << 4,
    UMI_DOCUMENT_PROVIDER_RENAME = 1U << 5
} UmiDocumentProviderFlag;

typedef struct UmiDocumentProviderV1 {
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
} UmiDocumentProviderV1;

UmiStatus umi_document_provider_validate(const UmiDocumentProviderV1 *provider);
UmiStatus umi_document_provider_read(const UmiDocumentProviderV1 *provider,
                                     const char *resource,
                                     unsigned char **out_bytes,
                                     size_t *out_size);
UmiStatus umi_document_provider_write(const UmiDocumentProviderV1 *provider,
                                      const char *resource,
                                      const void *bytes,
                                      size_t size,
                                      int atomic_replace);
UmiStatus umi_document_provider_stat(const UmiDocumentProviderV1 *provider,
                                     const char *resource,
                                     UmiDocumentFileInfo *out_info);
void umi_document_provider_release_bytes(const UmiDocumentProviderV1 *provider,
                                         void *bytes);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_PROVIDER_H */
