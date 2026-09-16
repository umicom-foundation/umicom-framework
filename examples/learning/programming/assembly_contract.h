/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/assembly_contract.h
 *
 * PURPOSE:
 *   Describe the native Assembly teaching ABI without toolkit types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_ASSEMBLY_CONTRACT_H
#define UMICOM_LEARNING_ASSEMBLY_CONTRACT_H
#include <stddef.h>
#include <stdint.h>
/* Leaf functions; no ownership transfer. Native functions are optional. */
uint64_t LabMaximumAsm(uint64_t left, uint64_t right);
/* values may be NULL only when count is zero; addition is modulo 2^64. */
uint64_t LabSumAsm(const uint64_t *values, size_t count);
#endif
