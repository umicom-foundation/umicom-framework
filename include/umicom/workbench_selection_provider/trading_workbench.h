/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/trading_workbench.h
 *
 * PURPOSE:
 *   Build the first complete Framework-owned Trader/TMS linked-workbench profile for instrument, account and trade workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADING_WORKBENCH_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_TRADING_WORKBENCH_H

#include "umicom/workbench_selection_provider/types.h"
#include "umicom/workbench_context_host/profile.h"
#include "umicom/workbench_context_source/trading_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchSelectionProviderTradingWorkbench {
    UmiWorkbenchContextHostProfile trader;
    UmiWorkbenchContextHostProfile tms;
    UmiWorkbenchContextSourceTradingProfile sources;
    char trading_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char operations_group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t revision;
} UmiWorkbenchSelectionProviderTradingWorkbench;

UmiStatus umi_workbench_selection_provider_trading_workbench_build(
    UmiWorkbenchSelectionProviderTradingWorkbench *out_profile);
UmiStatus umi_workbench_selection_provider_trading_workbench_validate(
    const UmiWorkbenchSelectionProviderTradingWorkbench *profile);

#ifdef __cplusplus
}
#endif
#endif
