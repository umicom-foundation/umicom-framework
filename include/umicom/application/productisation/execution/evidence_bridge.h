/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/evidence_bridge.h
 *
 * PURPOSE:
 *   Translate execution outcomes into the existing productisation evidence ledger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EVIDENCE_BRIDGE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_EVIDENCE_BRIDGE_H
#include "umicom/application/productisation/execution/work_item.h"
#include "umicom/application/productisation/evidence.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiProductisationEvidenceState umi_product_execution_evidence_state(
    const UmiProductExecutionWorkItem *item);
UmiStatus umi_product_execution_record_evidence(
    UmiProductisationEvidenceLedger *ledger,
    const UmiProductExecutionWorkItem *item);
#ifdef __cplusplus
}
#endif
#endif
