/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/editor_config_settings.h
 *
 * PURPOSE:
 *   Interpret project editor preferences without silently changing unsupported indentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_EDITOR_CONFIG_SETTINGS_H
#define UMICOM_EDITOR_CONFIG_SETTINGS_H
#include "umicom/editor/editor_config_result.h"
#include "umicom/editor/indentation.h"
#ifdef __cplusplus
extern "C" {
#endif
enum {
  UMI_EDITOR_CONFIG_INDENT_STYLE = 1U,
  UMI_EDITOR_CONFIG_INDENT_SIZE = 2U,
  UMI_EDITOR_CONFIG_TAB_WIDTH = 4U,
  UMI_EDITOR_CONFIG_LINE_ENDING = 8U,
  UMI_EDITOR_CONFIG_CHARSET = 16U,
  UMI_EDITOR_CONFIG_TRIM = 32U,
  UMI_EDITOR_CONFIG_FINAL_NEWLINE = 64U
};
typedef enum UmiEditorConfigEnding {
  UMI_EDITOR_CONFIG_LF = 1,
  UMI_EDITOR_CONFIG_CRLF,
  UMI_EDITOR_CONFIG_CR
} UmiEditorConfigEnding;
typedef enum UmiEditorConfigCharset {
  UMI_EDITOR_CONFIG_UTF8 = 1,
  UMI_EDITOR_CONFIG_UTF8_BOM,
  UMI_EDITOR_CONFIG_UTF16_LE,
  UMI_EDITOR_CONFIG_UTF16_BE,
  UMI_EDITOR_CONFIG_LATIN1
} UmiEditorConfigCharset;
typedef struct UmiEditorConfigSettings {
  unsigned present;
  unsigned indent_size, tab_width;
  int insert_spaces, indent_size_is_tab, trim_trailing_whitespace, insert_final_newline;
  UmiEditorConfigEnding line_ending;
  UmiEditorConfigCharset charset;
  size_t unknown_properties, invalid_values;
} UmiEditorConfigSettings;
/* Interpret recognized values case-insensitively. Invalid/unsupported values
 * are omitted and counted; unknown keys remain inspectable in the result.
 * This is a read-only projection, not permission to reformat or save a file.
 * Positive integer widths through UINT32_MAX are retained for review even
 * when a particular editor cannot represent them. Outputs unchanged on error. */
UmiStatus UmiEditorConfigSettingsRead(const UmiEditorConfigResult *result,
                                      UmiEditorConfigSettings *out_settings);
/* Derive the current Framework indentation pair using explicit editor defaults.
 * Distinct indent_size/tab_width or widths outside 1..16 cannot be represented
 * by this editor policy and return NOT_IMPLEMENTED with output untouched.
 * A missing/unset key keeps its default. No global or document settings change. */
UmiStatus UmiEditorConfigSettingsIndentation(const UmiEditorConfigResult *result,
                                             const UmiEditorIndentation *defaults,
                                             UmiEditorIndentation *out_indentation);
#ifdef __cplusplus
}
#endif
#endif
