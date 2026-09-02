/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/quant/random_sequence.h
 *
 * PURPOSE:
 *   Generate deterministic reproducible uniform pseudo-random sequences.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_QUANT_RANDOM_SEQUENCE_H
#define INCLUDE_UMICOM_FINANCE_QUANT_RANDOM_SEQUENCE_H

#include "umicom/finance/quant/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the quant random sequence data shared with callers of this public contract.
 */
typedef struct UmiQuantRandomSequence { uint64_t state; } UmiQuantRandomSequence;
/* Seed a deterministic xorshift sequence. */
UmiStatus umi_quant_random_sequence_init(UmiQuantRandomSequence *sequence, uint64_t seed);
/* Return the next uniform value in [0,1). */
double umi_quant_random_sequence_next(UmiQuantRandomSequence *sequence);

#ifdef __cplusplus
}
#endif

#endif
