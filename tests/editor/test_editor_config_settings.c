/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_editor_config_settings.c
 *
 * PURPOSE:
 *   Verify source precedence, copied provenance and explicit interpretation of project settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_fixture.h"
typedef struct Example {
  const char *name, *properties;
  unsigned width;
  int spaces;
  size_t invalid;
  UmiStatus status;
} Example;
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const Example cases[] = {
      {"default", "", 4U, 1, 0U, UMI_STATUS_OK},
      {"spaces", "indent_style=space\nindent_size=2\n", 2U, 1, 0U, UMI_STATUS_OK},
      {"tabs", "indent_style=tab\ntab_width=8\n", 8U, 0, 0U, UMI_STATUS_OK},
      {"tab-size", "indent_style=tab\nindent_size=tab\ntab_width=8\n", 8U, 0, 0U, UMI_STATUS_OK},
      {"tab-size-default", "indent_size=tab\n", 4U, 1, 0U, UMI_STATUS_OK},
      {"equal-widths", "indent_size=3\ntab_width=3\n", 3U, 1, 0U, UMI_STATUS_OK},
      {"distinct-widths", "indent_size=2\ntab_width=4\n", 0U, 0, 0U, UMI_STATUS_NOT_IMPLEMENTED},
      {"large-width", "indent_size=17\n", 0U, 0, 0U, UMI_STATUS_NOT_IMPLEMENTED},
      {"invalid-number", "indent_size=2x\n", 4U, 1, 1U, UMI_STATUS_OK},
      {"zero-number", "indent_size=0\n", 4U, 1, 1U, UMI_STATUS_OK},
      {"negative-number", "tab_width=-1\n", 4U, 1, 1U, UMI_STATUS_OK},
      {"overflow-number", "tab_width=4294967296\n", 4U, 1, 1U, UMI_STATUS_OK},
      {"unknown-style", "indent_style=elastic\n", 4U, 1, 1U, UMI_STATUS_OK},
      {"mixed-case", "InDeNt_StYlE=TaB\nTaB_WiDtH=6\n", 6U, 0, 0U, UMI_STATUS_OK},
      {"unset-default", "indent_size=8\nindent_size=unset\n", 4U, 1, 0U, UMI_STATUS_OK}};
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i].name)) {
      char text[256];
      int n = snprintf(text, sizeof text, "[*]\n%s", cases[i].properties);
      CHECK(n > 0 && (size_t)n < sizeof text);
      UmiEditorConfigResult *r = NULL;
      CHECK(ResolveOne(text, "a.c", &r) == UMI_STATUS_OK);
      UmiEditorConfigSettings values;
      CHECK(UmiEditorConfigSettingsRead(r, &values) == UMI_STATUS_OK &&
            values.invalid_values == cases[i].invalid);
      UmiEditorIndentation defaults = {4U, 1}, out = {9U, 0};
      CHECK(UmiEditorConfigSettingsIndentation(r, &defaults, &out) == cases[i].status);
      if (cases[i].status == UMI_STATUS_OK)
        CHECK(out.tab_width == cases[i].width && out.insert_spaces == cases[i].spaces);
      else
        CHECK(out.tab_width == 9U && out.insert_spaces == 0);
      UmiEditorConfigResultDestroy(r);
      return 0;
    }
  if (!strcmp(argv[1], "format-values")) {
    UmiEditorConfigResult *r = NULL;
    CHECK(ResolveOne("[*]\ncharset=UTF-16LE\nend_of_line=CRLF\ntrim_trailing_whitespace="
                     "TrUe\ninsert_final_newline=FALSE\n",
                     "a", &r) == UMI_STATUS_OK);
    UmiEditorConfigSettings s;
    CHECK(UmiEditorConfigSettingsRead(r, &s) == UMI_STATUS_OK);
    CHECK(s.present == (UMI_EDITOR_CONFIG_CHARSET | UMI_EDITOR_CONFIG_LINE_ENDING |
                        UMI_EDITOR_CONFIG_TRIM | UMI_EDITOR_CONFIG_FINAL_NEWLINE));
    CHECK(s.charset == UMI_EDITOR_CONFIG_UTF16_LE && s.line_ending == UMI_EDITOR_CONFIG_CRLF &&
          s.trim_trailing_whitespace && !s.insert_final_newline);
    UmiEditorConfigResultDestroy(r);
    return 0;
  }
  if (!strcmp(argv[1], "invalid-format")) {
    UmiEditorConfigResult *r = NULL;
    CHECK(ResolveOne("[*]\ncharset=ascii\nend_of_line=unix\ntrim_trailing_whitespace=yes\ninsert_"
                     "final_newline=maybe\ncustom=kept\n",
                     "a", &r) == UMI_STATUS_OK);
    UmiEditorConfigSettings s;
    CHECK(UmiEditorConfigSettingsRead(r, &s) == UMI_STATUS_OK && s.present == 0U &&
          s.invalid_values == 4U && s.unknown_properties == 1U);
    UmiEditorConfigResultDestroy(r);
    return 0;
  }
  if (!strcmp(argv[1], "arguments")) {
    UmiEditorConfigSettings s, sentinel;
    memset(&s, 0x39, sizeof s);
    sentinel = s;
    CHECK(UmiEditorConfigSettingsRead(NULL, &s) == UMI_STATUS_INVALID_ARGUMENT &&
          !memcmp(&s, &sentinel, sizeof s));
    UmiEditorIndentation value = {4U, 1};
    CHECK(UmiEditorConfigSettingsIndentation(NULL, &value, &value) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  return 2;
}
