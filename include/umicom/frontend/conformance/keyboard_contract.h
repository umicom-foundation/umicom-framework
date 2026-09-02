/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/keyboard_contract.h
 *
 * PURPOSE:
 *   required command and navigation keyboard coverage for workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_CONTRACT_H
#define UMICOM_FRONTEND_CONFORMANCE_KEYBOARD_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc keyboard contract data shared with callers of this public contract.
 */
typedef struct UmiFcKeyboardContract { size_t command_count; size_t navigation_count; bool shortcuts_documented; } UmiFcKeyboardContract;
/**
 * Check that fc keyboard contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_keyboard_contract_validate(const UmiFcKeyboardContract *item);

#ifdef __cplusplus
}
#endif
#endif
