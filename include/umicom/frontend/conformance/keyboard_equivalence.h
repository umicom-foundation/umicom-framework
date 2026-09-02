/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/keyboard_equivalence.h
 *
 * PURPOSE:
 *   keyboard command coverage scoring and mandatory shortcut detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_EQUIVALENCE_H
#define UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_EQUIVALENCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc keyboard equivalence data shared with callers of this public contract.
 */
typedef struct UmiFcKeyboardEquivalence { size_t required_commands; size_t implemented_commands; size_t mandatory_missing; } UmiFcKeyboardEquivalence;
/**
 * Provide the fc keyboard equivalence evaluate operation used by this module and its
 * client applications.
 */
double umi_fc_keyboard_equivalence_evaluate(const UmiFcKeyboardEquivalence *item);
/**
 * Provide the fc keyboard equivalence passes operation used by this module and its client
 * applications.
 */
bool umi_fc_keyboard_equivalence_passes(const UmiFcKeyboardEquivalence *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
