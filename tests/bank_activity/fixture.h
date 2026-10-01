/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_activity/fixture.h
 * PURPOSE: Build real postings with backdated reversal and a separate reservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_ACTIVITY_TEST_FIXTURE_H
#define UMICOM_BANK_ACTIVITY_TEST_FIXTURE_H
#include "../bank_interest/fixture.h"
#include "umicom/bank_operations/activity.h"
static inline UmiBankActivityQuery ActivityQuery(void)
{ UmiBankActivityQuery query = {0}; Id(&query.accountId, "account"); return query; }
static inline void ActivitySetup(Fixture *f)
{
    Setup(f);
    UmiBankCommand c = Make(f, UMI_BANK_TEST_CREDIT, "extra"); Id(&c.sourceAccountId, "account");
    c.amount = Cash(500); c.businessDate = (UmiFinancialDate){2026,10,1}; Send(f, &f->operator, &c);
    c = Make(f, UMI_BANK_CHARGE_SUBMIT, "charge"); Id(&c.sourceAccountId, "account"); Id(&c.ownerId, "monthly-charge");
    strcpy(c.name, "Practice charge"); c.amount = Cash(2500); Send(f, &f->maker, &c);
    c = Make(f, UMI_BANK_CHARGE_APPROVE, "charge"); Send(f, &f->checker, &c);
    c = Make(f, UMI_BANK_CHARGE_POST, "charge"); c.businessDate = (UmiFinancialDate){2026,10,2}; Send(f, &f->operator, &c);
    c = Make(f, UMI_BANK_CHARGE_REVERSE, "charge"); c.businessDate = (UmiFinancialDate){2026,9,29}; Send(f, &f->operator, &c);
    c = Make(f, UMI_BANK_HOLD_PLACE, "reservation"); Id(&c.sourceAccountId, "account"); c.amount = Cash(1000); Send(f, &f->operator, &c);
    Balance(f, 100500, 1000);
}
static inline UmiBankActivity *ActivityCapture(Fixture *f, UmiBankActivityQuery query)
{ UmiBankActivity *report = NULL; OK(UmiBankActivityCapture(f->bank, &query, &report)); CHECK(report != NULL); return report; }
#endif
