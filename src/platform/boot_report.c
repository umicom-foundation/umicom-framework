/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/boot_report.c
 *
 * PURPOSE:
 *   Validate the complete boot report before publishing its snapshot.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/boot_report.h"
#include <stdbool.h>
#include <string.h>

static bool Equal(const char *value, size_t size, const char *expected)
{
    return strlen(expected) == size && memcmp(value, expected, size) == 0;
}
static bool Number(const char *value, size_t length, uint32_t *out)
{
    uint32_t result = 0;
    if (length == 0 || length > 2 || (length > 1 && value[0] == '0')) return false;
    for (size_t i = 0; i < length; ++i) {
        if (value[i] < '0' || value[i] > '9') return false;
        result = result * 10U + (uint32_t)(value[i] - '0');
    }
    if (result > UMI_BOOT_REPORT_SERVICE_LIMIT) return false;
    *out = result;
    return true;
}
static bool Reason(const char *value, size_t length)
{
    static const char *const reasons[] = {
        "none", "requested", "configuration", "mount", "service-exit",
        "service-timeout", "service-launch", "report-write", "privilege"
    };
    for (size_t i = 0; i < sizeof reasons / sizeof reasons[0]; ++i)
        if (Equal(value, length, reasons[i])) return true;
    return false;
}
UmiStatus UmiBootReportParse(const char *text, size_t length, UmiBootReport *outReport)
{
    const char magic[] = "UMICOM_BOOT_REPORT 1\n";
    UmiBootReport result = {0};
    unsigned seen = 0;
    size_t cursor = sizeof magic - 1;
    if (!text || !outReport || length < cursor || length > UMI_BOOT_REPORT_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memcmp(text, magic, cursor) != 0 || text[length - 1] != '\n')
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < length; ++i)
        if ((unsigned char)text[i] < 32U && text[i] != '\n')
            return UMI_STATUS_INVALID_ARGUMENT;
    while (cursor < length) {
        size_t end = cursor;
        while (end < length && text[end] != '\n') ++end;
        const char *line = text + cursor;
        const char *equals = memchr(line, '=', end - cursor);
        if (!equals) return UMI_STATUS_INVALID_ARGUMENT;
        size_t keySize = (size_t)(equals - line);
        const char *value = equals + 1;
        size_t valueSize = (size_t)((text + end) - value);
        unsigned bit;
        if (Equal(line, keySize, "mode")) {
            bit = 1U;
            if (Equal(value, valueSize, "normal")) result.mode = UMI_BOOT_REPORT_NORMAL;
            else if (Equal(value, valueSize, "recovery")) result.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY;
            else return UMI_STATUS_INVALID_ARGUMENT;
        } else if (Equal(line, keySize, "state")) {
            bit = 2U;
            if (Equal(value, valueSize, "starting")) result.state = UMI_BOOT_REPORT_STARTING;
            else if (Equal(value, valueSize, "ready")) result.state = UMI_BOOT_REPORT_READY;
            else if (Equal(value, valueSize, "recovery")) result.state = UMI_BOOT_REPORT_RECOVERY;
            else return UMI_STATUS_INVALID_ARGUMENT;
        } else if (Equal(line, keySize, "planned")) {
            bit = 4U;
            if (!Number(value, valueSize, &result.planned)) return UMI_STATUS_INVALID_ARGUMENT;
        } else if (Equal(line, keySize, "completed")) {
            bit = 8U;
            if (!Number(value, valueSize, &result.completed)) return UMI_STATUS_INVALID_ARGUMENT;
        } else if (Equal(line, keySize, "reason")) {
            bit = 16U;
            if (!Reason(value, valueSize)) return UMI_STATUS_INVALID_ARGUMENT;
            memcpy(result.reason, value, valueSize);
        } else if (Equal(line, keySize, "source")) {
            bit = 32U;
            if (valueSize != 64U) return UMI_STATUS_INVALID_ARGUMENT;
            for (size_t i = 0; i < valueSize; ++i)
                if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'f')))
                    return UMI_STATUS_INVALID_ARGUMENT;
            memcpy(result.sourceId, value, valueSize);
        } else return UMI_STATUS_INVALID_ARGUMENT;
        if (seen & bit) return UMI_STATUS_INVALID_ARGUMENT;
        seen |= bit;
        cursor = end + 1;
    }
    if (seen != 63U || result.completed > result.planned) return UMI_STATUS_INVALID_ARGUMENT;
    if (result.state == UMI_BOOT_REPORT_RECOVERY) {
        if (strcmp(result.reason, "none") == 0) return UMI_STATUS_INVALID_ARGUMENT;
        if (result.mode == UMI_BOOT_REPORT_REQUESTED_RECOVERY &&
            result.completed != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (result.mode == UMI_BOOT_REPORT_NORMAL && strcmp(result.reason, "requested") == 0)
            return UMI_STATUS_INVALID_ARGUMENT;
    } else {
        if (result.mode != UMI_BOOT_REPORT_NORMAL || strcmp(result.reason, "none") != 0 || result.planned == 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (result.state == UMI_BOOT_REPORT_READY && result.completed != result.planned)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outReport = result;
    return UMI_STATUS_OK;
}
const char *UmiBootReportStateText(UmiBootReportState state)
{
    switch (state) {
    case UMI_BOOT_REPORT_STARTING: return "starting";
    case UMI_BOOT_REPORT_READY: return "ready";
    case UMI_BOOT_REPORT_RECOVERY: return "recovery";
    default: return "unknown";
    }
}
