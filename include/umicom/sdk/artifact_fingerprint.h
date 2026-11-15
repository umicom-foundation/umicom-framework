/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/artifact_fingerprint.h
 *
 * PURPOSE:
 *   Observe receipt-bound SDK artifact contents through bounded shared file readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_ARTIFACT_FINGERPRINT_H
#define UMICOM_SDK_ARTIFACT_FINGERPRINT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_SDK_FINGERPRINT_FILE_LIMIT (UINT64_C(8) * 1024U * 1024U * 1024U)
#define UMI_SDK_FINGERPRINT_TOTAL_LIMIT (UINT64_C(64) * 1024U * 1024U * 1024U)

/* NULL limits select the hard limits above. Explicit zero admits empty files
 * only. Callers can lower either budget; raising it past a hard limit rejects.
 * Total bytes count each physical record, even when paths happen to coincide. */
typedef struct UmiSdkFingerprintLimits {
    uint64_t maximum_file_bytes;
    uint64_t maximum_total_bytes;
} UmiSdkFingerprintLimits;
typedef struct UmiSdkFingerprint {
    char configuration[64];
    size_t records;
    size_t files;
    uint64_t bytes;
    char receipt_sha256[UMI_SHA256_HEX_CAPACITY];
    char sha256[UMI_SHA256_HEX_CAPACITY];
} UmiSdkFingerprint;

/* Read an absolute UTF-8 SDK prefix without writes, loads or child processes.
 * The existing version-1 artifact parser owns path and grammar validation.
 * Read every declared physical artifact with UmiInputFileVisit and re-read the
 * receipt before publishing. Missing, non-regular or linked final leaves,
 * malformed receipts, an interface-only receipt and any byte-budget failure
 * reject. All output fields are cleared on failure; no partial digest escapes.
 *
 * The digest binds exact receipt bytes (including line endings and order) and
 * each physical record's byte count and SHA-256, in receipt order. Prefix
 * location and timestamps are excluded. See the fingerprint guide for the
 * versioned binary framing. No authentication, ABI, headers, unlisted exports
 * or runtime behavior is inferred. Parent aliases may be followed in trusted
 * directories. Sequential reads and metadata checks are not an atomic snapshot.
 *
 * Synchronous I/O belongs on a worker. A borrowed cancellation token must live
 * for the call; cancellation is checked before/between files and per chunk.
 * Inputs and output must not overlap. */
UmiStatus UmiSdkArtifactFingerprint(const char *prefix,
    const UmiSdkFingerprintLimits *limits, const UmiCancellationToken *cancellation,
    UmiSdkFingerprint *out_fingerprint);
#ifdef __cplusplus
}
#endif
#endif
