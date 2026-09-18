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
#include "umicom/document/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the document hash bytes operation used by this module and its client
 * applications.
 */
uint64_t umi_document_hash_bytes(const void *bytes, size_t byte_count);
/**
 * Provide the document hash bytes portable operation used by this module and its client
 * applications.
 */
uint64_t umi_document_hash_bytes_portable(const void *bytes,
                                          size_t byte_count,
                                          uint64_t initial_hash);
/**
 * Provide the document fingerprint file operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_fingerprint_file(const char *path,
                                        UmiDocumentFingerprint *out_fingerprint);
/**
 * Provide the document fingerprint equal operation used by this module and its client
 * applications.
 */
int umi_document_fingerprint_equal(const UmiDocumentFingerprint *left,
                                   const UmiDocumentFingerprint *right);
/**
 * Provide the document fingerprint backend operation used by this module and its client
 * applications.
 */
const char *umi_document_fingerprint_backend(void);

/** Read a fingerprint through the selected provider, not a local-file bypass.
 * maximumBytes must be nonzero. Stat-capable providers reject an oversized
 * resource before reading; the returned size is also checked. Providers that
 * lack stat use read-and-check. This bounds accepted bytes, not allocations
 * internal to an arbitrary provider. The output is written only on success.
 * Fingerprints detect ordinary changes; they are not cryptographic proofs. */
UmiStatus UmiDocumentFingerprintRead(const UmiDocumentProvider *provider,
    const char *resource, size_t maximumBytes,
    UmiDocumentFingerprint *outFingerprint);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_FINGERPRINT_H */
