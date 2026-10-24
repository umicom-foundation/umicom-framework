/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_temporal.h
 * PURPOSE: Validate calendar dates and explicit-offset timestamps for forms.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_TEMPORAL_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_TEMPORAL_H
#include "umicom/base/status.h"
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/* These components describe the supplied local clock and its explicit UTC
 * offset. No host timezone, daylight-saving guess or locale is consulted. */
typedef struct UmiFormTimestamp {
  unsigned year, month, day, hour, minute, second;
  uint32_t nanosecond;
  int offset_minutes;
} UmiFormTimestamp;
/* Exact YYYY-MM-DD, Gregorian years 0001..9999. The input need not be terminated. */
UmiStatus umi_form_date_validate(const char *text, size_t length);
/* YYYY-MM-DDTHH:MM:SS[.1..9 digits](Z|+HH:MM|-HH:MM). Offset magnitude is at
 * most 14:00. Unknown -00:00 and leap-second spelling are refused. Output is
 * unchanged on failure. Applications retain the original text as provenance. */
UmiStatus umi_form_timestamp_parse(const char *text, size_t length, UmiFormTimestamp *out);
#ifdef __cplusplus
}
#endif
#endif
