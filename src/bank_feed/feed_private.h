/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/feed_private.h
 * PURPOSE: Share bounded import ownership between decoding and reporting.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_PRIVATE_H
#define UMICOM_BANK_FEED_PRIVATE_H
#include "umicom/bank_feed/transactions.h"
struct UmiBankFeed {
  UmiBankFeedSummary summary;
  UmiBankFeedRow *rows;
};
/* Keep timestamp parsing private until another consumer needs a broader date
 * contract. This validates source spelling; it never applies a local timezone. */
bool BankFeedTimestamp(const char *text);
#endif
