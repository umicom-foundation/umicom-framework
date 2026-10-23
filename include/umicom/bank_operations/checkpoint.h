/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/checkpoint.h
 *
 * PURPOSE:
 *   Read-only, cryptographically committed snapshots of the canonical bank
 *   event history for review and independent retention.
 *
 * OWNERSHIP AND TRUST:
 *   Umicom Bank Operations remains the sole authority for accepted commands,
 *   replay and persistence. This function merely derives a record-chain tip
 *   from its current immutable event view using the existing BankEncode codec.
 *   A checkpoint is a content commitment, NOT a signature or authority to
 *   change balances. Callers protect the expected tip independently.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_OPERATIONS_CHECKPOINT_H
#define UMICOM_BANK_OPERATIONS_CHECKPOINT_H

#include <stdint.h>
#include "umicom/bank_operations/operations.h"
#include "umicom/record_chain/chain.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Produce a deterministic checkpoint at the exact currently observed
 * revision. The chain identity must be an independently chosen nonzero 32-byte
 * value. The caller serialises access to operations; concurrent modifications
 * are not supported. Empty history returns the canonical genesis checkpoint.
 *
 * A stale expectedRevision returns UMI_STATUS_BUSY. A poisoned, inconsistent
 * or malformed event view is refused as UMI_STATUS_INVALID_STATE. On every
 * failure, *outCheckpoint remains exactly unchanged. No database writes or
 * application-local hashing implementation are performed. */
UmiStatus UmiBankOperationsCheckpoint(
    const UmiBankOperations *operations,
    const unsigned char chainId[UMI_RECORD_CHAIN_ID_BYTES],
    uint64_t expectedRevision,
    UmiRecordChainCheckpoint *outCheckpoint);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BANK_OPERATIONS_CHECKPOINT_H */
