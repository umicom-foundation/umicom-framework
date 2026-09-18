/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/fingerprint.c
 *
 * PURPOSE:
 *   Dispatch to the selected native hash scanner and fingerprint local files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/fingerprint.h"

#include <stdlib.h>
#include <sys/stat.h>

#include "fingerprint_internal.h"
#include "umicom/platform/filesystem.h"

#define UMI_DOCUMENT_FNV1A_OFFSET UINT64_C(14695981039346656037)

/*
 * Provide the document hash bytes operation used by this module and its client
 * applications.
 */
uint64_t umi_document_hash_bytes(const void *bytes, size_t byte_count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes == NULL && byte_count > 0U) return 0U;
#if defined(UMICOM_DOCUMENT_HASH_X86_64_ASM) || \
    defined(UMICOM_DOCUMENT_HASH_AARCH64_ASM) || \
    defined(UMICOM_DOCUMENT_HASH_RISCV64_ASM)
    return umi_document_hash_bytes_asm((const unsigned char *)bytes,
                                       byte_count,
                                       UMI_DOCUMENT_FNV1A_OFFSET);
#else
    return umi_document_hash_bytes_portable(bytes,
                                             byte_count,
                                             UMI_DOCUMENT_FNV1A_OFFSET);
#endif
}

/*
 * Provide the document fingerprint file operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_fingerprint_file(const char *path,
                                        UmiDocumentFingerprint *out_fingerprint)
{
    unsigned char *bytes = NULL;
    size_t size = 0U;
    struct stat information;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || out_fingerprint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_fingerprint = (UmiDocumentFingerprint){0};
    status = umi_fs_read_bytes(path, &bytes, &size);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_fingerprint->content_hash = umi_document_hash_bytes(bytes, size);
    out_fingerprint->byte_count = size;
    out_fingerprint->modified_time_seconds =
        stat(path, &information) == 0 ? (int64_t)information.st_mtime : 0;
    out_fingerprint->valid = 1;
    umi_fs_free_bytes(bytes);
    return UMI_STATUS_OK;
}

/*
 * Provide the document fingerprint equal operation used by this module and its client
 * applications.
 */
int umi_document_fingerprint_equal(const UmiDocumentFingerprint *left,
                                   const UmiDocumentFingerprint *right)
{
    return left != NULL && right != NULL && left->valid && right->valid &&
           left->content_hash == right->content_hash &&
           left->byte_count == right->byte_count;
}

/*
 * Provide the document fingerprint backend operation used by this module and its client
 * applications.
 */
const char *umi_document_fingerprint_backend(void)
{
#if defined(UMICOM_DOCUMENT_HASH_X86_64_ASM)
    return "x86-64 assembly";
#elif defined(UMICOM_DOCUMENT_HASH_AARCH64_ASM)
    return "AArch64 assembly";
#elif defined(UMICOM_DOCUMENT_HASH_RISCV64_ASM)
    return "RISC-V 64 assembly";
#else
    return "portable C23";
#endif
}

/* All document frontends use the provider selected by the coordinator. */
UmiStatus UmiDocumentFingerprintRead(const UmiDocumentProvider *provider,
    const char *resource, size_t maximumBytes,
    UmiDocumentFingerprint *outFingerprint)
{
    UmiDocumentFileInfo info = {0};
    unsigned char *bytes = NULL;
    size_t length = 0U;
    if (provider == NULL || resource == NULL || resource[0] == '\0' ||
        maximumBytes == 0U || outFingerprint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_document_provider_stat(provider, resource, &info);
    if (status == UMI_STATUS_OK) {
        if (!info.exists) return UMI_STATUS_NOT_FOUND;
        if (!info.regular_file) return UMI_STATUS_NOT_IMPLEMENTED;
        if (info.byte_count > maximumBytes) return UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (status != UMI_STATUS_NOT_IMPLEMENTED) return status;
    status = umi_document_provider_read(provider, resource, &bytes, &length);
    if (status == UMI_STATUS_OK && length > maximumBytes)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && length != 0U && bytes == NULL)
        status = UMI_STATUS_IO_ERROR;
    if (status == UMI_STATUS_OK) {
        UmiDocumentFingerprint result = {0};
        result.content_hash = umi_document_hash_bytes(bytes, length);
        result.byte_count = length;
        result.modified_time_seconds = info.modified_time_seconds;
        result.valid = 1;
        *outFingerprint = result;
    }
    umi_document_provider_release_bytes(provider, bytes);
    return status;
}
