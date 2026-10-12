/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/fixture.h
 * PURPOSE: Provide invented transaction data for bank import regression checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_TEST_FIXTURE_H
#define UMICOM_BANK_FEED_TEST_FIXTURE_H
#include "umicom/bank_feed/transactions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
  do {                                                                                             \
    if (!(value)) {                                                                                \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                                  \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#define BANK_TEST_ROW                                                                              \
  "{\"AccountId\":\"practice-account\",\"TransactionId\":\"credit-one\","                          \
  "\"CreditDebitIndicator\":\"Credit\",\"Status\":\"BOOK\",\"BookingDateTime\":\"2026-10-12T10:"   \
  "00:00Z\","                                                                                      \
  "\"Amount\":{\"Amount\":\"12.50\",\"Currency\":\"GBP\"},\"TransactionInformation\":\"Example "   \
  "credit\"}"
static inline char *BankFixture(const char *rows) {
  size_t bytes = strlen(rows) + 64U;
  char *text = malloc(bytes);
  if (text != NULL)
    (void)snprintf(text, bytes, "{\"Data\":{\"Transaction\":[%s]}}", rows);
  return text;
}
static inline char *BankReplace(const char *source, const char *find, const char *replacement) {
  const char *at = strstr(source, find);
  if (at == NULL)
    return NULL;
  size_t bytes = strlen(source) - strlen(find) + strlen(replacement) + 1U;
  char *text = malloc(bytes);
  if (text == NULL)
    return NULL;
  size_t prefix = (size_t)(at - source);
  memcpy(text, source, prefix);
  memcpy(text + prefix, replacement, strlen(replacement));
  strcpy(text + prefix + strlen(replacement), at + strlen(find));
  return text;
}
#endif
