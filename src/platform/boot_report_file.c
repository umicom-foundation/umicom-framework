/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/boot_report_file.c
 *
 * PURPOSE:
 *   Share native file admission and source matching across OS report consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/boot_report_file.h"
#include "umicom/platform/input_file.h"
#include <stdbool.h>
#include <string.h>

static bool SourceIdentity(const char *value)
{
    for (size_t i = 0; i < 64U; ++i)
        if (!((value[i] >= '0' && value[i] <= '9') ||
            (value[i] >= 'a' && value[i] <= 'f'))) return false;
    return value[64] == '\0';
}

/* Preserve the original loader for review. The caller-facing behavior is
 * unchanged; one shared read and parse now feeds normal and capture callers. */
#if 0
UmiStatus UmiBootReportLoad(const char *path, const char *expectedSourceId,
    UmiBootReport *outReport)
{
    if (!outReport || (expectedSourceId && !SourceIdentity(expectedSourceId)))
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0;
    /* Keep file ownership in the common platform reader and grammar in the
     * existing pure parser. Guest init never links this host I/O adapter. */
    UmiStatus status = UmiInputFileRead(path, UMI_BOOT_REPORT_MAX_BYTES, &bytes, &size);
    UmiBootReport parsed;
    if (status == UMI_STATUS_OK)
        status = UmiBootReportParse((const char *)bytes, size, &parsed);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && expectedSourceId &&
        strcmp(parsed.sourceId, expectedSourceId)) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *outReport = parsed;
    return status;
}
#endif

static UmiStatus ReadReport(const char *path, const char *expectedSourceId,
    unsigned char **bytes, size_t *size, UmiBootReport *parsed)
{
    /* The caller owns the read buffer on every outcome. Keep parser admission
     * and declared-source matching identical for the two public routes. */
    if (expectedSourceId && !SourceIdentity(expectedSourceId)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiInputFileRead(path, UMI_BOOT_REPORT_MAX_BYTES, bytes, size);
    if (status == UMI_STATUS_OK)
        status = UmiBootReportParse((const char *)*bytes, *size, parsed);
    if (status == UMI_STATUS_OK && expectedSourceId && strcmp(parsed->sourceId, expectedSourceId))
        return UMI_STATUS_INVALID_STATE;
    return status;
}

UmiStatus UmiBootReportLoad(const char *path, const char *expectedSourceId, UmiBootReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t size = 0U;
    UmiBootReport parsed;
    UmiStatus status = ReadReport(path, expectedSourceId, &bytes, &size, &parsed);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK) *outReport = parsed;
    return status;
}

UmiStatus UmiBootReportInspectFile(const char *path, const char *expectedSourceId,
    const char *expectedCaptureSha256, UmiBootReportCapture *outCapture)
{
    if (outCapture == NULL || (expectedCaptureSha256 && !SourceIdentity(expectedCaptureSha256)))
        return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    UmiBootReportCapture incoming = {0};
    UmiStatus status = ReadReport(path, expectedSourceId, &bytes, &incoming.byteCount, &incoming.report);
    /* Hash the same bytes that passed the parser. A second file open could
     * associate a digest with a different report after a concurrent change. */
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer(bytes, incoming.byteCount, incoming.captureSha256);
    UmiInputFileFree(bytes);
    if (status == UMI_STATUS_OK && expectedCaptureSha256 && strcmp(incoming.captureSha256, expectedCaptureSha256))
        return UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) *outCapture = incoming;
    return status;
}
