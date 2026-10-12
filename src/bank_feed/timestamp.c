/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/timestamp.c
 * PURPOSE: Validate bank booking timestamps without timezone-dependent conversion.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "feed_private.h"
#include "umicom/finance/core/types.h"
#include <string.h>
/* Preserve the provider timezone spelling while validating its components.
 * No host timezone or locale participates in accepting a booking timestamp. */
bool BankFeedTimestamp(const char *text) {
  size_t size = strlen(text);
  if (size < 20U || text[4] != '-' || text[7] != '-' || text[10] != 'T' || text[13] != ':' ||
      text[16] != ':')
    return false;
  const size_t starts[] = {0U, 5U, 8U, 11U, 14U, 17U}, widths[] = {4U, 2U, 2U, 2U, 2U, 2U};
  unsigned values[6] = {0};
  for (size_t i = 0; i < 6U; ++i)
    for (size_t j = 0; j < widths[i]; ++j) {
      char digit = text[starts[i] + j];
      if (digit < '0' || digit > '9')
        return false;
      values[i] = values[i] * 10U + (unsigned)(digit - '0');
    }
  UmiFinancialDate date = {(int32_t)values[0], (uint8_t)values[1], (uint8_t)values[2]};
  if (!umi_financial_date_is_valid(date) || values[3] > 23U || values[4] > 59U || values[5] > 59U)
    return false;
  size_t at = 19U;
  if (text[at] == '.') {
    size_t begin = ++at;
    while (text[at] >= '0' && text[at] <= '9')
      ++at;
    if (at == begin || at - begin > 9U)
      return false;
  }
  if (strcmp(text + at, "Z") == 0)
    return true;
  if (size - at != 6U || (text[at] != '+' && text[at] != '-') || text[at + 3U] != ':')
    return false;
  const size_t digits[] = {1U, 2U, 4U, 5U};
  for (size_t i = 0; i < 4U; ++i)
    if (text[at + digits[i]] < '0' || text[at + digits[i]] > '9')
      return false;
  unsigned hours = (unsigned)(text[at + 1U] - '0') * 10U + (unsigned)(text[at + 2U] - '0');
  unsigned minutes = (unsigned)(text[at + 4U] - '0') * 10U + (unsigned)(text[at + 5U] - '0');
  return hours <= 14U && minutes < 60U && (hours != 14U || minutes == 0U);
}
