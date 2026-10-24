/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * One entity boundary for preset persistence, historical readers and discovery. */
#ifndef UMICOM_SEARCH_PRESET_INTERNAL_H
#define UMICOM_SEARCH_PRESET_INTERNAL_H
#include "umicom/data/structured_form_search_preset.h"
#include <string.h>
static inline UmiStatus umi_search_preset_entity_check(const char *id) {
  if (!id)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0;
  while (length < 64 && id[length])
    ++length;
  const size_t prefix = sizeof(UMI_FORM_SEARCH_PRESET_PREFIX) - 1;
  if (length <= prefix || length > prefix + 40 ||
      strncmp(id, UMI_FORM_SEARCH_PRESET_PREFIX, prefix))
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = prefix; i < length; ++i) {
    char c = id[i];
    if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' ||
          c == '-'))
      return UMI_STATUS_INVALID_ARGUMENT;
  }
  return UMI_STATUS_OK;
}
#endif
