/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/surface_requirement.h
 *
 * PURPOSE:
 *   required and optional semantic capability requirements for a Framework UI surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SURFACE_REQUIREMENT_H
#define UMICOM_FRONTEND_CONFORMANCE_SURFACE_REQUIREMENT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc surface requirement data shared with callers of this public contract.
 */
typedef struct UmiFcSurfaceRequirement { char surface_id[UMI_FC_ID_CAPACITY]; uint64_t required; uint64_t optional; bool allow_degraded; } UmiFcSurfaceRequirement;
/**
 * Provide the fc surface requirement make operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_surface_requirement_make(const char *surface_id,uint64_t required,uint64_t optional,bool allow_degraded,UmiFcSurfaceRequirement *out_requirement);
/**
 * Provide the fc surface requirement missing operation used by this module and its client
 * applications.
 */
uint64_t umi_fc_surface_requirement_missing(const UmiFcSurfaceRequirement *requirement,uint64_t actual);

#ifdef __cplusplus
}
#endif
#endif
