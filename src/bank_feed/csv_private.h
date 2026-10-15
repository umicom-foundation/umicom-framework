/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/csv_private.h
 * PURPOSE: Share statement-row validation between fixed and mapped CSV readers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_CSV_PRIVATE_H
#define UMICOM_BANK_FEED_CSV_PRIVATE_H
#include "feed_private.h"
#include "umicom/base/csv_reader.h"
#include "umicom/bank_feed/csv_import.h"
/* The caller owns an unpublished feed with MAX_ROWS slots. This routine adds
 * exactly one validated normalised row or leaves the published count unchanged.
 * The issue column refers to the normalised schema; mappers translate it back. */
UmiStatus BankFeedCsvAppend(UmiBankFeed *feed, const UmiCsvReadRow *source,
                            UmiBankFeedCsvIssue *issue);
#endif
