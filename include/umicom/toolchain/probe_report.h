/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/probe_report.h
 *
 * PURPOSE:
 *   Record one native executable discovery/validation result without losing diagnostics.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_PROBE_REPORT_H
#define INCLUDE_UMICOM_TOOLCHAIN_PROBE_REPORT_H
#include "umicom/base/status.h"
#include "umicom/base/value_archive.h"
#include "umicom/toolchain/tool.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_PROBE_OUTPUT_CAPACITY 1024U

/**
 * Represent the toolchain probe report data shared with callers of this public contract.
 */
typedef struct UmiToolchainProbeReport {
    UmiToolKind kind;
    UmiStatus status;
    int found;
    int validated;
    int from_explicit_root;
    char path[UMI_TOOL_PATH_CAPACITY];
    char version[UMI_TOOL_VERSION_CAPACITY];
    char detail[UMI_TOOLCHAIN_PROBE_OUTPUT_CAPACITY];
} UmiToolchainProbeReport;

/**
 * Initialise toolchain probe report from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_probe_report_init(UmiToolchainProbeReport *report,
                                     UmiToolKind kind);
/**
 * Check that toolchain probe report satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_toolchain_probe_report_validate(const UmiToolchainProbeReport *report);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_toolchain_probe_report_archive_encode(const UmiToolchainProbeReport *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_toolchain_probe_report_archive_decode(const void *bytes, size_t byte_count,
    UmiToolchainProbeReport *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiToolchainProbeReport object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_toolchain_probe_report_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiToolchainProbeReport *workspace, UmiToolchainProbeReport *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_toolchain_probe_report_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiToolchainProbeReport *expected, UmiToolchainProbeReport *value);


#ifdef __cplusplus
}
#endif
#endif
