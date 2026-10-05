/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_operation/test_probe_report_contract.c
 *
 * PURPOSE:
 *   Verify the public contract for toolchain operation module probe_report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/probe_report.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/toolchain/probe_report.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiToolchainProbeReportTransferEqual(const UmiToolchainProbeReport *a, const UmiToolchainProbeReport *b)
{
    return a->kind == b->kind &&
        a->status == b->status &&
        a->found == b->found &&
        a->validated == b->validated &&
        a->from_explicit_root == b->from_explicit_root &&
        strcmp(a->path, b->path) == 0 &&
        strcmp(a->version, b->version) == 0 &&
        strcmp(a->detail, b->detail) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiToolchainProbeReportTransferTails(UmiToolchainProbeReport *value)
{
    (void)value;
    {
        size_t used = strlen(value->path) + 1U;
        memset(value->path + used, 0xa5, sizeof(value->path) - used);
    }
    {
        size_t used = strlen(value->version) + 1U;
        memset(value->version + used, 0xa5, sizeof(value->version) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiToolchainProbeReportTransferMalformed(const UmiToolchainProbeReport *sample)
{
    (void)sample;
    {
        UmiToolchainProbeReport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.path, 'x', sizeof(invalid.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_toolchain_probe_report_validate(&invalid) != UMI_STATUS_OK) ||
            umi_toolchain_probe_report_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated path was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiToolchainProbeReport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.version, 'x', sizeof(invalid.version));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_toolchain_probe_report_validate(&invalid) != UMI_STATUS_OK) ||
            umi_toolchain_probe_report_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated version was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiToolchainProbeReport invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.detail, 'x', sizeof(invalid.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_toolchain_probe_report_validate(&invalid) != UMI_STATUS_OK) ||
            umi_toolchain_probe_report_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated detail was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiToolchainProbeReportTransferCases, UmiToolchainProbeReport,
    umi_toolchain_probe_report_archive_encode, umi_toolchain_probe_report_archive_decode,
    UmiToolchainProbeReportTransferEqual, UmiToolchainProbeReportTransferTails, UmiToolchainProbeReportTransferMalformed)

int main(void){ UmiToolchainProbeReport r; umi_toolchain_probe_report_init(&r,UMI_TOOL_GIT); r.found=1; r.validated=1; strcpy(r.path,"git"); assert(umi_toolchain_probe_report_validate(&r)==UMI_STATUS_OK);
    if (UmiToolchainProbeReportTransferCases(&r) != 0) return 1;
 return 0; }
