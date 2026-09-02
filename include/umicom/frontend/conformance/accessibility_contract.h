/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/accessibility_contract.h
 *
 * PURPOSE:
 *   required semantic accessibility roles, names, states and keyboard affordances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fc accessibility contract data shared with callers of this public
 * contract.
 */
typedef struct UmiFcAccessibilityContract { uint64_t required_roles; bool named; bool keyboard_reachable; bool state_exposed; } UmiFcAccessibilityContract;
/**
 * Check that fc accessibility contract satisfies its contract before another service
 * relies on it.
 */
bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item);

#ifdef __cplusplus
}
#endif
#endif
