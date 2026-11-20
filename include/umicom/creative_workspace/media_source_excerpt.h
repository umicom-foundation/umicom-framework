/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/media_source_excerpt.h
 *
 * PURPOSE:
 *   Read a reviewed line range from a bounded local UTF-8 source for Media briefs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_MEDIA_SOURCE_EXCERPT_H
#define UMICOM_CREATIVE_MEDIA_SOURCE_EXCERPT_H

#include "umicom/base/sha256.h"
#include "umicom/platform/cancellation.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_MEDIA_SOURCE_FILE_MAX_BYTES (1024U * 1024U)
#define UMI_MEDIA_SOURCE_EXCERPT_CAPACITY 2049U
/* Installed callers can retain older SDK support without guessing versions. */
#define UMI_MEDIA_SOURCE_EXCERPT_INSPECTION 1

typedef struct UmiMediaSourceExcerpt {
    size_t first_line;
    size_t last_line;
    size_t byte_offset;
    size_t byte_count;
    char text[UMI_MEDIA_SOURCE_EXCERPT_CAPACITY];
} UmiMediaSourceExcerpt;

/* Read a caller-selected absolute regular file in a trusted directory. Only
 * declared UTF-8 plain text is supported. The entire file must be valid UTF-8
 * and may contain tab, CR and LF in addition to printable ASCII and Unicode.
 * Optional leading UTF-8 BOM bytes are excluded from the text. Byte offsets
 * start at zero and refer to the original file, including any BOM bytes.
 *
 * Lines are one-based and inclusive. LF, CRLF and CR are accepted. Preserve
 * separators inside the range and omit the last line's separator. An empty
 * physical line can be selected. A trailing separator adds no extra line.
 * Use a smaller range if the complete selection would exceed 2048 bytes.
 * The function leaves out unchanged on failure and never writes the source.
 * It performs blocking I/O. Check cancellation around the read and before
 * publication, and call from a worker when used by an interactive application.
 * The result owns the copied text and stores no source path or model authority. */
UmiStatus UmiMediaSourceExcerptReadUtf8(const char *absolute_path,
    size_t first_line, size_t last_line, const UmiCancellationToken *cancel,
    UmiMediaSourceExcerpt *out);

/* A copied excerpt and its source identity come from the same read. The
 * digest includes the original byte-order mark and every line separator. */
typedef struct UmiMediaSourceExcerptObservation {
    UmiMediaSourceExcerpt excerpt;
    size_t file_bytes;
    char source_sha256[UMI_SHA256_HEX_CAPACITY];
} UmiMediaSourceExcerptObservation;

/* Inspect with expected_sha256 == NULL. To repeat a reviewed selection,
 * supply its exact lowercase SHA-256. A different source returns
 * INVALID_STATE. The result remains unchanged on failure or cancellation.
 * Pass absolute paths under the same trusted-directory contract. */
UmiStatus UmiMediaSourceExcerptInspectUtf8(const char *absolute_path,
    size_t first_line, size_t last_line, const char *expected_sha256,
    const UmiCancellationToken *cancel, UmiMediaSourceExcerptObservation *out);

/* Dispatch the inspect/excerpt subcommand with UTF-8 arguments. The host
 * owns native argument decoding and calls this at the process entry point. */
int UmiMediaSourceExcerptCommand(int argc, char **argv);

#ifdef __cplusplus
}
#endif
#endif
