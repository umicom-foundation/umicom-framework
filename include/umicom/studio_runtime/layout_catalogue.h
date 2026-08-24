/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_catalogue.h
 *
 * PURPOSE:
 *   Publish and resolve the Framework-owned Studio semantic layout presets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_CATALOGUE_H
#include "umicom/studio_runtime/layout_presets/presets.h"
#ifdef __cplusplus
extern "C" {
#endif
size_t umi_studio_layout_catalogue_count(void);
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_at(size_t index);
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_catalogue_find(const char *preset_id);
#ifdef __cplusplus
}
#endif
#endif
