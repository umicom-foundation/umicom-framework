/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_config_result.h
 *
 * PURPOSE:
 *   Resolve ordered project configuration layers with property provenance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_RESULT_H
#define UMICOM_EDITOR_CONFIG_RESULT_H
#include "umicom/editor/editor_config.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CONFIG_MAXIMUM_LAYERS 32U
#define UMI_EDITOR_CONFIG_MAXIMUM_SOURCE_NAME 2048U
typedef struct UmiEditorConfigResult UmiEditorConfigResult;
typedef struct UmiEditorConfigLayer {
  const UmiEditorConfig *config;
  const char *relative_path; /* File relative to this configuration's directory. */
  const char *source_name;   /* Display provenance only; never opened as a path. */
} UmiEditorConfigLayer;
typedef struct UmiEditorConfigProperty {
  const char *key, *value, *source_name, *pattern;
  size_t line;
} UmiEditorConfigProperty;
/* Resolve outermost to innermost. The last root=true layer discards all
 * preceding layers. Matching sections apply in source order; last key wins,
 * and case-insensitive unset removes the key. Unknown properties survive.
 * Source labels and final values are copied; the result outlives its layers.
 * No file I/O or settings changes. Any invalid layer/pattern/capacity failure
 * clears out_result and publishes no partial result. Limit 32 layers and
 * 1024 distinct final keys. Zero layers creates an empty independent result.
 * This explicit layer API does not search directories by itself. */
UmiStatus UmiEditorConfigResolve(const UmiEditorConfigLayer *layers, size_t count,
                                 UmiEditorConfigResult **out_result);
void UmiEditorConfigResultDestroy(UmiEditorConfigResult *result);
size_t UmiEditorConfigResultCount(const UmiEditorConfigResult *result);
/* Outputs borrow immutable strings until result destruction; errors leave
 * outputs untouched. Lookup uses case-insensitive ASCII keys. */
UmiStatus UmiEditorConfigResultAt(const UmiEditorConfigResult *result, size_t index,
                                  UmiEditorConfigProperty *out_property);
UmiStatus UmiEditorConfigResultFind(const UmiEditorConfigResult *result, const char *key,
                                    UmiEditorConfigProperty *out_property);
#ifdef __cplusplus
}
#endif
#endif
