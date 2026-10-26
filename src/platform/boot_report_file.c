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
