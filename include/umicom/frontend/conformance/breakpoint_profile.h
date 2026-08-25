/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/breakpoint_profile.h
 *
 * PURPOSE:
 *   validated ordered responsive breakpoints and viewport class resolution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_BREAKPOINT_PROFILE_H
#define UMICOM_FRONTEND_CONFORMANCE_BREAKPOINT_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFcBreakpointProfile { int32_t values[8]; size_t count; } UmiFcBreakpointProfile;
void umi_fc_breakpoint_profile_init(UmiFcBreakpointProfile *profile);
UmiStatus umi_fc_breakpoint_profile_add(UmiFcBreakpointProfile *profile,int32_t width);
size_t umi_fc_breakpoint_profile_bucket(const UmiFcBreakpointProfile *profile,int32_t width);

#ifdef __cplusplus
}
#endif
#endif
