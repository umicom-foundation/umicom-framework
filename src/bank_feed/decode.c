/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/decode.c
 * PURPOSE: Import selected bank transaction fields with strict types and bounded ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "feed_private.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus FeedObject(const UmiJsonTree *tree, int parent, const char *key, int *out) {
  UmiStatus status = UmiJsonTreeMember(tree, parent, key, out);
  if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, *out) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  return status;
}
/* JSON validates UTF-8; count decoded code points here because bank field
 * limits describe characters, while C storage is bounded in bytes. */
static UmiStatus FeedText(const UmiJsonTree *tree, int object, const char *key, char *out,
                          size_t capacity, size_t characters, bool optional) {
  int node;
  UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
  if (optional && status == UMI_STATUS_NOT_FOUND)
    return UMI_STATUS_OK;
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
    return UMI_STATUS_PARSE_ERROR;
  status = UmiJsonTreeText(tree, node, out, capacity);
  if (status != UMI_STATUS_OK)
    return status;
  size_t count = 0U;
  for (const unsigned char *p = (const unsigned char *)out; *p; ++p) {
    if (*p < 32U || *p == 127U)
      return UMI_STATUS_PARSE_ERROR;
    if ((*p & 0xc0U) != 0x80U)
      ++count;
  }
  if (count > characters)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  return count != 0U || optional ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}
/* Parse the bank amount grammar before the shared decimal parser. This keeps
 * exponent notation and rounding out of imported financial observations. */
static UmiStatus FeedAmount(UmiBankFeedRow *row) {
  size_t at = 0U;
  while (row->amountText[at] >= '0' && row->amountText[at] <= '9')
    ++at;
  if (at == 0U || at > 13U)
    return UMI_STATUS_PARSE_ERROR;
  uint8_t scale = 0U;
  if (row->amountText[at] == '.') {
    size_t begin = ++at;
    while (row->amountText[at] >= '0' && row->amountText[at] <= '9')
      ++at;
    if (at == begin || at - begin > 5U)
      return UMI_STATUS_PARSE_ERROR;
    scale = (uint8_t)(at - begin);
  }
  if (row->amountText[at] != '\0' || strlen(row->currency) != 3U)
    return UMI_STATUS_PARSE_ERROR;
  for (size_t i = 0; i < 3U; ++i)
    if (row->currency[i] < 'A' || row->currency[i] > 'Z')
      return UMI_STATUS_PARSE_ERROR;
  return UmiDecimalParse(row->amountText, at, scale, &row->amount);
}
/* Select only fields this reader understands. New provider mappings should
 * translate into these owned values rather than bypass their validation. */
