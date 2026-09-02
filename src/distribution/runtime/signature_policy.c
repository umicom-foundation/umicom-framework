/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/signature_policy.c
 *
 * PURPOSE:
 *   signature requirement and trusted-publisher policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/signature_policy.h"

/*
 * Initialise dr signature policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_signature_policy_init(UmiDrSignaturePolicy*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){*p=(UmiDrSignaturePolicy){0};p->required=true;p->trusted_publisher_only=true;}}bool umi_dr_signature_policy_accept(const UmiDrSignaturePolicy*p,bool s,bool t,bool ts){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->required&&!s)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->trusted_publisher_only&&!t)return false;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->timestamp_required&&!ts)return false;return true;}
