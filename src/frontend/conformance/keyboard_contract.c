/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/keyboard_contract.c
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
#include "umicom/frontend/conformance/keyboard_contract.h"

/*
 * Check that fc keyboard contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_keyboard_contract_validate(const UmiFcKeyboardContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->command_count>0U && item->navigation_count>0U && item->shortcuts_documented;}
