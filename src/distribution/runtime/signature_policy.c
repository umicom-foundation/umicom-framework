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

void umi_dr_signature_policy_init(UmiDrSignaturePolicy*p){if(p!=NULL){*p=(UmiDrSignaturePolicy){0};p->required=true;p->trusted_publisher_only=true;}}bool umi_dr_signature_policy_accept(const UmiDrSignaturePolicy*p,bool s,bool t,bool ts){if(p==NULL)return false;if(p->required&&!s)return false;if(p->trusted_publisher_only&&!t)return false;if(p->timestamp_required&&!ts)return false;return true;}
