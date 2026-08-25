/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/selection_contract.c
 *
 * PURPOSE:
 *   single, multiple and range selection semantics for list, tree, grid and editor surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/selection_contract.h"

bool umi_fc_selection_contract_validate(const UmiFcSelectionContract *item){if(item==NULL)return false;return item->required_modes!=0U && item->preserve_on_refresh;}
