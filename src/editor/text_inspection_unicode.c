/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/text_inspection_unicode.c
 *
 * PURPOSE:
 *   Decode exact UTF-8 scalars without locale or platform-dependent signed character behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "text_inspection_internal.h"
UmiStatus InspectionScalar(const char *text, size_t bytes, size_t offset, uint32_t *scalar,
                           size_t *width) {
  if (text == NULL || offset >= bytes || scalar == NULL || width == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  const unsigned char *s = (const unsigned char *)text + offset;
  uint32_t value;
  size_t count;
  if (s[0] < 0x80U) {
    value = s[0];
    count = 1U;
  } else if (s[0] >= 0xc2U && s[0] <= 0xdfU) {
    value = s[0] & 0x1fU;
    count = 2U;
  } else if (s[0] >= 0xe0U && s[0] <= 0xefU) {
    value = s[0] & 0x0fU;
    count = 3U;
  } else if (s[0] >= 0xf0U && s[0] <= 0xf4U) {
    value = s[0] & 0x07U;
    count = 4U;
  } else
    return UMI_STATUS_PARSE_ERROR;
  if (count > bytes - offset)
    return UMI_STATUS_PARSE_ERROR;
  for (size_t i = 1U; i < count; ++i) {
    if ((s[i] & 0xc0U) != 0x80U)
      return UMI_STATUS_PARSE_ERROR;
    value = (value << 6U) | (s[i] & 0x3fU);
  }
  if (value == 0U || (count == 2U && value < 0x80U) || (count == 3U && value < 0x800U) ||
      (count == 4U && value < 0x10000U) || value > 0x10ffffU ||
      (value >= 0xd800U && value <= 0xdfffU))
    return UMI_STATUS_PARSE_ERROR;
  *scalar = value;
  *width = count;
  return UMI_STATUS_OK;
}
