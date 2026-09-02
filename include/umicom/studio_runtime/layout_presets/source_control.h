/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_presets/source_control.h
 *
 * PURPOSE:
 *   Publish the Source Control semantic Studio layout preset.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_SOURCE_CONTROL_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_SOURCE_CONTROL_H
#include "umicom/studio_runtime/layout_preset.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio layout preset source control operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_source_control(void);
#ifdef __cplusplus
}
#endif
#endif
