/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/breakpoint_profile.h
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
#ifndef UMICOM_FRONTEND_CONFORMANCE_BREAKPOINT_PROFILE_H
#define UMICOM_FRONTEND_CONFORMANCE_BREAKPOINT_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc breakpoint profile data shared with callers of this public contract.
 */
typedef struct UmiFcBreakpointProfile { int32_t values[8]; size_t count; } UmiFcBreakpointProfile;
/**
 * Initialise fc breakpoint profile from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_breakpoint_profile_init(UmiFcBreakpointProfile *profile);
/**
 * Add fc breakpoint profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fc_breakpoint_profile_add(UmiFcBreakpointProfile *profile,int32_t width);
/**
 * Provide the fc breakpoint profile bucket operation used by this module and its client
 * applications.
 */
size_t umi_fc_breakpoint_profile_bucket(const UmiFcBreakpointProfile *profile,int32_t width);

#ifdef __cplusplus
}
#endif
#endif
