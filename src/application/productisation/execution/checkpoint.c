/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/checkpoint.c
 *
 * PURPOSE:
 *   Capture compact queue-state summaries for resume and operational evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/checkpoint.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise product execution checkpoint ledger from caller-provided values so later
 * operations receive a known state.
 */
void umi_product_execution_checkpoint_ledger_init(
    UmiProductExecutionCheckpointLedger *ledger)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger != NULL) {
        (void)memset(ledger, 0, sizeof(*ledger));
        ledger->next_revision = 1U;
    }
}
/*
 * Provide the product execution checkpoint capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_checkpoint_capture(
    UmiProductExecutionCheckpointLedger *ledger,
    const UmiProductExecutionWorkQueue *queue,
    const char *label)
{
    size_t i;
    UmiProductExecutionCheckpoint *cp;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || queue == NULL || label == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (ledger->count >= UMI_PRODUCT_EXECUTION_MAX_CHECKPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    cp = &ledger->checkpoints[ledger->count++];
    (void)memset(cp, 0, sizeof(*cp));
    cp->revision = ledger->next_revision++;
    (void)snprintf(cp->label, sizeof(cp->label), "%s", label);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < queue->count; ++i) {
        /* Select the behaviour associated with the requested command or state value. */
        switch (queue->items[i].state) {
        case UMI_PRODUCT_EXECUTION_SUCCEEDED: cp->succeeded++; break;
        case UMI_PRODUCT_EXECUTION_FAILED: cp->failed++; break;
        case UMI_PRODUCT_EXECUTION_BLOCKED: cp->blocked++; break;
        case UMI_PRODUCT_EXECUTION_CANCELLED: cp->cancelled++; break;
        default: break;
        }
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution checkpoint latest operation used by this module and its
 * client applications.
 */
const UmiProductExecutionCheckpoint *umi_product_execution_checkpoint_latest(
    const UmiProductExecutionCheckpointLedger *ledger)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (ledger == NULL || ledger->count == 0U) return NULL;
    return &ledger->checkpoints[ledger->count - 1U];
}
