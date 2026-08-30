/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/drag_drop_contract.c
 *
 * PURPOSE:
 *   semantic drag/drop operation, keyboard alternative and docking affordance requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/drag_drop_contract.h"

bool umi_fc_drag_drop_contract_validate(const UmiFcDragDropContract *item){if(item==NULL)return false;return item->required_ops!=0U && item->keyboard_alternative;}
