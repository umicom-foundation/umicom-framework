/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/fingerprint.c
 *
 * PURPOSE:
 *   Dispatch to the selected native hash scanner and fingerprint local files.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/fingerprint.h"

#include <stdlib.h>
#include <sys/stat.h>

#include "fingerprint_internal.h"
#include "umicom/platform/filesystem.h"

#define UMI_DOCUMENT_FNV1A_OFFSET UINT64_C(14695981039346656037)

uint64_t umi_document_hash_bytes(const void *bytes, size_t byte_count)
{
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

UmiStatus umi_document_fingerprint_file(const char *path,
                                        UmiDocumentFingerprint *out_fingerprint)
{
    unsigned char *bytes = NULL;
    size_t size = 0U;
    struct stat information;
    UmiStatus status;
    if (path == NULL || out_fingerprint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_fingerprint = (UmiDocumentFingerprint){0};
    status = umi_fs_read_bytes(path, &bytes, &size);
    if (status != UMI_STATUS_OK) return status;
    out_fingerprint->content_hash = umi_document_hash_bytes(bytes, size);
    out_fingerprint->byte_count = size;
    out_fingerprint->modified_time_seconds =
        stat(path, &information) == 0 ? (int64_t)information.st_mtime : 0;
    out_fingerprint->valid = 1;
    umi_fs_free_bytes(bytes);
    return UMI_STATUS_OK;
}

int umi_document_fingerprint_equal(const UmiDocumentFingerprint *left,
                                   const UmiDocumentFingerprint *right)
{
    return left != NULL && right != NULL && left->valid && right->valid &&
           left->content_hash == right->content_hash &&
           left->byte_count == right->byte_count;
}

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
