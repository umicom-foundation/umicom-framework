/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance_operations/checkpoint.h
 *
 * PURPOSE:
 *   Expose read-only commitments over the canonical financial operations
 *   history, without creating a parallel transaction or database authority.
 *
 * SECURITY:
 *   Hash commitments cannot authenticate their producer. The caller must
 *   retain the expected tip outside the dataset being verified. Recompute
 *   only from a serially accessed and internally consistent service view.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_OPERATIONS_CHECKPOINT_H
#define UMICOM_FINANCE_OPERATIONS_CHECKPOINT_H

#include <stdint.h>
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
UmiStatus UmiFinanceOperationsCheckpoint(
    const UmiFinanceOperations *operations,
    const unsigned char chainId[UMI_RECORD_CHAIN_ID_BYTES],
    uint64_t expectedRevision,
    UmiRecordChainCheckpoint *outCheckpoint);
#ifdef __cplusplus
}
#endif
#endif /* UMICOM_FINANCE_OPERATIONS_CHECKPOINT_H */
