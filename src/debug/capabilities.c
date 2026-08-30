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
void umi_debug_capability_set_init(UmiDebugCapabilitySet*s){if(s!=NULL){(void)memset(s,0,sizeof(*s));s->supported=UINT64_MAX;s->revision=1U;}}
UmiStatus umi_debug_capability_set_advertise(UmiDebugCapabilitySet*s,uint64_t c){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->advertised=c&s->supported;s->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_debug_capability_set_require(UmiDebugCapabilitySet*s,uint64_t c){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->required=c;s->revision+=1U;return (c&~s->supported)==0U?UMI_STATUS_OK:UMI_STATUS_UNAVAILABLE;}
int umi_debug_capability_set_ready(const UmiDebugCapabilitySet*s){return s!=NULL&&(s->required&~s->advertised)==0U;}
int umi_debug_capability_set_has(const UmiDebugCapabilitySet*s,uint64_t c){return s!=NULL&&c!=0U&&(s->advertised&c)==c;}
