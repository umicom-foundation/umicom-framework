/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/context_contract.c
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_contract.h"

bool umi_fc_context_contract_validate(const UmiFcContextContract *item){if(item==NULL)return false;return item->required_types!=0U && item->accessible_label;}
