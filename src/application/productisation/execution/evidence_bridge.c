/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/evidence_bridge.c
 *
 * PURPOSE:
 *   Project completion outcomes into canonical productisation acceptance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/evidence_bridge.h"
/*
 * Provide the product execution evidence state operation used by this module and its
 * client applications.
 */
UmiProductisationEvidenceState umi_product_execution_evidence_state(
    const UmiProductExecutionWorkItem *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_PRODUCTISATION_EVIDENCE_BLOCKED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->state == UMI_PRODUCT_EXECUTION_SUCCEEDED) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->step.stage == UMI_PRODUCTISATION_STAGE_ACCEPTANCE)
            return UMI_PRODUCTISATION_EVIDENCE_ACCEPTED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->step.owner == UMI_PRODUCTISATION_OWNER_APPLICATION)
            return UMI_PRODUCTISATION_EVIDENCE_ADOPTED;
        return UMI_PRODUCTISATION_EVIDENCE_IMPLEMENTED;
    }
    return UMI_PRODUCTISATION_EVIDENCE_BLOCKED;
}
/*
 * Provide the product execution record evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_record_evidence(
    UmiProductisationEvidenceLedger *ledger,
    const UmiProductExecutionWorkItem *item)
{
    const char *application_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || item == NULL || item->step.asset_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    application_id = item->step.application_id[0] != '\0'
        ? item->step.application_id : "framework";
    return umi_productisation_evidence_record(
        ledger,
        application_id,
        UMI_PRODUCTISATION_ASSET_FEATURE,
        item->step.asset_id,
        "",
        umi_product_execution_evidence_state(item),
        item->evidence_reference);
}
