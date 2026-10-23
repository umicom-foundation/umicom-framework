/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_operations/checkpoint.h
 * PURPOSE: Expose revision-bound accounting and exchange history commitments.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_CHECKPOINT_H
#define UMICOM_FINANCE_OPERATIONS_CHECKPOINT_H
#include "umicom/finance_operations/operations.h"
#include "umicom/record_chain/chain.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Commit to retained UFIN1 canonical command text, excluding the terminator,
 * with kind 0x55464931. Includes accounting, simulated matching and settlement.
 * The serial service must have a valid view at expected_revision; a failed
 * reload blocks this export until a successful reload. No I/O or mutation.
 * out is unchanged on refusal. The stable, nonzero chain ID and independently
 * retained expected checkpoint belong to the host's private source identity.
 * This proves equality with an anchor, not real settlement or actor identity. */
UmiStatus UmiFinanceOperationsCheckpoint(const UmiFinanceOperations *operations,
    const unsigned char chain_id[32], uint64_t expected_revision,
    UmiRecordChainCheckpoint *out);
#ifdef __cplusplus
}
#endif
#endif
