/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/accounting/double_entry.c
 *
 * PURPOSE:
 *   Apply Treasury Double-entry Balance without duplicating canonical finance/treasury state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../../base/policy_batch_internal.h"
#include "umicom/open_tms/accounting/double_entry.h"
#include "../open_tms_internal.h"

UmiStatus umi_open_tms_evaluate_accounting_double_entry(
    const UmiOpenTmsInput *input,
    UmiOpenTmsSnapshot *outSnapshot)
{
    static const UmiOpenTmsPolicy policy = {
        "accounting.double-entry",
        "Treasury Double-entry Balance",
        UMI_OPEN_TMS_POLICY_BALANCE,
        100.000000,
        99.000000,
        0.250,
        0.250,
        0.250,
        0.250
    };
    return umi_open_tms_evaluate_policy(&policy, input, outSnapshot);
}

/* Reuse the scalar policy for complete table refreshes; the shared batch
 * owner stages all rows before publishing a successful calculation. */
UMI_DEFINE_POLICY_BATCH(umi_open_tms_evaluate_accounting_double_entry_batch, umi_open_tms_evaluate_accounting_double_entry,
    UmiOpenTmsInput, UmiOpenTmsSnapshot)
