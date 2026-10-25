/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication_fields.c
 *
 * PURPOSE:
 *   Share canonical public-field validation between HTML rendering and saved plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_plan_internal.h"
bool UmiKanbanPublicText(const char *text, size_t capacity, bool optional) {
  if (text == NULL)
    return false;
  size_t length = 0U;
  while (length < capacity && text[length] != '\0')
    ++length;
  if (length == capacity || (!optional && length == 0U))
    return false;
  const unsigned char *p = (const unsigned char *)text;
  for (size_t i = 0U; i < length;) {
    unsigned char first = p[i];
    if (first < 0x80U) {
      if ((first < 0x20U && first != '\n' && first != '\t' && first != '\r') || first == 0x7fU)
        return false;
      ++i;
      continue;
    }
    size_t width = first >= 0xc2U && first <= 0xdfU   ? 2U
                   : first >= 0xe0U && first <= 0xefU ? 3U
                   : first >= 0xf0U && first <= 0xf4U ? 4U
                                                      : 0U;
    if (width == 0U || width > length - i)
      return false;
    for (size_t j = 1U; j < width; ++j)
      if ((p[i + j] & 0xc0U) != 0x80U)
        return false;
    if ((first == 0xe0U && p[i + 1U] < 0xa0U) || (first == 0xedU && p[i + 1U] > 0x9fU) ||
        (first == 0xf0U && p[i + 1U] < 0x90U) || (first == 0xf4U && p[i + 1U] > 0x8fU))
      return false;
    i += width;
  }
  return true;
}
