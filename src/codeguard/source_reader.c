/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/source_reader.c
 *
 * PURPOSE:
 *   Reuse native bounded input, path resolution and cancellation for source analysis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "source_reader.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include <string.h>

typedef struct UmiCodeGuardByteVisit {
    UmiInputChunkVisitor visitor;
    void *context;
    const UmiCancellationToken *cancellation;
} UmiCodeGuardByteVisit;

static UmiStatus umi_codeguard_reader_observe(UmiStatus status,
    const UmiCancellationToken *cancellation)
{
    return status == UMI_STATUS_OK &&
        umi_cancellation_token_is_requested(cancellation)
        ? UMI_STATUS_CANCELLED : status;
}

static UmiStatus umi_codeguard_reader_bytes(const unsigned char *bytes,
    size_t size, void *context)
{
    UmiCodeGuardByteVisit *visit = context;
    UmiStatus status = umi_codeguard_reader_observe(UMI_STATUS_OK, visit->cancellation);
    if (status == UMI_STATUS_OK)
        status = visit->visitor(bytes, size, visit->context);
    return umi_codeguard_reader_observe(status, visit->cancellation);
}

UmiStatus umi_codeguard_source_visit_bytes(const char *path, uint64_t maximum,
    UmiInputChunkVisitor visitor, void *context,
    const UmiCancellationToken *cancellation)
{
    if (path == NULL || visitor == NULL || path[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    char absolute[UMI_PATH_CAPACITY];
    char directory[UMI_PATH_CAPACITY];
    const char *input = path;
    /* Keep relative compatibility inputs, but resolve them once for this
     * visit. Native input remains the authority for allowed file types and
     * final-link policy. This lexical operation is not a filesystem sandbox. */
    if (!umi_path_is_absolute(path)) {
        UmiStatus status = umi_fs_current_directory(directory, sizeof directory);
        if (status != UMI_STATUS_OK) return status;
        status = umi_path_absolute(path, directory, absolute, sizeof absolute);
        if (status != UMI_STATUS_OK) return status;
        input = absolute;
    }
    UmiCodeGuardByteVisit visit = {visitor, context, cancellation};
    uint64_t size = 0U;
    UmiStatus status = UmiInputFileVisit(input, maximum,
        umi_codeguard_reader_bytes, &visit, &size);
    if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_IO_ERROR;
    /* The final observation also covers empty files, which have no chunks.
     * Cancellation cannot interrupt a blocking operating-system read. */
    return umi_codeguard_reader_observe(status, cancellation);
}

typedef struct UmiCodeGuardLineVisit {
    char text[8192];
    size_t used;
    size_t number;
    UmiCodeGuardLineVisitor visitor;
    void *context;
    const UmiCancellationToken *cancellation;
} UmiCodeGuardLineVisit;

static UmiStatus umi_codeguard_reader_line(UmiCodeGuardLineVisit *visit)
{
    UmiStatus status = umi_codeguard_reader_observe(UMI_STATUS_OK, visit->cancellation);
    if (status != UMI_STATUS_OK) return status;
    if (visit->number == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    visit->text[visit->used] = '\0';
    status = visit->visitor(visit->text, visit->used, ++visit->number, visit->context);
    visit->used = 0U;
    return umi_codeguard_reader_observe(status, visit->cancellation);
}

static UmiStatus umi_codeguard_reader_text(const unsigned char *bytes,
    size_t size, void *context)
{
    UmiCodeGuardLineVisit *visit = context;
    for (size_t i = 0U; i < size; ++i) {
        if (bytes[i] == 0U) return UMI_STATUS_INVALID_ARGUMENT;
        visit->text[visit->used++] = (char)bytes[i];
        if (bytes[i] == '\n' || visit->used == sizeof visit->text - 1U) {
            UmiStatus status = umi_codeguard_reader_line(visit);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_codeguard_source_visit_lines(const char *path, uint64_t maximum,
    UmiCodeGuardLineVisitor visitor, void *context,
    const UmiCancellationToken *cancellation)
{
    if (visitor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardLineVisit visit = {.visitor = visitor, .context = context,
        .cancellation = cancellation};
    UmiStatus status = umi_codeguard_source_visit_bytes(path, maximum,
        umi_codeguard_reader_text, &visit, cancellation);
    /* A trailing partial segment is admitted only after native input and
     * cleanup succeeded. Never turn a truncated input into a valid last line. */
    if (status == UMI_STATUS_OK && visit.used != 0U)
        status = umi_codeguard_reader_line(&visit);
    return status;
}
