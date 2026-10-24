/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/ascii_number.c
 * PURPOSE: Keep native and web numeric input independent of ambient locale.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#if !defined(_WIN32) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif
#include "umicom/base/ascii_number.h"
#include <errno.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus span_check(const char *text, size_t length) {
  if (text == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length == 0U)
    return UMI_STATUS_PARSE_ERROR;
  if (length > UMI_ASCII_NUMBER_MAX_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return memchr(text, '\0', length) == NULL ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
static UmiStatus magnitude(const char *text, size_t length, uint64_t limit, uint64_t *out) {
  uint64_t value = 0U;
  if (length == 0U)
    return UMI_STATUS_PARSE_ERROR;
  for (size_t i = 0U; i < length; ++i) {
    if (text[i] < '0' || text[i] > '9')
      return UMI_STATUS_PARSE_ERROR;
    uint64_t digit = (uint64_t)(text[i] - '0');
    if (value > (limit - digit) / 10U)
      return UMI_STATUS_CAPACITY_EXCEEDED;
    value = value * 10U + digit;
  }
  *out = value;
  return UMI_STATUS_OK;
}
UmiStatus umi_ascii_uint64_parse(const char *text, size_t length, uint64_t *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = span_check(text, length);
  if (status != UMI_STATUS_OK)
    return status;
  uint64_t value;
  status = magnitude(text, length, UINT64_MAX, &value);
  if (status == UMI_STATUS_OK)
    *out = value;
  return status;
}
UmiStatus umi_ascii_int64_parse(const char *text, size_t length, int64_t *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = span_check(text, length);
  if (status != UMI_STATUS_OK)
    return status;
  int negative = text[0] == '-';
  size_t start = (negative || text[0] == '+') ? 1U : 0U;
  uint64_t value;
  uint64_t limit = (uint64_t)INT64_MAX + (negative ? 1U : 0U);
  status = magnitude(text + start, length - start, limit, &value);
  if (status != UMI_STATUS_OK)
    return status;
  /* Avoid casting the magnitude of INT64_MIN to a signed type. */
  *out = negative ? (value == 0U ? 0 : -1 - (int64_t)(value - 1U)) : (int64_t)value;
  return UMI_STATUS_OK;
}
static int decimal_spelling(const char *text, size_t length) {
  size_t at = 0U, first;
  if (text[at] == '+' || text[at] == '-')
    ++at;
  first = at;
  while (at < length && text[at] >= '0' && text[at] <= '9')
    ++at;
  if (at == first)
    return 0;
  if (at < length && text[at] == '.') {
    first = ++at;
    while (at < length && text[at] >= '0' && text[at] <= '9')
      ++at;
    if (at == first)
      return 0;
  }
  if (at < length && (text[at] == 'e' || text[at] == 'E')) {
    ++at;
    if (at < length && (text[at] == '+' || text[at] == '-'))
      ++at;
    first = at;
    while (at < length && text[at] >= '0' && text[at] <= '9')
      ++at;
    if (at == first)
      return 0;
  }
  return at == length;
}
UmiStatus umi_ascii_double_parse(const char *text, size_t length, double *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = span_check(text, length);
  if (status != UMI_STATUS_OK)
    return status;
  if (!decimal_spelling(text, length))
    return UMI_STATUS_PARSE_ERROR;
  char buffer[UMI_ASCII_NUMBER_MAX_BYTES + 1U], *end = NULL;
  memcpy(buffer, text, length);
  buffer[length] = '\0';
  double value = 0.0;
  int conversion_errno = 0;
#if defined(_WIN32)
  _locale_t locale = _create_locale(LC_NUMERIC, "C");
  if (locale == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  errno = 0;
  value = _strtod_l(buffer, &end, locale);
  conversion_errno = errno;
  _free_locale(locale);
#elif defined(__unix__) || defined(__APPLE__)
  locale_t locale = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
  if (locale == (locale_t)0)
    return UMI_STATUS_OUT_OF_MEMORY;
  errno = 0;
  value = strtod_l(buffer, &end, locale);
  conversion_errno = errno;
  freelocale(locale);
#else
  (void)buffer;
  (void)end;
  (void)value;
  (void)conversion_errno;
  return UMI_STATUS_NOT_IMPLEMENTED;
#endif
  if (end != buffer + length)
    return UMI_STATUS_PARSE_ERROR;
  /* Finite nonzero subnormals remain representable. Refuse underflow to zero,
   * overflow and conversion errors, rather than silently changing an amount. */
  if (!isfinite(value) || (conversion_errno != 0 && !(conversion_errno == ERANGE && value != 0.0)))
    return UMI_STATUS_CAPACITY_EXCEEDED;
  *out = value;
  return UMI_STATUS_OK;
}
UmiStatus umi_ascii_double_format(double value, char *out, size_t capacity, size_t *out_length) {
  if (out == NULL || out_length == NULL || !isfinite(value))
    return UMI_STATUS_INVALID_ARGUMENT;
  char buffer[64];
  int length = 0;
#if defined(_WIN32)
  _locale_t locale = _create_locale(LC_NUMERIC, "C");
  if (locale == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  length = _snprintf_l(buffer, sizeof(buffer), "%.*g", locale, DBL_DECIMAL_DIG, value);
  _free_locale(locale);
#elif defined(__unix__) || defined(__APPLE__)
  locale_t locale = newlocale(LC_NUMERIC_MASK, "C", (locale_t)0);
  if (locale == (locale_t)0)
    return UMI_STATUS_OUT_OF_MEMORY;
  /* POSIX formatting has no portable snprintf_l. Limit the temporary locale to
   * this thread, restore it before freeing, and never call application callbacks. */
  locale_t previous = uselocale(locale);
  if (previous == (locale_t)0) {
    freelocale(locale);
    return UMI_STATUS_UNAVAILABLE;
  }
  length = snprintf(buffer, sizeof(buffer), "%.*g", DBL_DECIMAL_DIG, value);
  if (uselocale(previous) == (locale_t)0)
    return UMI_STATUS_INTERNAL_ERROR;
  freelocale(locale);
#else
  (void)buffer;
  (void)length;
  (void)capacity;
  return UMI_STATUS_NOT_IMPLEMENTED;
#endif
  if (length < 0 || (size_t)length >= sizeof(buffer))
    return UMI_STATUS_INTERNAL_ERROR;
  if ((size_t)length >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, buffer, (size_t)length + 1U);
  *out_length = (size_t)length;
  return UMI_STATUS_OK;
}
