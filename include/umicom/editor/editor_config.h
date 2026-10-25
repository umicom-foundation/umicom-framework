/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_config.h
 *
 * PURPOSE:
 *   Read and inspect owned project editor configuration without modifying settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_H
#define UMICOM_EDITOR_CONFIG_H
#include "umicom/base/status.h"
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_EDITOR_CONFIG_MAXIMUM_BYTES (1024U * 1024U)
#define UMI_EDITOR_CONFIG_MAXIMUM_SECTIONS 256U
#define UMI_EDITOR_CONFIG_MAXIMUM_PAIRS 1024U
#define UMI_EDITOR_CONFIG_MAXIMUM_KEY 1024U
#define UMI_EDITOR_CONFIG_MAXIMUM_VALUE 4096U
#define UMI_EDITOR_CONFIG_MAXIMUM_PATTERN 1024U
typedef struct UmiEditorConfig UmiEditorConfig;
typedef struct UmiEditorConfigDiagnostic {
  size_t line;
  UmiStatus status;
} UmiEditorConfigDiagnostic;
typedef struct UmiEditorConfigPair {
  const char *key, *value;
  size_t line, section;
} UmiEditorConfigPair;
/* Parse copied UTF-8 content with LF/CRLF endings. No filesystem, callbacks,
 * settings mutation or code execution. Accept empty files, unknown pairs and
 * full-line comments; inline # and ; remain value text. Keys are ASCII-folded,
 * values are retained. Preamble pairs have section SIZE_MAX. Last root pair in
 * the preamble decides root=true (case-insensitive); section root is ordinary.
 * Limits above apply before allocation/publication. Failure clears out_config
 * and reports the one-based offending line when known. The optional diagnostic
 * does not overlap input/output. Destroy each successful owner. */
UmiStatus UmiEditorConfigParse(const char *source, size_t bytes, UmiEditorConfig **out_config,
                               UmiEditorConfigDiagnostic *diagnostic);
void UmiEditorConfigDestroy(UmiEditorConfig *config);
int UmiEditorConfigIsRoot(const UmiEditorConfig *config);
size_t UmiEditorConfigSectionCount(const UmiEditorConfig *config);
size_t UmiEditorConfigPairCount(const UmiEditorConfig *config);
/* Borrowed values remain immutable until Destroy. Error leaves outputs intact. */
UmiStatus UmiEditorConfigSectionAt(const UmiEditorConfig *config, size_t index,
                                   const char **out_pattern, size_t *out_line);
UmiStatus UmiEditorConfigPairAt(const UmiEditorConfig *config, size_t index,
                                UmiEditorConfigPair *out_pair);
#ifdef __cplusplus
}
#endif
#endif
