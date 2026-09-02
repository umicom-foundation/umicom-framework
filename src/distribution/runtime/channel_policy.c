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

/*
 * Initialise dr channel policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_channel_policy_init(UmiDrChannelPolicy*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){*p=(UmiDrChannelPolicy){0};p->allow_prerelease_to_stable=true;}}bool umi_dr_channel_policy_transition(const UmiDrChannelPolicy*p,UmiDrChannelKind from,UmiDrChannelKind to){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||from==0||to==0)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(to>=from)return true;return p->allow_downgrade;}
