/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/keyboard_contract.c
 *
 * PURPOSE:
 *   required command and navigation keyboard coverage for workstation surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/keyboard_contract.h"

bool umi_fc_keyboard_contract_validate(const UmiFcKeyboardContract *item){if(item==NULL)return false;return item->command_count>0U && item->navigation_count>0U && item->shortcuts_documented;}
