/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/renderer_registry.h
 *
 * PURPOSE:
 *   Register and resolve semantic-contract web renderers without application-specific switch statements.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDERER_REGISTRY_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDERER_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/renderer_descriptor.h"

typedef struct UmiNativeWebRendererRegistry { UmiNativeWebRendererDescriptor items[UMI_NATIVE_WEB_MAX_RENDERERS]; size_t count; uint64_t revision; } UmiNativeWebRendererRegistry;
/* Initialise an empty renderer registry. */
void umi_native_web_renderer_registry_init(UmiNativeWebRendererRegistry *registry);
/* Add one renderer, rejecting duplicate renderer IDs or semantic contracts. */
UmiStatus umi_native_web_renderer_registry_add(UmiNativeWebRendererRegistry *registry, const UmiNativeWebRendererDescriptor *descriptor);
/* Resolve a renderer by semantic contract. */
const UmiNativeWebRendererDescriptor *umi_native_web_renderer_registry_resolve(const UmiNativeWebRendererRegistry *registry, const char *semantic_contract);

#ifdef __cplusplus
}
#endif
#endif
