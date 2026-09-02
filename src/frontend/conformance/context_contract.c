/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/context_contract.c
 *
 * PURPOSE:
 *   typed context-channel requirements for linked cross-application surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/context_contract.h"

/*
 * Check that fc context contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_context_contract_validate(const UmiFcContextContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_types!=0U && item->accessible_label;}
