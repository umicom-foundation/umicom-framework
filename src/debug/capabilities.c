/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/capabilities.c
 *
 * PURPOSE:
 *   Implement the capabilities behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework DAP capabilities. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/debug/capabilities.h"
#include <string.h>
/*
 * Initialise debug capability set from caller-provided values so later operations receive
 * a known state.
 */
void umi_debug_capability_set_init(UmiDebugCapabilitySet*s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){(void)memset(s,0,sizeof(*s));s->supported=UINT64_MAX;s->revision=1U;}}
/*
 * Provide the debug capability set advertise operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_capability_set_advertise(UmiDebugCapabilitySet*s,uint64_t c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->advertised=c&s->supported;s->revision+=1U;return UMI_STATUS_OK;}
/*
 * Provide the debug capability set require operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_capability_set_require(UmiDebugCapabilitySet*s,uint64_t c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->required=c;s->revision+=1U;return (c&~s->supported)==0U?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;}
/*
 * Provide the debug capability set ready operation used by this module and its client
 * applications.
 */
int umi_debug_capability_set_ready(const UmiDebugCapabilitySet*s){return s!=NULL&&(s->required&~s->advertised)==0U;}
/*
 * Provide the debug capability set has operation used by this module and its client
 * applications.
 */
int umi_debug_capability_set_has(const UmiDebugCapabilitySet*s,uint64_t c){return s!=NULL&&c!=0U&&(s->advertised&c)==c;}
