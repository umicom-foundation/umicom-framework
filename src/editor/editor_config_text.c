/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/editor_config_text.c
 *
 * PURPOSE:
 *   Keep UTF-8 and case admission consistent across configuration readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "editor_config_internal.h"
#include "umicom/editor/text_position.h"
#include <string.h>
UmiStatus UmiEditorConfigTextValidate(const char *text, size_t bytes) {
  if (text == NULL || memchr(text, 0, bytes))
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorTextBufferView view = {0};
  view.struct_size = sizeof view;
  view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
  view.bytes = text;
  view.byte_count = bytes;
  view.capacity = bytes;
  UmiEditorTextPosition position;
  return UmiEditorTextViewPositionAt(&view, bytes, &position);
}
size_t UmiEditorConfigScalar(const char *text, uint32_t *out) {
  const unsigned char *s = (const unsigned char *)text;
  size_t n = s[0] < 0x80U ? 1U : s[0] < 0xe0U ? 2U : s[0] < 0xf0U ? 3U : 4U;
  uint32_t v = s[0] & (n == 1U ? 0x7fU : n == 2U ? 0x1fU : n == 3U ? 0x0fU : 0x07U);
  for (size_t i = 1U; i < n; ++i)
    v = (v << 6U) | (s[i] & 0x3fU);
  *out = v;
  return n;
}
int UmiEditorConfigEqualFolded(const char *left, const char *right) {
  while (*left && *right) {
    unsigned char a = (unsigned char)*left++, b = (unsigned char)*right++;
    if (a >= 'A' && a <= 'Z')
      a = (unsigned char)(a - 'A' + 'a');
    if (b >= 'A' && b <= 'Z')
      b = (unsigned char)(b - 'A' + 'a');
    if (a != b)
      return 0;
  }
  return *left == *right;
}
