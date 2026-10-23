/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/bank_operations/checkpoint.h
 * PURPOSE: Project a revision-bound commitment of retained local banking events.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BANK_OPERATIONS_CHECKPOINT_H
#define UMICOM_BANK_OPERATIONS_CHECKPOINT_H
#include "umicom/bank_operations/operations.h"
#include "umicom/record_chain/chain.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Commit to the entire serial service's cached event history, using the
 * existing UBO1 canonical text bytes (excluding the terminator), kind 0x55424f31.
 * expected_revision must equal the observed view. No database is read or
 * changed and no payment is executed. Reload explicitly to observe persisted
 * changes made by another handle. Compare with an independently saved anchor;
 * recomputing after an alteration alone cannot establish authenticity.
 * The owner supplies a stable nonzero chain ID per database and codec schema.
 * out remains unchanged on failure. Keep hashes and their private source data
 * under the same access policy; no public-chain publication is implied. */
UmiStatus UmiBankOperationsCheckpoint(const UmiBankOperations *operations,
    const unsigned char chain_id[32], uint64_t expected_revision,
    UmiRecordChainCheckpoint *out);
#ifdef __cplusplus
}
#endif
#endif
