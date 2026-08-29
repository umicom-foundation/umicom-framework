/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/text.c
 *
 * PURPOSE:
 *   Implement deterministic bounded text operations shared by every Framework
 *   library while preserving null termination and explicit capacity evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/base/text.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Return capacity when the fixed buffer has no terminator. */
static size_t bounded_length(const char *text, size_t capacity) {
  size_t length = 0U;

  while (length < capacity && text[length] != '\0')
    length += 1U;
  return length;
}

UmiStatus umi_text_copy(char *destination, size_t capacity,
                        const char *source) {
  size_t source_length;

  if (destination == NULL || capacity == 0U || source == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  source_length = strlen(source);
  if (source_length >= capacity) {
    destination[0] = '\0';
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  if (source_length > 0U)
    (void)memcpy(destination, source, source_length);
  destination[source_length] = '\0';
  return UMI_STATUS_OK;
}

size_t umi_text_copy_truncated(char *destination, size_t capacity,
                               const char *source) {
  size_t source_length;
  size_t copy_length;

  if (destination == NULL || capacity == 0U)
    return 0U;
  if (source == NULL) {
    destination[0] = '\0';
    return 0U;
  }
  source_length = strlen(source);
  copy_length = source_length < capacity - 1U ? source_length : capacity - 1U;
  if (copy_length > 0U)
    (void)memcpy(destination, source, copy_length);
  destination[copy_length] = '\0';
  return source_length;
}

UmiStatus umi_text_append(char *destination, size_t capacity,
                          const char *suffix) {
  size_t destination_length;
  size_t suffix_length;

  if (destination == NULL || capacity == 0U || suffix == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  destination_length = bounded_length(destination, capacity);
  if (destination_length == capacity)
    return UMI_STATUS_INVALID_STATE;
  suffix_length = strlen(suffix);
  if (suffix_length >= capacity - destination_length)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (suffix_length > 0U)
    (void)memcpy(destination + destination_length, suffix, suffix_length);
  destination[destination_length + suffix_length] = '\0';
  return UMI_STATUS_OK;
}

UmiStatus umi_text_format(char *destination, size_t capacity,
                          const char *format, ...) {
  va_list arguments;
  int written;

  if (destination == NULL || capacity == 0U || format == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  va_start(arguments, format);
  written = vsnprintf(destination, capacity, format, arguments);
  va_end(arguments);
  if (written < 0) {
    destination[0] = '\0';
    return UMI_STATUS_INTERNAL_ERROR;
  }
  return (size_t)written < capacity ? UMI_STATUS_OK
                                    : UMI_STATUS_CAPACITY_EXCEEDED;
}
