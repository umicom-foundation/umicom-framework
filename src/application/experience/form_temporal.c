/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_temporal.c
 * PURPOSE: Keep form date and instant validation independent of host settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_temporal.h"
#include <string.h>
static int digits(const char *text, size_t count, unsigned *out) {
  unsigned value = 0U;
  for (size_t i = 0U; i < count; ++i) {
    if (text[i] < '0' || text[i] > '9')
      return 0;
    value = value * 10U + (unsigned)(text[i] - '0');
  }
  *out = value;
  return 1;
}
static int date_parts(const char *text, unsigned *year, unsigned *month, unsigned *day) {
  static const unsigned days[] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
  if (text[4] != '-' || text[7] != '-' || !digits(text, 4U, year) || !digits(text + 5, 2U, month) ||
      !digits(text + 8, 2U, day) || *year == 0U || *month == 0U || *month > 12U || *day == 0U)
    return 0;
  unsigned maximum = days[*month - 1U];
  if (*month == 2U && *year % 4U == 0U && (*year % 100U != 0U || *year % 400U == 0U))
    ++maximum;
  return *day <= maximum;
}
UmiStatus umi_form_date_validate(const char *text, size_t length) {
  unsigned year, month, day;
  if (text == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  return length == 10U && date_parts(text, &year, &month, &day) ? UMI_STATUS_OK
                                                                : UMI_STATUS_PARSE_ERROR;
}
UmiStatus umi_form_timestamp_parse(const char *text, size_t length, UmiFormTimestamp *out) {
  UmiFormTimestamp value = {0};
  size_t at = 19U;
  if (text == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length < 20U || length > 35U || text[10] != 'T' || text[13] != ':' || text[16] != ':' ||
      !date_parts(text, &value.year, &value.month, &value.day) ||
      !digits(text + 11, 2U, &value.hour) || !digits(text + 14, 2U, &value.minute) ||
      !digits(text + 17, 2U, &value.second) || value.hour > 23U || value.minute > 59U ||
      value.second > 59U)
    return UMI_STATUS_PARSE_ERROR;
  if (text[at] == '.') {
    size_t start = ++at;
    while (at < length && text[at] >= '0' && text[at] <= '9') {
      if (at - start == 9U)
        return UMI_STATUS_PARSE_ERROR;
      value.nanosecond = value.nanosecond * 10U + (uint32_t)(text[at++] - '0');
    }
    if (at == start)
      return UMI_STATUS_PARSE_ERROR;
    for (size_t n = at - start; n < 9U; ++n)
      value.nanosecond *= 10U;
  }
  if (length - at == 1U && text[at] == 'Z') {
    value.offset_minutes = 0;
  } else {
    unsigned hours, minutes;
    if (length - at != 6U || (text[at] != '+' && text[at] != '-') || text[at + 3U] != ':' ||
        !digits(text + at + 1U, 2U, &hours) || !digits(text + at + 4U, 2U, &minutes) ||
        hours > 14U || minutes > 59U || (hours == 14U && minutes != 0U) ||
        (text[at] == '-' && hours == 0U && minutes == 0U))
      return UMI_STATUS_PARSE_ERROR;
    value.offset_minutes = (int)(hours * 60U + minutes);
    if (text[at] == '-')
      value.offset_minutes = -value.offset_minutes;
  }
  *out = value;
  return UMI_STATUS_OK;
}
