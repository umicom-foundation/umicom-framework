/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/breakpoint_profile.c
 *
 * PURPOSE:
 *   validated ordered responsive breakpoints and viewport class resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/breakpoint_profile.h"

/*
 * Initialise fc breakpoint profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_breakpoint_profile_init(UmiFcBreakpointProfile *profile){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile!=NULL)*profile=(UmiFcBreakpointProfile){0};}
/*
 * Add fc breakpoint profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fc_breakpoint_profile_add(UmiFcBreakpointProfile *profile,int32_t width){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL||width<=0)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile->count>0U&&width<=profile->values[profile->count-1U])return UMI_STATUS_INVALID_STATE;profile->values[profile->count++]=width;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<profile->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile->values[i]<=profile->values[i-1U])return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
/*
 * Provide the fc breakpoint profile bucket operation used by this module and its client
 * applications.
 */
size_t umi_fc_breakpoint_profile_bucket(const UmiFcBreakpointProfile *profile,int32_t width){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(profile==NULL)return 0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<profile->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(width<=profile->values[i])return i;return profile->count;}
