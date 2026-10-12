/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_private.h
 * PURPOSE: Keep statement selection identical for arithmetic and exported observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_STATEMENT_PRIVATE_H
#define UMICOM_BANK_STATEMENT_PRIVATE_H
#include "feed_private.h"
#include "umicom/bank_feed/statement.h"
/* Import already validates timestamps. Compare their calendar date deliberately:
 * bank statement dates belong to the provider's local reporting day. */
bool BankStatementInPeriod(const UmiBankFeedRow *row, const UmiBankStatementRequest *request);
bool BankStatementAccount(const UmiBankFeedRow *row, const UmiBankStatementRequest *request);
#endif
