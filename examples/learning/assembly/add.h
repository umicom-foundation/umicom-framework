/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/assembly/add.h
 *
 * PURPOSE:
 *   Declare the same unsigned arithmetic contract for C and Assembly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LEARNING_ADD_H
#define UMICOM_LEARNING_ADD_H
#include <stdint.h>

/* Results wrap modulo 2^32; this is not signed-overflow arithmetic. */
uint32_t AddTwoC(uint32_t left, uint32_t right);
uint32_t AddTwoAssembly(uint32_t left, uint32_t right);
#endif
