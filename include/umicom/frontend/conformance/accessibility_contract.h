/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/accessibility_contract.h
 *
 * PURPOSE:
 *   required semantic accessibility roles, names, states and keyboard affordances.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_ACCESSIBILITY_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcAccessibilityContract { uint64_t required_roles; bool named; bool keyboard_reachable; bool state_exposed; } UmiFcAccessibilityContract;
bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item);

#ifdef __cplusplus
}
#endif
#endif
