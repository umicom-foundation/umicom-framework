/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/focus_contract.h
 *
 * PURPOSE:
 *   focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFcFocusContract { size_t focusable_count; size_t traversal_count; bool modal_trap_required; } UmiFcFocusContract;
bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item);

#ifdef __cplusplus
}
#endif
#endif
