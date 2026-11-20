/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/native_kernel_abi_report.c
 *
 * PURPOSE:
 *   Admit framed native Kernel ABI metadata without loading guest code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/native_kernel_abi_report.h"
#include "umicom/base/ascii_number.h"
#include "umicom/platform/input_file.h"
#include <stdbool.h>
#include <string.h>

typedef struct AbiReader {
    const char *next;
    size_t remaining;
} AbiReader;

static bool ReadLine(AbiReader *reader, const char **line, size_t *size)
{
    /* Only a real line terminator advances the view. Strip the optional CR
     * that belongs to serial CRLF and leave all other bytes to the grammar. */
    const char *end = memchr(reader->next, '\n', reader->remaining);
    if (end == NULL) return false;
    *line = reader->next;
    *size = (size_t)(end - reader->next);
    reader->remaining -= *size + 1U;
    reader->next = end + 1;
    if (*size > 0U && end[-1] == '\r') --*size;
    return true;
}

static bool ReadValue(AbiReader *reader, const char *prefix, const char **value, size_t *length)
{
    /* Match the complete field prefix before exposing a borrowed value span. */
    size_t prefixLength = strlen(prefix);
    if (!ReadLine(reader, value, length) || *length <= prefixLength ||
        memcmp(*value, prefix, prefixLength) != 0) return false;
    *value += prefixLength;
    *length -= prefixLength;
    return true;
}

static bool Decimal(const char *text, size_t length, uint64_t *out)
{
    /* Kernel emits canonical unsigned decimal. Reuse Framework's checked
     * conversion and retain the original spelling rule for a leading zero. */
    return (length <= 1U || text[0] != '0') &&
        umi_ascii_uint64_parse(text, length, out) == UMI_STATUS_OK;
}

UmiStatus UmiNativeKernelAbiReportParse(const char *bytes, size_t length,
    UmiNativeKernelAbiReport *outReport)
{
    if (bytes == NULL || length == 0U || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_NATIVE_KERNEL_ABI_REPORT_MAX_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Admit the record envelope separately from the guest ABI version. */
    AbiReader reader = {bytes, length};
    const char *text;
    size_t size;
    uint64_t schema = 0U;
    if (!ReadValue(&reader, "UMICOM_NATIVE_ABI_REPORT ", &text, &size) ||
        !Decimal(text, size, &schema) || schema == 0U) return UMI_STATUS_PARSE_ERROR;
    if (schema != 1U) return UMI_STATUS_UNAVAILABLE;

    /* The ABI version is major.minor, with both values fitting the Kernel's
     * native version word. Keep future major versions visible to the caller. */
    if (!ReadValue(&reader, "abi.version=", &text, &size)) return UMI_STATUS_PARSE_ERROR;
    const char *dot = memchr(text, '.', size);
    uint64_t major = 0U, minor = 0U;
    if (dot == NULL || !Decimal(text, (size_t)(dot - text), &major) ||
        !Decimal(dot + 1, size - (size_t)(dot + 1 - text), &minor) ||
        major == 0U || major > UINT16_MAX || minor > UINT16_MAX) return UMI_STATUS_PARSE_ERROR;
    UmiNativeKernelAbiReport report = {.major = (uint16_t)major, .minor = (uint16_t)minor};

    /* A stable field order makes duplicate, missing and unknown rows fail
     * without a second interpretation. The complete mask is retained. */
    static const char *const fields[] = {"abi.word-bits=", "abi.byte-order=", "abi.page-bytes=",
        "abi.copy-limit=", "abi.call-limit=", "abi.compiled-features="};
    uint64_t *values[] = {&report.wordBits, &report.byteOrder, &report.pageBytes,
        &report.copyLimit, &report.callLimit, &report.compiledFeatures};
    for (size_t index = 0U; index < sizeof fields / sizeof fields[0]; ++index) {
        if (!ReadValue(&reader, fields[index], &text, &size) || !Decimal(text, size, values[index]))
            return UMI_STATUS_PARSE_ERROR;
    }

    /* Validate the schema's physical units and require one complete frame,
     * excluding command echoes, console prompts and trailing bytes. */
    if ((report.wordBits != 32U && report.wordBits != 64U) ||
        (report.byteOrder != 1U && report.byteOrder != 2U) ||
        report.pageBytes == 0U || (report.pageBytes & (report.pageBytes - 1U)) != 0U ||
        report.callLimit == 0U) return UMI_STATUS_PARSE_ERROR;
    static const char closing[] = "UMICOM_NATIVE_ABI_END";
    if (!ReadLine(&reader, &text, &size) || size != sizeof closing - 1U ||
        memcmp(text, closing, size) != 0 || reader.remaining != 0U) return UMI_STATUS_PARSE_ERROR;
    *outReport = report;
    return UMI_STATUS_OK;
}

UmiStatus UmiNativeKernelAbiReportLoad(const char *absolutePath,
    UmiNativeKernelAbiReport *outReport)
{
    /* Reuse native ownership and regular-file admission. The parser stages
     * the value and the temporary read buffer is released on all outcomes. */
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    unsigned char *bytes = NULL;
    size_t length = 0U;
    UmiStatus status = UmiInputFileRead(absolutePath, UMI_NATIVE_KERNEL_ABI_REPORT_MAX_BYTES, &bytes, &length);
    if (status == UMI_STATUS_OK) status = UmiNativeKernelAbiReportParse((const char *)bytes, length, outReport);
    UmiInputFileFree(bytes);
    return status;
}
