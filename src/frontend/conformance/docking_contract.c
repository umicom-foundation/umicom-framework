/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/docking_contract.c
 *
 * PURPOSE:
 *   dock zones, floating, auto-hide and split/tab workstation requirements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/docking_contract.h"

/*
 * Check that fc docking contract satisfies its contract before another service relies on
 * it.
 */
bool umi_fc_docking_contract_validate(const UmiFcDockingContract *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return false;return item->required_features!=0U && item->allowed_zones!=0U;}
