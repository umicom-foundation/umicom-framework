/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rules/arithmetic.h
 *
 * PURPOSE:
 *   Declare the arithmetic CodeGuard rule pack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_RULES_ARITHMETIC_H
#define UMICOM_CODEGUARD_RULES_ARITHMETIC_H
#include <stddef.h>
#include "umicom/codeguard/rule.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiCodeGuardRule *umi_codeguard_rules_arithmetic(size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
