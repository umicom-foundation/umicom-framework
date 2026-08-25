/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/focus_contract.c
 *
 * PURPOSE:
 *   focusable-element ordering and focus-trap requirements for interactive surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/focus_contract.h"

bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item){if(item==NULL)return false;return item->traversal_count>=item->focusable_count;}
