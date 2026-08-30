/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/channel_policy.c
 *
 * PURPOSE:
 *   allowed channel transitions and downgrade policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/channel_policy.h"

void umi_dr_channel_policy_init(UmiDrChannelPolicy*p){if(p!=NULL){*p=(UmiDrChannelPolicy){0};p->allow_prerelease_to_stable=true;}}bool umi_dr_channel_policy_transition(const UmiDrChannelPolicy*p,UmiDrChannelKind from,UmiDrChannelKind to){if(p==NULL||from==0||to==0)return false;if(to>=from)return true;return p->allow_downgrade;}
