/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/bank_feed/statement_timeline_private.h
 * PURPOSE: Own copied statement scope and bounded daily rows behind the public API.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_STATEMENT_TIMELINE_PRIVATE_H
#define UMICOM_BANK_STATEMENT_TIMELINE_PRIVATE_H
#include "umicom/bank_feed/statement_timeline.h"
struct UmiBankStatementTimeline {
    char account[161], currency[4];
    UmiFinancialDate first, last;
    UmiDecimal opening, reportedClosing;
    UmiBankStatementTimelineSummary summary;
    UmiBankStatementDay days[UMI_BANK_FEED_MAX_ROWS];
};
#endif
