/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/event_contract.c
 *
 * PURPOSE:
 *   semantic user-event support requirements independent of native toolkit event classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_contract.h"

bool umi_fc_event_contract_validate(const UmiFcEventContract *item){if(item==NULL)return false;return item->required_families!=0U && item->ordered;}
