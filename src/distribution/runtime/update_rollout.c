/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/update_rollout.c
 *
 * PURPOSE:
 *   deterministic staged-rollout cohort selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/update_rollout.h"

uint32_t umi_dr_update_rollout_bucket(const char*id){return id==NULL?100U:(uint32_t)(umi_dr_hash_text(id)%100U);}bool umi_dr_update_rollout_selected(const char*id,uint32_t p){if(p>100U)p=100U;return umi_dr_update_rollout_bucket(id)<p;}
