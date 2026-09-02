/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_catalogue.h
 *
 * PURPOSE:
 *   Publish and resolve the Framework-owned Studio semantic layout presets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_CATALOGUE_H
#include "umicom/studio_runtime/layout_presets/presets.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Return the number of records represented by studio layout catalogue without changing
 * their state.
 */
size_t umi_studio_layout_catalogue_count(void);
/**
 * Find studio layout catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_at(size_t index);
/**
 * Find studio layout catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_find(const char *preset_id);
#ifdef __cplusplus
}
#endif
#endif
