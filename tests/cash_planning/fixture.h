/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/fixture.h
 * PURPOSE: Share explicit cash assumptions for meaningful projection and document checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CASH_PLANNING_TEST_FIXTURE_H
#define UMICOM_CASH_PLANNING_TEST_FIXTURE_H
#include "umicom/cash_planning/document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static inline UmiCashPlanConfig Config(void)
{
    return (UmiCashPlanConfig){.title = "Household cash",
                               .start = {2028, 2, 1},
                               .end = {2028, 2, 29},
                               .opening = {100000, 2, {"GBP"}},
                               .buffer_minor = 20000};
}
static inline UmiCashPlan *Plan(void)
{
    UmiCashPlan *p = NULL;
    UmiCashPlanConfig c = Config();
    OK(UmiCashPlanCreate(&c, &p));
    return p;
}
static inline UmiCashPlanEntry Entry(const char *id, int day, int64_t amount, int receive)
{
    UmiCashPlanEntry e = {.date = {2028, 2, (uint8_t)day},
                          .amount = {amount, 2, {"GBP"}},
                          .direction =
                              receive ? UMI_FINANCIAL_DIRECTION_RECEIVE : UMI_FINANCIAL_DIRECTION_PAY,
                          .enabled = 1};
    (void)snprintf(e.id, sizeof e.id, "%s", id);
    (void)snprintf(e.label, sizeof e.label, "Payment %s", id);
    return e;
}
static inline void Add(UmiCashPlan *p, const char *id, int day, int64_t amount, int receive)
{
    UmiCashPlanEntry e = Entry(id, day, amount, receive);
    OK(UmiCashPlanAdd(p, &e));
}
static inline char *Encode(UmiCashPlan *p, size_t *n)
{
    char *b = NULL;
    OK(UmiCashPlanEncode(p, &b, n));
    return b;
}
static inline void Same(UmiCashPlan *a, UmiCashPlan *b)
{
    size_t an = 0U, bn = 0U;
    char *ab = Encode(a, &an), *bb = Encode(b, &bn);
    CHECK(an == bn && memcmp(ab, bb, an) == 0);
    UmiCashPlanFreeBytes(ab);
    UmiCashPlanFreeBytes(bb);
}
#endif
