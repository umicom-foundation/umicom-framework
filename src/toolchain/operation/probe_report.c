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
