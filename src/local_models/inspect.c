/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/inspect.c
 *
 * PURPOSE:
 *   Verify local model identity with the shared streaming reader and incremental SHA-256.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/input_stream.h"
#include <string.h>

typedef struct ModelDigest {
    UmiSha256 hash;
    unsigned char header[24];
    size_t header_bytes;
    const UmiCancellationToken *cancellation;
} ModelDigest;
static UmiStatus ObserveModel(const unsigned char *bytes, size_t size, void *context)
{
    ModelDigest *digest = context;
    if (digest->cancellation != NULL && umi_cancellation_token_is_requested(digest->cancellation))
        return UMI_STATUS_CANCELLED;
    size_t copy = sizeof(digest->header) - digest->header_bytes;
    if (copy > size) copy = size;
    if (copy != 0U) {
        memcpy(digest->header + digest->header_bytes, bytes, copy);
        digest->header_bytes += copy;
    }
    return UmiSha256Update(&digest->hash, bytes, size);
}
static uint64_t LittleEndian(const unsigned char *bytes, size_t count)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < count; ++i) value |= (uint64_t)bytes[i] << (i * 8U);
    return value;
}
UmiStatus UmiModelInspect(const char *path, const char *expectedSha256,
    const UmiCancellationToken *cancellation, UmiModelInspection *outInspection)
{
    /* Copy a reviewed digest before clearing output: applications may recheck
     * a file using the digest stored in that same inspection object. */
    char expected[65] = {0};
    bool expectedValid = expectedSha256 == NULL || UmiModelDigestValid(expectedSha256, 64U);
    if (expectedValid && expectedSha256 != NULL) memcpy(expected, expectedSha256, sizeof(expected));
    if (outInspection != NULL) memset(outInspection, 0, sizeof(*outInspection));
    if (outInspection == NULL || !expectedValid) return UMI_STATUS_INVALID_ARGUMENT;
    if (cancellation != NULL && umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    ModelDigest digest = {0};
    UmiModelInspection inspected = {0};
    digest.cancellation = cancellation;
    UmiSha256Init(&digest.hash);
    UmiStatus status = UmiInputFileVisit(path, UMI_MODEL_MAX_BYTES, ObserveModel, &digest, &inspected.bytes);
    if (status != UMI_STATUS_OK) return status;
    if (digest.header_bytes != sizeof(digest.header) || memcmp(digest.header, "GGUF", 4U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    inspected.gguf_version = (uint32_t)LittleEndian(digest.header + 4, 4U);
    inspected.tensors = LittleEndian(digest.header + 8, 8U);
    inspected.metadata_entries = LittleEndian(digest.header + 16, 8U);
    /* Reject obvious truncation/count abuse without claiming tensor parsing.
     * Even one tensor needs descriptors and data beyond the 24-byte header. */
    if ((inspected.gguf_version != 2U && inspected.gguf_version != 3U) ||
        inspected.tensors == 0U || inspected.tensors > (inspected.bytes - 24U) / 16U ||
        inspected.metadata_entries > (inspected.bytes - 24U) / 12U) return UMI_STATUS_PARSE_ERROR;
    unsigned char hash[32];
    status = UmiSha256Final(&digest.hash, hash);
    if (status != UMI_STATUS_OK) return status;
    UmiSha256Hex(hash, inspected.sha256);
    if (expected[0] != '\0' && strcmp(expected, inspected.sha256) != 0)
        return UMI_STATUS_INVALID_STATE;
    *outInspection = inspected;
    return UMI_STATUS_OK;
}
