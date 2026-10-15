/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/csv_fixture.h
 * PURPOSE: Provide invented CSV observations shared by importer and UI regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_FEED_CSV_FIXTURE_H
#define UMICOM_BANK_FEED_CSV_FIXTURE_H
#define BANK_CSV_HEADER \
    "account,transaction,reference,information,booking_date,currency,amount,status,mutability\n"
#define BANK_CSV_CREDIT \
    "practice-account,credit-one,ref,Example credit,2026-10-12,GBP,12.50,BOOK,Immutable\n"
#define BANK_CSV_DEBIT \
    "practice-account,debit-one,,Example debit,2026-10-12,GBP,-2.25,BOOK,Immutable\n"
#define BANK_CSV_PENDING \
    "practice-account,pending-one,,Pending debit,2026-10-12,GBP,-999.00,PDNG,Mutable\n"
#define BANK_CSV_TEXT BANK_CSV_HEADER BANK_CSV_CREDIT BANK_CSV_DEBIT BANK_CSV_PENDING
#endif
