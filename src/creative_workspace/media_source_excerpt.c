/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_source_excerpt.c
 *
 * PURPOSE:
 *   Select exact UTF-8 excerpt bytes and their original line and byte references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_source_excerpt.h"
#include "umicom/base/utf8.h"
#include "umicom/creative_workspace/media_brief.h"
#include "umicom/platform/input_file.h"
#include "umicom/security/secrets.h"
#include <string.h>

/* Keep the displayed excerpt interchangeable with the existing draft field. */
static_assert(sizeof(((UmiMediaBriefDraft *)0)->source) == UMI_MEDIA_SOURCE_EXCERPT_CAPACITY);

static UmiStatus Extract(const unsigned char *bytes, size_t size,
    size_t first_line, size_t last_line, UmiMediaSourceExcerpt *candidate)
{
    static const unsigned char bom[] = {0xEFU, 0xBBU, 0xBFU};
    size_t cursor = size >= sizeof(bom) && memcmp(bytes, bom, sizeof(bom)) == 0 ? sizeof(bom) : 0U;

    /* Validate the complete file. A valid prefix is insufficient when the
     * remainder might belong to a binary or mixed-encoding document. */
    if (!UmiUtf8Validate(bytes + cursor, size - cursor, NULL)) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = cursor; i < size; ++i) {
        unsigned char byte = bytes[i];
        if (byte == 127U || (byte < 32U && byte != '\t' && byte != '\r' && byte != '\n'))
            return UMI_STATUS_PARSE_ERROR;
    }

    /* Offsets stay relative to the original file. Scanning past a CRLF pair
     * once keeps its two bytes intact when that separator is inside a range. */
    size_t start = cursor;
    for (size_t line = 1U; cursor < size; ++line) {
        if (line == first_line) start = cursor;
        while (cursor < size && bytes[cursor] != '\r' && bytes[cursor] != '\n') ++cursor;
        if (line == last_line) {
            size_t count = cursor - start;
            if (count >= sizeof(candidate->text)) return UMI_STATUS_CAPACITY_EXCEEDED;
            candidate->first_line = first_line;
            candidate->last_line = last_line;
            candidate->byte_offset = start;
            candidate->byte_count = count;
            memcpy(candidate->text, bytes + start, count);
            return UMI_STATUS_OK;
        }
        if (cursor == size) break;
        if (bytes[cursor] == '\r' && cursor + 1U < size && bytes[cursor + 1U] == '\n') ++cursor;
        ++cursor;
    }
    return UMI_STATUS_NOT_FOUND;
}

/* The shared read now derives byte references and optional source identity
 * from a single bounded buffer. Retain the former excerpt-only reader for
 * engineering review. The existing public route keeps its original behavior. */
#if 0
UmiStatus UmiMediaSourceExcerptReadUtf8(const char *absolute_path,
    size_t first_line, size_t last_line, const UmiCancellationToken *cancel,
    UmiMediaSourceExcerpt *out)
{
    if (out == NULL || first_line == 0U || last_line < first_line) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(absolute_path, UMI_MEDIA_SOURCE_FILE_MAX_BYTES, &bytes, &size);
    if (status != UMI_STATUS_OK) return status;

    /* Keep intermediate source bytes private. Publish the copied range only
     * after admission, cleanup and the final cooperative cancellation check. */
    UmiMediaSourceExcerpt candidate = {0};
    if (umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    else status = Extract(bytes, size, first_line, last_line, &candidate);
    umi_secret_clear(bytes, size);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out = candidate;
    umi_secret_clear(&candidate, sizeof(candidate));
    return status;
}
#endif

/* The digest is computed before clearing the captured file buffer. Both
 * the identity and the excerpt refer to those exact bytes, even if a path
 * is later replaced before a consumer repeats the selection. */
static UmiStatus ReadSource(const char *absolute_path, size_t first_line,
    size_t last_line, const char *expected_sha256, const UmiCancellationToken *cancel,
    bool inspect, UmiMediaSourceExcerptObservation *candidate)
{
    if (first_line == 0U || last_line < first_line) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiStatus status = UmiInputFileRead(absolute_path, UMI_MEDIA_SOURCE_FILE_MAX_BYTES, &bytes, &size);
    if (status != UMI_STATUS_OK) return status;

    if (umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && inspect) {
        status = UmiSha256Buffer(bytes, size, candidate->source_sha256);
        candidate->file_bytes = size;
        if (status == UMI_STATUS_OK && expected_sha256 != NULL &&
            strcmp(expected_sha256, candidate->source_sha256) != 0)
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        status = Extract(bytes, size, first_line, last_line, &candidate->excerpt);
    }
    umi_secret_clear(bytes, size);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    return status;
}

/* Existing callers can read a range without paying to hash the whole file. */
UmiStatus UmiMediaSourceExcerptReadUtf8(const char *absolute_path,
    size_t first_line, size_t last_line, const UmiCancellationToken *cancel,
    UmiMediaSourceExcerpt *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiMediaSourceExcerptObservation candidate = {0};
    UmiStatus status = ReadSource(absolute_path, first_line, last_line, NULL, cancel, false, &candidate);
    if (status == UMI_STATUS_OK) *out = candidate.excerpt;
    umi_secret_clear(&candidate, sizeof(candidate));
    return status;
}

UmiStatus UmiMediaSourceExcerptInspectUtf8(const char *absolute_path,
    size_t first_line, size_t last_line, const char *expected_sha256,
    const UmiCancellationToken *cancel, UmiMediaSourceExcerptObservation *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate the supplied receipt before reading a path. Canonical output
     * is lowercase hexadecimal, so clients can copy it back verbatim. */
    if (expected_sha256 != NULL &&
        (strspn(expected_sha256, "0123456789abcdef") != UMI_SHA256_HEX_CAPACITY - 1U ||
         expected_sha256[UMI_SHA256_HEX_CAPACITY - 1U] != '\0'))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiMediaSourceExcerptObservation candidate = {0};
    UmiStatus status = ReadSource(absolute_path, first_line, last_line, expected_sha256,
        cancel, true, &candidate);
    if (status == UMI_STATUS_OK) *out = candidate;
    umi_secret_clear(&candidate, sizeof(candidate));
    return status;
}
