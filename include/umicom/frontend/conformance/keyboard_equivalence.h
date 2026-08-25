/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/keyboard_equivalence.h
 *
 * PURPOSE:
 *   keyboard command coverage scoring and mandatory shortcut detection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcKeyboardEquivalence { size_t required_commands; size_t implemented_commands; size_t mandatory_missing; } UmiFcKeyboardEquivalence;
double umi_fc_keyboard_equivalence_evaluate(const UmiFcKeyboardEquivalence *item);
bool umi_fc_keyboard_equivalence_passes(const UmiFcKeyboardEquivalence *item,double minimum);

#ifdef __cplusplus
}
#endif
#endif
