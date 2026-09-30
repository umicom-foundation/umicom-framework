/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_interest/fixture.h
 * PURPOSE: Share a bounded practice ledger fixture across interest regression cases.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BANK_INTEREST_TEST_FIXTURE_H
#define UMICOM_BANK_INTEREST_TEST_FIXTURE_H
#include "umicom/bank_operations/review.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
typedef struct Fixture { UmiBankOperations *bank; uint64_t serial; UmiBankActor maker, checker, operator; } Fixture;
static inline void Id(UmiFinancialId *id, const char *text) { OK(umi_financial_id_assign(id, text)); }
static inline UmiMoney Cash(int64_t amount) { UmiMoney value = {0}; memcpy(value.currency.code, "GBP", 4); value.scale = 2; value.minor_units = amount; return value; }
static inline UmiBankCommand Make(Fixture *f, UmiBankAction action, const char *id)
{
    UmiBankCommand c; UmiBankCounts n; OK(UmiBankOperationsCounts(f->bank, &n));
    UmiBankCommandInit(&c, action); Id(&c.id, id);
    (void)snprintf(c.requestId.value, sizeof c.requestId.value, "interest-request-%" PRIu64, ++f->serial);
    c.expectedRevision = n.revision; c.businessDate = (UmiFinancialDate){2026,9,30}; c.timestampMillis = (int64_t)f->serial;
    return c;
}
static inline void Send(Fixture *f, const UmiBankActor *actor, const UmiBankCommand *c)
{ UmiBankReceipt receipt; OK(UmiBankOperationsExecute(f->bank, actor, c, &receipt)); CHECK(!receipt.idempotent); }
static inline void Setup(Fixture *f)
{
    UmiBankCommand c;
    Id(&f->maker.id, "maker"); f->maker.capabilities = UMI_BANK_CAP_CUSTOMERS | UMI_BANK_CAP_PAYMENTS;
    Id(&f->checker.id, "checker"); f->checker.capabilities = UMI_BANK_CAP_APPROVE;
    Id(&f->operator.id, "operator"); f->operator.capabilities = UMI_BANK_CAP_OPERATE | UMI_BANK_CAP_TEST_FUNDING;
    c = Make(f, UMI_BANK_CUSTOMER_CREATE, "customer"); strcpy(c.name, "Interest lesson"); Send(f, &f->maker, &c);
    c = Make(f, UMI_BANK_ACCOUNT_OPEN, "account"); Id(&c.ownerId, "customer"); strcpy(c.name, "Practice savings"); c.amount = Cash(0); Send(f, &f->maker, &c);
    c = Make(f, UMI_BANK_TEST_CREDIT, "funding"); Id(&c.sourceAccountId, "account"); c.amount = Cash(100000); Send(f, &f->operator, &c);
}
static inline UmiBankCommand Interest(Fixture *f, const char *id, const char *period)
{
    UmiBankCommand c = Make(f, UMI_BANK_INTEREST_SUBMIT, id);
    Id(&c.ownerId, period); Id(&c.sourceAccountId, "account"); c.interest = (UmiBankInterestTerms){500,30,365}; return c;
}
static inline void Step(Fixture *f, UmiBankAction action, const UmiBankActor *actor)
{ UmiBankCommand c = Make(f, action, "interest"); Send(f, actor, &c); }
static inline void Balance(Fixture *f, int64_t booked, int64_t reserved)
{ UmiBankBalance b; OK(UmiBankOperationsBalance(f->bank, "account", &b)); CHECK(b.booked.minor_units == booked && b.reserved.minor_units == reserved && b.available.minor_units == booked - reserved); }
static inline void Fail(Fixture *f, const UmiBankActor *actor, UmiBankCommand c, UmiStatus expected)
{
    UmiBankCounts before, after; UmiBankReceipt receipt; UmiBankBalance funds, actual; UmiBankReview *review = NULL;
    OK(UmiBankOperationsCounts(f->bank, &before)); OK(UmiBankOperationsBalance(f->bank, "account", &funds));
    CHECK(UmiBankOperationsReview(f->bank, actor, &c, &review) == expected && review == NULL);
    CHECK(UmiBankOperationsExecute(f->bank, actor, &c, &receipt) == expected && receipt.revision == 0);
    OK(UmiBankOperationsCounts(f->bank, &after)); OK(UmiBankOperationsBalance(f->bank, "account", &actual));
    CHECK(before.revision == after.revision && before.journals == after.journals && before.interestRequests == after.interestRequests);
    CHECK(funds.booked.minor_units == actual.booked.minor_units && funds.reserved.minor_units == actual.reserved.minor_units);
}
#endif
