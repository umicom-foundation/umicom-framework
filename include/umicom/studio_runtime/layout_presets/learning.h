/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_presets/learning.h
 *
 * PURPOSE:
 *   Publish the C Development and Learning semantic Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_LEARNING_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_PRESET_LEARNING_H

#include "umicom/studio_runtime/layout_preset.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the layout that places lessons beside a working C toolchain. */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_learning(void);

#ifdef __cplusplus
}
#endif
#endif
