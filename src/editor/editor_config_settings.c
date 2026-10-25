/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config_settings.c
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

#include "umicom/editor/editor_config_settings.h"
#include "editor_config_internal.h"
#include <stdint.h>
#include <string.h>
static int Positive(const char *s, unsigned *out) {
  if (!*s)
    return 0;
  uint32_t value = 0;
  for (; *s; ++s) {
    if (*s < '0' || *s > '9')
      return 0;
    unsigned digit = (unsigned)(*s - '0');
    if (value > (UINT32_MAX - digit) / 10U)
      return 0;
    value = value * 10U + digit;
  }
  if (!value)
    return 0;
  *out = value;
  return 1;
}
static int Boolean(const char *s, int *out) {
  if (UmiEditorConfigEqualFolded(s, "true")) {
    *out = 1;
    return 1;
  }
  if (UmiEditorConfigEqualFolded(s, "false")) {
    *out = 0;
    return 1;
  }
  return 0;
}
UmiStatus UmiEditorConfigSettingsRead(const UmiEditorConfigResult *r,
                                      UmiEditorConfigSettings *out) {
  if (!r || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorConfigSettings value = {0};
  for (size_t i = 0; i < UmiEditorConfigResultCount(r); ++i) {
    UmiEditorConfigProperty p;
    UmiStatus status = UmiEditorConfigResultAt(r, i, &p);
    if (status != UMI_STATUS_OK)
      return status;
    unsigned bit = 0U;
    int valid = 1;
    if (!strcmp(p.key, "indent_style")) {
      bit = UMI_EDITOR_CONFIG_INDENT_STYLE;
      if (UmiEditorConfigEqualFolded(p.value, "space"))
        value.insert_spaces = 1;
      else if (UmiEditorConfigEqualFolded(p.value, "tab"))
        value.insert_spaces = 0;
      else
        valid = 0;
    } else if (!strcmp(p.key, "indent_size")) {
      bit = UMI_EDITOR_CONFIG_INDENT_SIZE;
      value.indent_size_is_tab = UmiEditorConfigEqualFolded(p.value, "tab");
      valid = value.indent_size_is_tab || Positive(p.value, &value.indent_size);
    } else if (!strcmp(p.key, "tab_width")) {
      bit = UMI_EDITOR_CONFIG_TAB_WIDTH;
      valid = Positive(p.value, &value.tab_width);
    } else if (!strcmp(p.key, "end_of_line")) {
      bit = UMI_EDITOR_CONFIG_LINE_ENDING;
      if (UmiEditorConfigEqualFolded(p.value, "lf"))
        value.line_ending = UMI_EDITOR_CONFIG_LF;
      else if (UmiEditorConfigEqualFolded(p.value, "crlf"))
        value.line_ending = UMI_EDITOR_CONFIG_CRLF;
      else if (UmiEditorConfigEqualFolded(p.value, "cr"))
        value.line_ending = UMI_EDITOR_CONFIG_CR;
      else
        valid = 0;
    } else if (!strcmp(p.key, "charset")) {
      bit = UMI_EDITOR_CONFIG_CHARSET;
      const char *names[] = {"utf-8", "utf-8-bom", "utf-16le", "utf-16be", "latin1"};
      valid = 0;
      for (size_t j = 0; j < 5U; ++j)
        if (UmiEditorConfigEqualFolded(p.value, names[j])) {
          value.charset = (UmiEditorConfigCharset)(j + 1U);
          valid = 1;
        }
    } else if (!strcmp(p.key, "trim_trailing_whitespace")) {
      bit = UMI_EDITOR_CONFIG_TRIM;
      valid = Boolean(p.value, &value.trim_trailing_whitespace);
    } else if (!strcmp(p.key, "insert_final_newline")) {
      bit = UMI_EDITOR_CONFIG_FINAL_NEWLINE;
      valid = Boolean(p.value, &value.insert_final_newline);
    } else {
      ++value.unknown_properties;
      continue;
    }
    if (valid)
      value.present |= bit;
    else
      ++value.invalid_values;
  }
  *out = value;
  return UMI_STATUS_OK;
}
UmiStatus UmiEditorConfigSettingsIndentation(const UmiEditorConfigResult *r,
                                             const UmiEditorIndentation *defaults,
                                             UmiEditorIndentation *out) {
  if (!r || !out || UmiEditorIndentationValidate(defaults) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorConfigSettings s;
  UmiStatus status = UmiEditorConfigSettingsRead(r, &s);
  if (status != UMI_STATUS_OK)
    return status;
  UmiEditorIndentation value = *defaults;
  if (s.present & UMI_EDITOR_CONFIG_INDENT_STYLE)
    value.insert_spaces = s.insert_spaces;
  unsigned tab = defaults->tab_width, indent = tab;
  if (s.present & UMI_EDITOR_CONFIG_TAB_WIDTH)
    tab = s.tab_width;
  else if ((s.present & UMI_EDITOR_CONFIG_INDENT_SIZE) && !s.indent_size_is_tab)
    tab = s.indent_size;
  if (s.present & UMI_EDITOR_CONFIG_INDENT_SIZE)
    indent = s.indent_size_is_tab ? tab : s.indent_size;
  else
    indent = tab;
  if (tab != indent || tab < 1U || tab > UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH)
    return UMI_STATUS_NOT_IMPLEMENTED;
  value.tab_width = tab;
  *out = value;
  return UMI_STATUS_OK;
}
