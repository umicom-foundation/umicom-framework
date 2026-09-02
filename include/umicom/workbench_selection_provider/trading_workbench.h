/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/trading_workbench.h
 *
 * PURPOSE:
 *   Build the first complete Framework-owned Trader/TMS linked-workbench profile for instrument, account and trade workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADING_WORKBENCH_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADING_WORKBENCH_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/workbench_context_host/profile.h"
#include "umicom/workbench_context_source/trading_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench selection provider trading workbench data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchSelectionProviderTradingWorkbench {
    UmiWorkbenchContextHostProfile trader;
    UmiWorkbenchContextHostProfile tms;
    UmiWorkbenchContextSourceTradingProfile sources;
    char trading_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char operations_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchSelectionProviderTradingWorkbench;

/**
 * Provide the workbench selection provider trading workbench build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_trading_workbench_build(
    UmiWorkbenchSelectionProviderTradingWorkbench *out_profile);
/**
 * Check that workbench selection provider trading workbench satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_selection_provider_trading_workbench_validate(
    const UmiWorkbenchSelectionProviderTradingWorkbench *profile);

#ifdef __cplusplus
}
#endif
#endif
