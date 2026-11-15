/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk/artifact_fingerprint.c
 *
 * PURPOSE:
 *   Bind a validated SDK receipt and streamed artifact bytes into one reproducible content fingerprint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk/artifact_fingerprint.h"
#include "umicom/sdk/artifact_inventory.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/input_stream.h"
#include <string.h>

typedef struct SdkDigestStream {
    UmiSha256 hash;
    const UmiCancellationToken *cancellation;
} SdkDigestStream;

static int SdkFingerprintCancelled(const UmiCancellationToken *token)
{
    return token != NULL && umi_cancellation_token_is_requested(token);
}
static UmiStatus SdkFingerprintChunk(const unsigned char *bytes, size_t size, void *context)
{
    SdkDigestStream *stream = context;
    if (SdkFingerprintCancelled(stream->cancellation)) return UMI_STATUS_CANCELLED;
    return UmiSha256Update(&stream->hash, bytes, size);
}
/* Fixed-width little-endian counts make the preimage independent of the host
 * ABI. Hash raw digest bytes, never their presentation spelling. */
static UmiStatus SdkFingerprintCount(UmiSha256 *hash, uint64_t value)
{
    unsigned char encoded[8];
    for (size_t i = 0U; i < sizeof encoded; ++i)
        encoded[i] = (unsigned char)(value >> (i * 8U));
    return UmiSha256Update(hash, encoded, sizeof encoded);
}

UmiStatus UmiSdkArtifactFingerprint(const char *prefix,
    const UmiSdkFingerprintLimits *limits, const UmiCancellationToken *cancellation,
    UmiSdkFingerprint *out_fingerprint)
{
    if (out_fingerprint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_fingerprint, 0, sizeof *out_fingerprint);
    UmiSdkFingerprintLimits budget = {UMI_SDK_FINGERPRINT_FILE_LIMIT, UMI_SDK_FINGERPRINT_TOTAL_LIMIT};
    if (limits != NULL) budget = *limits;
    if (prefix == NULL || !umi_path_is_absolute(prefix) ||
        budget.maximum_file_bytes > UMI_SDK_FINGERPRINT_FILE_LIMIT ||
        budget.maximum_total_bytes > UMI_SDK_FINGERPRINT_TOTAL_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (SdkFingerprintCancelled(cancellation)) return UMI_STATUS_CANCELLED;

    char selected[UMI_PATH_CAPACITY], receipt[UMI_PATH_CAPACITY];
    unsigned char *receipt_bytes = NULL, *checked_bytes = NULL;
    size_t receipt_size = 0U, checked_size = 0U;
    UmiSdkArtifactInventory *inventory = NULL;
    UmiSdkArtifactSummary summary;
    UmiSdkFingerprint candidate = {0};
    UmiStatus status = umi_path_normalise(prefix, selected, sizeof selected);
    if (status == UMI_STATUS_OK)
        status = umi_path_join(selected, UMI_SDK_ARTIFACT_INVENTORY_PATH, receipt, sizeof receipt);
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(receipt, UMI_SDK_ARTIFACT_BYTE_LIMIT, &receipt_bytes, &receipt_size);
    if (status == UMI_STATUS_OK)
        status = UmiSdkArtifactInventoryParse(receipt_bytes, receipt_size, &inventory);
    if (status == UMI_STATUS_OK)
        status = UmiSdkArtifactInventoryReadSummary(inventory, &summary);
    if (status == UMI_STATUS_OK && summary.files == 0U) status = UMI_STATUS_INVALID_STATE;

    UmiSha256 aggregate, receipt_hash;
    unsigned char digest[UMI_SHA256_BYTES];
    static const char domain[] = "umicom.sdk.fingerprint/1\n";
    UmiSha256Init(&aggregate);
    UmiSha256Init(&receipt_hash);
    if (status == UMI_STATUS_OK) {
        memcpy(candidate.configuration, summary.configuration, sizeof candidate.configuration);
        candidate.records = summary.entries;
        candidate.files = summary.files;
        status = UmiSha256Update(&receipt_hash, receipt_bytes, receipt_size);
    }
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&receipt_hash, digest);
    if (status == UMI_STATUS_OK) {
        UmiSha256Hex(digest, candidate.receipt_sha256);
        status = UmiSha256Update(&aggregate, domain, sizeof domain - 1U);
    }
    if (status == UMI_STATUS_OK) status = SdkFingerprintCount(&aggregate, (uint64_t)receipt_size);
    if (status == UMI_STATUS_OK) status = UmiSha256Update(&aggregate, digest, sizeof digest);
    if (status == UMI_STATUS_OK) status = SdkFingerprintCount(&aggregate, (uint64_t)candidate.files);

    for (size_t i = 0U; status == UMI_STATUS_OK && i < candidate.records; ++i) {
        if (SdkFingerprintCancelled(cancellation)) { status = UMI_STATUS_CANCELLED; break; }
        const UmiSdkArtifactEntry *entry = UmiSdkArtifactInventoryEntryAt(inventory, i);
        if (entry == NULL) { status = UMI_STATUS_INTERNAL_ERROR; break; }
        if (entry->kind == UMI_SDK_ARTIFACT_INTERFACE) continue;
        char filename[UMI_PATH_CAPACITY];
        status = umi_path_join(selected, entry->relative_path, filename, sizeof filename);
        uint64_t remaining = budget.maximum_total_bytes - candidate.bytes;
        uint64_t maximum = remaining < budget.maximum_file_bytes ? remaining : budget.maximum_file_bytes;
        uint64_t bytes = 0U;
        SdkDigestStream stream;
        UmiSha256Init(&stream.hash);
        stream.cancellation = cancellation;
        if (status == UMI_STATUS_OK)
            status = UmiInputFileVisit(filename, maximum, SdkFingerprintChunk, &stream, &bytes);
        if (status == UMI_STATUS_OK && SdkFingerprintCancelled(cancellation)) status = UMI_STATUS_CANCELLED;
        if (status == UMI_STATUS_OK) status = UmiSha256Final(&stream.hash, digest);
        if (status == UMI_STATUS_OK) status = SdkFingerprintCount(&aggregate, bytes);
        if (status == UMI_STATUS_OK) status = UmiSha256Update(&aggregate, digest, sizeof digest);
        if (status == UMI_STATUS_OK) candidate.bytes += bytes;
    }
    /* A changed receipt must not describe a successful earlier traversal. This
     * detects observed edits but cannot turn separate file reads into a snapshot.
     * Keep the prefix quiescent when establishing a distribution baseline. */
    if (status == UMI_STATUS_OK && SdkFingerprintCancelled(cancellation)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = UmiInputFileRead(receipt, UMI_SDK_ARTIFACT_BYTE_LIMIT, &checked_bytes, &checked_size);
    if (status == UMI_STATUS_OK &&
        (checked_size != receipt_size || memcmp(checked_bytes, receipt_bytes, receipt_size) != 0))
        status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && SdkFingerprintCancelled(cancellation)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) status = UmiSha256Final(&aggregate, digest);
    if (status == UMI_STATUS_OK) {
        UmiSha256Hex(digest, candidate.sha256);
        *out_fingerprint = candidate;
    }
    UmiInputFileFree(checked_bytes);
    UmiInputFileFree(receipt_bytes);
    UmiSdkArtifactInventoryDestroy(inventory);
    return status;
}
