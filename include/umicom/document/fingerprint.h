/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/fingerprint.h
 *
 * PURPOSE:
 *   Define portable and architecture-accelerated content fingerprints used for
 *   external-change detection, caches, conflict checks and audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_FINGERPRINT_H
#define UMICOM_DOCUMENT_FINGERPRINT_H

#include "umicom/base/status.h"
#include "umicom/document/types.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t umi_document_hash_bytes(const void *bytes, size_t byte_count);
uint64_t umi_document_hash_bytes_portable(const void *bytes,
                                          size_t byte_count,
                                          uint64_t initial_hash);
UmiStatus umi_document_fingerprint_file(const char *path,
                                        UmiDocumentFingerprint *out_fingerprint);
int umi_document_fingerprint_equal(const UmiDocumentFingerprint *left,
                                   const UmiDocumentFingerprint *right);
const char *umi_document_fingerprint_backend(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_FINGERPRINT_H */
