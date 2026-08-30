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

bool umi_fc_accessibility_contract_validate(const UmiFcAccessibilityContract *item){if(item==NULL)return false;return item->named && item->keyboard_reachable && item->state_exposed;}
