/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/focus_contract.h
 *
 * PURPOSE:
 *   focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_FOCUS_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_FOCUS_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc focus contract data shared with callers of this public contract.
 */
typedef struct UmiFcFocusContract { size_t focusable_count; size_t traversal_count; bool modal_trap_required; } UmiFcFocusContract;
/**
 * Check that fc focus contract satisfies its contract before another service relies on it.
 */
bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item);

#ifdef __cplusplus
}
#endif
#endif
