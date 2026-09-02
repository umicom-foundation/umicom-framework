/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/layout_presets/code_preview.h
 * PURPOSE: Publish the reusable code-and-live-preview Studio layout.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_LAYOUT_CODE_PREVIEW_H
#define UMICOM_STUDIO_RUNTIME_LAYOUT_CODE_PREVIEW_H

#include "umicom/studio_runtime/layout_preset.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return the immutable layout used for source editing beside live output. */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_code_preview(void);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_STUDIO_RUNTIME_LAYOUT_CODE_PREVIEW_H */
