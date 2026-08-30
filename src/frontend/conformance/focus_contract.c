/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/focus_contract.c
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
#include "umicom/frontend/conformance/focus_contract.h"

bool umi_fc_focus_contract_validate(const UmiFcFocusContract *item){if(item==NULL)return false;return item->traversal_count>=item->focusable_count;}
