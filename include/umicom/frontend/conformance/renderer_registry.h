/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/renderer_registry.h
 *
 * PURPOSE:
 *   bounded registry of frontend renderer profiles with stable identity lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_RENDERER_REGISTRY_H
#define UMICOM_FRONTEND_CONFORMANCE_RENDERER_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"
#include "umicom/frontend/conformance/renderer_profile.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fc renderer registry data shared with callers of this public contract.
 */
typedef struct UmiFcRendererRegistry { UmiFcRendererProfile items[8]; size_t count; uint64_t revision; } UmiFcRendererRegistry;
/**
 * Initialise fc renderer registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_fc_renderer_registry_init(UmiFcRendererRegistry *registry);
/**
 * Provide the fc renderer registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_fc_renderer_registry_upsert(UmiFcRendererRegistry *registry,const UmiFcRendererProfile *profile);
/**
 * Find fc renderer registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_fc_renderer_registry_find(const UmiFcRendererRegistry *registry,const char *id,UmiFcRendererProfile *out_profile);

#ifdef __cplusplus
}
#endif
#endif
