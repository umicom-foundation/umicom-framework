/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/resource_policy.c
 *
 * PURPOSE:
 *   resource selection rules for locale, scale and optional fallbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/resource_policy.h"

#include <string.h>
/*
 * Initialise dr resource policy from caller-provided values so later operations receive a
 * known state.
 */
void umi_dr_resource_policy_init(UmiDrResourcePolicy*p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){*p=(UmiDrResourcePolicy){0};p->preferred_scale=100U;p->allow_locale_fallback=true;p->allow_scale_fallback=true;}}
/*
 * Provide the dr resource policy accepts operation used by this module and its client
 * applications.
 */
bool umi_dr_resource_policy_accepts(const UmiDrResourcePolicy*p,const char*l,uint32_t s){bool lm,sm;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||l==NULL)return false;lm=p->preferred_locale[0]=='\0'||strcmp(p->preferred_locale,l)==0;sm=p->preferred_scale==0U||p->preferred_scale==s;return (lm||p->allow_locale_fallback)&&(sm||p->allow_scale_fallback);}
