/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/docking_contract.c
 *
 * PURPOSE:
 *   dock zones, floating, auto-hide and split/tab workstation requirements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_contract.h"

bool umi_fc_docking_contract_validate(const UmiFcDockingContract *item){if(item==NULL)return false;return item->required_features!=0U && item->allowed_zones!=0U;}
