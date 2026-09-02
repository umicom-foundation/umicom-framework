/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/surface_support.h
 *
 * PURPOSE:
 *   support scoring and blocker detection for a renderer against one semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_SURFACE_SUPPORT_H
#define UMICOM_FRONTEND_CONFORMANCE_SURFACE_SUPPORT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc surface support data shared with callers of this public contract.
 */
typedef struct UmiFcSurfaceSupport { uint64_t required; uint64_t supported; } UmiFcSurfaceSupport;
/**
 * Initialise fc surface support from caller-provided values so later operations receive a
 * known state.
 */
void umi_fc_surface_support_init(UmiFcSurfaceSupport *self);
/**
 * Provide the fc surface support satisfied operation used by this module and its client
 * applications.
 */
bool umi_fc_surface_support_satisfied(const UmiFcSurfaceSupport *self);
/**
 * Provide the fc surface support score operation used by this module and its client
 * applications.
 */
double umi_fc_surface_support_score(const UmiFcSurfaceSupport *self);

#ifdef __cplusplus
}
#endif
#endif
