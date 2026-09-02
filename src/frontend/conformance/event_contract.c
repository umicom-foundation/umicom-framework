/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/event_contract.c
 *
 * PURPOSE:
 *   semantic user-event support requirements independent of native toolkit event classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/event_contract.h"

/* Check that fc event contract satisfies its contract before another service relies on it. */
bool umi_fc_event_contract_validate(const UmiFcEventContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_families!=0U && item->ordered;}
