/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/breakpoint_profile.c
 *
 * PURPOSE:
 *   validated ordered responsive breakpoints and viewport class resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/breakpoint_profile.h"

void umi_fc_breakpoint_profile_init(UmiFcBreakpointProfile *profile){if(profile!=NULL)*profile=(UmiFcBreakpointProfile){0};}
UmiStatus umi_fc_breakpoint_profile_add(UmiFcBreakpointProfile *profile,int32_t width){size_t i;if(profile==NULL||width<=0)return UMI_STATUS_INVALID_ARGUMENT;if(profile->count>=8U)return UMI_STATUS_CAPACITY_EXCEEDED;if(profile->count>0U&&width<=profile->values[profile->count-1U])return UMI_STATUS_INVALID_STATE;profile->values[profile->count++]=width;for(i=1U;i<profile->count;++i)if(profile->values[i]<=profile->values[i-1U])return UMI_STATUS_INVALID_STATE;return UMI_STATUS_OK;}
size_t umi_fc_breakpoint_profile_bucket(const UmiFcBreakpointProfile *profile,int32_t width){size_t i;if(profile==NULL)return 0U;for(i=0U;i<profile->count;++i)if(width<=profile->values[i])return i;return profile->count;}
