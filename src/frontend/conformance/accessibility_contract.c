/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/accessibility_contract.c
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
#include "umicom/frontend/conformance/accessibility_contract.h"

/*
 * Check that fc accessibility contract satisfies its contract before another service
 * relies on it.
 */
bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->named && item->keyboard_reachable && item->state_exposed;}
