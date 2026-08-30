/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/selection_contract.c
 *
 * PURPOSE:
 *   single, multiple and range selection semantics for list, tree, grid and editor surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/selection_contract.h"

bool umi_fc_selection_contract_validate(const UmiFcSelectionContract *item){if(item==NULL)return false;return item->required_modes!=0U && item->preserve_on_refresh;}