static UmiStatus FeedRow(const UmiJsonTree *tree, int node, UmiBankFeedRow *row) {
  if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
#define FEED_TEXT(key, field, max, optional)                                                       \
  do {                                                                                             \
    UmiStatus fieldStatus =                                                                        \
        FeedText(tree, node, key, row->field, sizeof row->field, max, optional);                   \
    if (fieldStatus != UMI_STATUS_OK)                                                              \
      return fieldStatus;                                                                          \
  } while (0)
  FEED_TEXT("AccountId", account, 40U, false);
  FEED_TEXT("TransactionId", transaction, 210U, true);
  FEED_TEXT("TransactionReference", reference, 210U, true);
  FEED_TEXT("TransactionInformation", information, 500U, true);
  FEED_TEXT("BookingDateTime", bookingTime, 39U, false);
#undef FEED_TEXT
  if (!BankFeedTimestamp(row->bookingTime))
    return UMI_STATUS_PARSE_ERROR;
  char direction[16] = {0}, state[16] = {0}, mutability[16] = {0};
  UmiStatus status =
      FeedText(tree, node, "CreditDebitIndicator", direction, sizeof direction, 15U, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(direction, "Credit") == 0)
    row->credit = true;
  else if (strcmp(direction, "Debit") != 0)
    return UMI_STATUS_PARSE_ERROR;
  status = FeedText(tree, node, "Status", state, sizeof state, 15U, false);
  if (status != UMI_STATUS_OK)
    return status;
  if (strcmp(state, "BOOK") == 0 || strcmp(state, "Booked") == 0)
    row->booked = true;
  else if (strcmp(state, "PDNG") != 0 && strcmp(state, "Pending") != 0)
    return UMI_STATUS_NOT_IMPLEMENTED;
  status = FeedText(tree, node, "TransactionMutability", mutability, sizeof mutability, 15U, true);
  if (status != UMI_STATUS_OK)
    return status;
  if (mutability[0] && strcmp(mutability, "Mutable") != 0 && strcmp(mutability, "Immutable") != 0)
    return UMI_STATUS_PARSE_ERROR;
  row->mutableRecord = !row->booked || strcmp(mutability, "Mutable") == 0;
  int amount;
  status = FeedObject(tree, node, "Amount", &amount);
  if (status == UMI_STATUS_OK)
    status = FeedText(tree, amount, "Amount", row->amountText, sizeof row->amountText, 23U, false);
  if (status == UMI_STATUS_OK)
    status = FeedText(tree, amount, "Currency", row->currency, sizeof row->currency, 3U, false);
  return status == UMI_STATUS_OK ? FeedAmount(row) : status;
}
static UmiStatus FeedPagination(const UmiJsonTree *tree, UmiBankFeed *feed) {
  int links;
  UmiStatus status = UmiJsonTreeMember(tree, 0, "Links", &links);
  if (status == UMI_STATUS_NOT_FOUND)
    return UMI_STATUS_OK;
  if (status != UMI_STATUS_OK)
    return status;
  if (UmiJsonTreeKind(tree, links) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
    return UMI_STATUS_PARSE_ERROR;
  const char *names[] = {"Prev", "Next"};
  bool *flags[] = {&feed->summary.hasPreviousPage, &feed->summary.hasNextPage};
  for (size_t i = 0; i < 2U; ++i) {
    char value[4097] = {0};
    status = FeedText(tree, links, names[i], value, sizeof value, 4096U, true);
    if (status != UMI_STATUS_OK)
      return status;
    *flags[i] = value[0] != '\0';
  }
  /* Links are only completeness hints. Never retain or follow an imported URL;
   * a future authenticated adapter must apply its own origin and consent rules. */
  return UMI_STATUS_OK;
}
/* Build an isolated page and publish it only after every selected field has
 * passed validation. Callers never receive a partially decoded transaction set. */
UmiStatus UmiBankFeedCreate(const void *bytes, size_t length, UmiBankFeed **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (bytes == NULL || length == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (length > UMI_BANK_FEED_MAX_BYTES)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiJsonTreeLimits limits = {UMI_BANK_FEED_MAX_BYTES, 65536U, 24U};
  UmiJsonTree *tree = NULL;
  UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, NULL, &tree);
  if (status != UMI_STATUS_OK)
    return status;
  UmiBankFeed *feed = calloc(1U, sizeof *feed);
  if (feed == NULL) {
    UmiJsonTreeDestroy(tree);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  int data, array = -1;
  status = FeedObject(tree, 0, "Data", &data);
  if (status == UMI_STATUS_OK) {
    status = UmiJsonTreeMember(tree, data, "Transaction", &array);
    /* No transactions is a legitimate empty page, not a zero account balance. */
    if (status == UMI_STATUS_NOT_FOUND)
      status = UMI_STATUS_OK;
    else if (status == UMI_STATUS_OK &&
             UmiJsonTreeKind(tree, array) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
      status = UMI_STATUS_PARSE_ERROR;
  }
  size_t count = array >= 0 ? UmiJsonTreeCount(tree, array) : 0U;
  if (status == UMI_STATUS_OK && count > UMI_BANK_FEED_MAX_ROWS)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  if (status == UMI_STATUS_OK && count != 0U) {
    feed->rows = calloc(count, sizeof *feed->rows);
    if (feed->rows == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  for (int node = array >= 0 ? UmiJsonTreeFirst(tree, array) : -1;
       status == UMI_STATUS_OK && node >= 0; node = UmiJsonTreeNext(tree, node)) {
    UmiBankFeedRow *row = &feed->rows[feed->summary.rows];
    status = FeedRow(tree, node, row);
    if (status != UMI_STATUS_OK)
      break;
    if (row->transaction[0] == '\0')
      ++feed->summary.missingIdentifiers;
    else
      for (size_t i = 0; i < feed->summary.rows; ++i)
        if (strcmp(row->account, feed->rows[i].account) == 0 &&
            strcmp(row->transaction, feed->rows[i].transaction) == 0) {
          status = UMI_STATUS_ALREADY_EXISTS;
          break;
        }
    ++feed->summary.rows;
  }
  if (status == UMI_STATUS_OK)
    status = FeedPagination(tree, feed);
  UmiJsonTreeDestroy(tree);
  if (status == UMI_STATUS_OK)
    *out = feed;
  else
    UmiBankFeedDestroy(feed);
  return status;
}
