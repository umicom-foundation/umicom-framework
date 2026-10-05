/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/probe_report.c
 *
 * PURPOSE:
 *   Implement deterministic native tool probe report defaults and validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/probe_report.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/*
 * Initialise toolchain probe report from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_probe_report_init(UmiToolchainProbeReport *report,
                                     UmiToolKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    report->kind = kind;
    report->status = UMI_STATUS_NOT_FOUND;
}

/*
 * Check that toolchain probe report satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_toolchain_probe_report_validate(const UmiToolchainProbeReport *report)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(report->path, '\0', sizeof(report->path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(report->version, '\0', sizeof(report->version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(report->detail, '\0', sizeof(report->detail)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (report == NULL || report->kind < 0 || report->kind >= UMI_TOOL_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (report->validated && (!report->found || report->path[0] == '\0')) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiToolchainProbeReportArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1d9d7eb5bc4b125c);
    schema = (schema ^ (uint64_t)sizeof(((UmiToolchainProbeReport *)0)->path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiToolchainProbeReport *)0)->version)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiToolchainProbeReport *)0)->detail)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiToolchainProbeReportArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U + sizeof(((UmiToolchainProbeReport *)0)->path) - 1U +
        8U + sizeof(((UmiToolchainProbeReport *)0)->version) - 1U +
        8U + sizeof(((UmiToolchainProbeReport *)0)->detail) - 1U;
}
static void UmiToolchainProbeReportArchiveWrite(UmiArchiveWriter *writer, const UmiToolchainProbeReport *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteSigned(writer, (int64_t)value->found);
    UmiArchiveWriteSigned(writer, (int64_t)value->validated);
    UmiArchiveWriteSigned(writer, (int64_t)value->from_explicit_root);
    UmiArchiveWriteText(writer, value->path, sizeof(value->path));
    UmiArchiveWriteText(writer, value->version, sizeof(value->version));
    UmiArchiveWriteText(writer, value->detail, sizeof(value->detail));
}
static void UmiToolchainProbeReportArchiveRead(UmiArchiveReader *reader, UmiToolchainProbeReport *value)
{
    value->kind = (UmiToolKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->found = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->validated = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->from_explicit_root = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    UmiArchiveReadText(reader, value->path, sizeof(value->path));
    UmiArchiveReadText(reader, value->version, sizeof(value->version));
    UmiArchiveReadText(reader, value->detail, sizeof(value->detail));
}
static UmiStatus UmiToolchainProbeReportArchiveValidate(const UmiToolchainProbeReport *value)
{
    return umi_toolchain_probe_report_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_toolchain_probe_report_archive_encode, umi_toolchain_probe_report_archive_decode,
    UmiToolchainProbeReport, UmiToolchainProbeReportArchiveSchema, UmiToolchainProbeReportArchiveBound, UmiToolchainProbeReportArchiveWrite, UmiToolchainProbeReportArchiveRead, UmiToolchainProbeReportArchiveValidate)
