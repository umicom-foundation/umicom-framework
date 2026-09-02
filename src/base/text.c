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

  /*
   * Continue only while work remains available; the loop body advances the state on each
   * pass.
   */
  while (length < capacity && text[length] != '\0')
    length += 1U;
  return length;
}

/* Copy text into module-owned storage so callers keep ownership of their input values. */
UmiStatus umi_text_copy(char *destination, size_t capacity,
                        const char *source) {
  size_t source_length;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U || source == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  source_length = strlen(source);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (source_length >= capacity) {
    destination[0] = '\0';
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  /* Apply this branch only when its contract condition is satisfied. */
  if (source_length > 0U)
    (void)memcpy(destination, source, source_length);
  destination[source_length] = '\0';
  return UMI_STATUS_OK;
}

/*
 * Provide the text copy truncated operation used by this module and its client
 * applications.
 */
size_t umi_text_copy_truncated(char *destination, size_t capacity,
                               const char *source) {
  size_t source_length;
  size_t copy_length;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U)
    return 0U;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (source == NULL) {
    destination[0] = '\0';
    return 0U;
  }
  source_length = strlen(source);
  copy_length = source_length < capacity - 1U ? source_length : capacity - 1U;
  /* Apply this branch only when its contract condition is satisfied. */
  if (copy_length > 0U)
    (void)memcpy(destination, source, copy_length);
  destination[copy_length] = '\0';
  return source_length;
}

/* Add text only after its inputs and available capacity have been checked. */
UmiStatus umi_text_append(char *destination, size_t capacity,
                          const char *suffix) {
  size_t destination_length;
  size_t suffix_length;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U || suffix == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  destination_length = bounded_length(destination, capacity);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (destination_length == capacity)
    return UMI_STATUS_INVALID_STATE;
  suffix_length = strlen(suffix);
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (suffix_length >= capacity - destination_length)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Apply this branch only when its contract condition is satisfied. */
  if (suffix_length > 0U)
    (void)memcpy(destination + destination_length, suffix, suffix_length);
  destination[destination_length + suffix_length] = '\0';
  return UMI_STATUS_OK;
}

/* Provide the text format operation used by this module and its client applications. */
UmiStatus umi_text_format(char *destination, size_t capacity,
                          const char *format, ...) {
  va_list arguments;
  int written;

  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (destination == NULL || capacity == 0U || format == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  va_start(arguments, format);
  written = vsnprintf(destination, capacity, format, arguments);
  va_end(arguments);
  /* Apply this branch only when its contract condition is satisfied. */
  if (written < 0) {
    destination[0] = '\0';
    return UMI_STATUS_INTERNAL_ERROR;
  }
  return (size_t)written < capacity ? UMI_STATUS_OK
                                    : UMI_STATUS_CAPACITY_EXCEEDED;
}
