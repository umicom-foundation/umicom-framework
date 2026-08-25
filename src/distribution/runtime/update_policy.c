/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_policy.c
 *
 * PURPOSE:
 *   automatic/manual update eligibility, metered-network and maintenance-window policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_policy.h"

void umi_dr_update_policy_init(UmiDrUpdatePolicy*p){if(p!=NULL){*p=(UmiDrUpdatePolicy){0};p->require_signature=true;}}bool umi_dr_update_policy_eligible(const UmiDrUpdatePolicy*p,bool metered,bool in_window,bool signed_update){if(p==NULL)return false;if(p->require_signature&&!signed_update)return false;if(metered&&!p->allow_metered)return false;if(p->maintenance_window&&!in_window)return false;return true;}
