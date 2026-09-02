/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_policy.c
 *
 * PURPOSE:
 *   automatic/manual update eligibility, metered-network and maintenance-window policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_policy.h"

/*
 * Initialise dr update policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_update_policy_init(UmiDrUpdatePolicy*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){*p=(UmiDrUpdatePolicy){0};p->require_signature=true;}}bool umi_dr_update_policy_eligible(const UmiDrUpdatePolicy*p,bool metered,bool in_window,bool signed_update){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->require_signature&&!signed_update)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(metered&&!p->allow_metered)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->maintenance_window&&!in_window)return false;return true;}
