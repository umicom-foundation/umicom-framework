/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/cash_planning/test_portfolio.c
 *
 * PURPOSE:
 *   Check exact dated cash rollups, input revisions and atomic portfolio refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/cash_planning/portfolio.h"
#include "fixture.h"
#undef CHECK
#include "../build_log/fixture.h"

static UmiCashPlan *MakePlan(const char *currency, uint8_t scale, int64_t balance, int64_t buffer)
{
    /* Reuse the shared leap-month range while varying only the reviewed unit. */
    UmiCashPlanConfig config = Config();
    memcpy(config.opening.currency.code, currency, sizeof config.opening.currency.code);
    config.opening.scale = scale;
    config.opening.minor_units = balance;
    config.buffer_minor = buffer;
    UmiCashPlan *plan = NULL;
    OK(UmiCashPlanCreate(&config, &plan));
    return plan;
}

static void AddEntry(UmiCashPlan *plan, const char *id, int day, int64_t amount, int receive, int enabled)
{
    /* The actor chooses an amount in this plan's explicit currency and scale. */
    UmiCashPlanConfig config;
    OK(UmiCashPlanRead(plan, &config));
    UmiCashPlanEntry input = Entry(id, day, amount, receive);
    input.amount.currency = config.opening.currency;
    input.amount.scale = config.opening.scale;
    input.enabled = enabled;
    OK(UmiCashPlanAdd(plan, &input));
}

static void Group(const UmiCashPlanPortfolioRow *actual, const char *currency, uint8_t scale,
    const int64_t values[4], size_t count, size_t shortfalls)
{
    /* Validate every total independently, including its declared unit. */
    const UmiMoney *totals[] = {&actual->opening, &actual->inflow, &actual->outflow, &actual->closing};
    for (size_t index = 0U; index < sizeof totals / sizeof totals[0]; ++index) {
        CHECK(!strcmp(totals[index]->currency.code, currency));
        CHECK(totals[index]->scale == scale && totals[index]->minor_units == values[index]);
    }
    CHECK(actual->plan_count == count && actual->plans_with_shortfalls == shortfalls);
}

static void Refuse(const UmiCashPlan *const *plans, size_t count, UmiStatus expected)
{
    /* A partial calculation must leave all existing result bytes in place. */
    UmiCashPlanPortfolio result, prior;
    memset(&result, 0xA5, sizeof result);
    memcpy(&prior, &result, sizeof prior);
    CHECK(UmiCashPlanProjectPortfolio(plans, count, &result) == expected);
    CHECK(!memcmp(&prior, &result, sizeof result));
}

static void Worked(const char *mode)
{
    UmiCashPlan *a = MakePlan("USD", 2U, 10000, 11000);
    UmiCashPlan *b = MakePlan("USD", 2U, -1000, 0);
    UmiCashPlan *c = MakePlan("EUR", 2U, 4000, 0);
    UmiCashPlan *d = MakePlan("USD", 3U, 1000, 0);
    AddEntry(a, "sales", 1, 4000, 1, 1);
    AddEntry(a, "rent", 4, 2000, 0, 1);
    AddEntry(b, "deposit", 3, 3000, 1, 1);
    AddEntry(b, "fee", 12, 500, 0, 1);
    AddEntry(c, "sale", 29, 500, 1, 1);
    AddEntry(d, "cost", 7, 100, 0, 1);
    if (!strcmp(mode, "revisions")) AddEntry(d, "alternate", 9, 100, 1, 0);

    const UmiCashPlan *normal[] = {a, b, c, d};
    const UmiCashPlan *reverse[] = {d, c, b, a};
    const UmiCashPlan *const *selection = !strcmp(mode, "reordered") ? reverse : normal;
    uint64_t revisions[4];
    for (size_t index = 0U; index < 4U; ++index) revisions[index] = UmiCashPlanRevision(selection[index]);
    UmiCashPlanPortfolio output;
    OK(UmiCashPlanProjectPortfolio(selection, 4U, &output));
    CHECK(output.plan_count == 4U && output.group_count == 3U);
    CHECK(output.start.year == 2028 && output.start.month == 2 && output.start.day == 1);
    CHECK(output.end.year == 2028 && output.end.month == 2 && output.end.day == 29);
    for (size_t index = 0U; index < 4U; ++index) {
        CHECK(output.revisions[index] == revisions[index]);
        CHECK(UmiCashPlanRevision(selection[index]) == revisions[index]);
    }
    /* The published result owns its values even after all plan owners close. */
    UmiCashPlanDestroy(a); UmiCashPlanDestroy(b); UmiCashPlanDestroy(c); UmiCashPlanDestroy(d);
    Group(&output.groups[0], "EUR", 2U, (const int64_t[]){4000, 500, 0, 4500}, 1U, 0U);
    Group(&output.groups[1], "USD", 2U, (const int64_t[]){9000, 7000, 2500, 13500}, 2U, 2U);
    Group(&output.groups[2], "USD", 3U, (const int64_t[]){1000, 0, 100, 900}, 1U, 0U);
}

static void CheckMaximum(void)
{
    /* The bound accepts every distinct group and sorts reverse-order inputs. */
    static const char *const currency[] = {"USD", "SEK", "NZD", "NOK", "MXN", "KRW", "JPY", "INR",
        "HKD", "GBP", "EUR", "DKK", "CNY", "CHF", "CAD", "AUD"};
    UmiCashPlan *owners[UMI_CASH_PLAN_PORTFOLIO_CAPACITY];
    const UmiCashPlan *plans[UMI_CASH_PLAN_PORTFOLIO_CAPACITY];
    static_assert(sizeof currency / sizeof currency[0] == UMI_CASH_PLAN_PORTFOLIO_CAPACITY);
    for (size_t index = 0U; index < UMI_CASH_PLAN_PORTFOLIO_CAPACITY; ++index) {
        owners[index] = MakePlan(currency[index], 2U, 50, 0);
        plans[index] = owners[index];
    }
    UmiCashPlanPortfolio output;
    OK(UmiCashPlanProjectPortfolio(plans, UMI_CASH_PLAN_PORTFOLIO_CAPACITY, &output));
    CHECK(output.plan_count == UMI_CASH_PLAN_PORTFOLIO_CAPACITY && output.group_count == output.plan_count);
    for (size_t index = 0U; index < UMI_CASH_PLAN_PORTFOLIO_CAPACITY; ++index) {
        Group(&output.groups[index], currency[UMI_CASH_PLAN_PORTFOLIO_CAPACITY - 1U - index], 2U,
            (const int64_t[]){50, 0, 0, 50}, 1U, 0U);
        UmiCashPlanDestroy(owners[index]);
    }
}

static void CheckReopened(void)
{
    /* Exercise the existing explicit save/load owner. The portfolio consumes
     * reloaded plans and leaves both saved source documents byte-for-byte intact. */
    char directory[UMI_PATH_CAPACITY], sterling[UMI_PATH_CAPACITY], euros[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(sterling, directory, "cash-\xC3\xA9-gbp.json");
    FixturePath(euros, directory, "cash-eur.json");
    UmiCashPlan *a = MakePlan("GBP", 2U, 10000, 0);
    UmiCashPlan *b = MakePlan("EUR", 2U, 2000, 0);
    AddEntry(a, "cost", 2, 100, 0, 1);
    AddEntry(b, "receipt", 29, 400, 1, 1);
    UmiOutputFileSnapshot receipt;
    OK(UmiCashPlanSaveNew(sterling, a, &receipt));
    OK(UmiCashPlanSaveNew(euros, b, &receipt));
    UmiCashPlanDestroy(a); UmiCashPlanDestroy(b);

    size_t gbpSize = 0U, eurSize = 0U;
    unsigned char *gbpBytes = FixtureRead(sterling, &gbpSize);
    unsigned char *eurBytes = FixtureRead(euros, &eurSize);
    UmiCashPlan *loadedGbp = NULL, *loadedEur = NULL;
    OK(UmiCashPlanLoad(sterling, &loadedGbp));
    OK(UmiCashPlanLoad(euros, &loadedEur));
    const UmiCashPlan *input[] = {loadedGbp, loadedEur};
    UmiCashPlanPortfolio output;
    OK(UmiCashPlanProjectPortfolio(input, 2U, &output));
    CHECK(output.plan_count == 2U && output.group_count == 2U);
    CHECK(output.revisions[0] == UmiCashPlanRevision(loadedGbp) && output.revisions[1] == UmiCashPlanRevision(loadedEur));
    Group(&output.groups[0], "EUR", 2U, (const int64_t[]){2000, 400, 0, 2400}, 1U, 0U);
    Group(&output.groups[1], "GBP", 2U, (const int64_t[]){10000, 0, 100, 9900}, 1U, 0U);
    size_t afterSize = 0U;
    unsigned char *after = FixtureRead(sterling, &afterSize);
    CHECK(afterSize == gbpSize && memcmp(after, gbpBytes, gbpSize) == 0);
    free(after);
    after = FixtureRead(euros, &afterSize);
    CHECK(afterSize == eurSize && memcmp(after, eurBytes, eurSize) == 0);
    free(after);
    free(gbpBytes); free(eurBytes);
    UmiCashPlanDestroy(loadedGbp); UmiCashPlanDestroy(loadedEur);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (!strcmp(mode, "groups") || !strcmp(mode, "reordered") || !strcmp(mode, "revisions")) {
        Worked(mode);
        return 0;
    }
    if (!strcmp(mode, "maximum")) {
        CheckMaximum();
        return 0;
    }
    if (!strcmp(mode, "reopened")) {
        CheckReopened();
        return 0;
    }

    UmiCashPlan *a = MakePlan("GBP", 2U, 100, 0);
    UmiCashPlan *b = MakePlan("GBP", 2U, 100, 0);
    const UmiCashPlan *input[] = {a, b};
    /* Each refusal has its own named case. Boundary and shape errors occur
     * before casting any partial group into the prior caller result. */
    if (!strcmp(mode, "duplicate")) {
        input[1] = a;
        Refuse(input, 2U, UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "different-start") || !strcmp(mode, "different-end")) {
        UmiCashPlanConfig config = Config();
        if (!strcmp(mode, "different-start")) config.start.day = 2U;
        else config.end.day = 28U;
        OK(UmiCashPlanSetConfig(b, &config));
        Refuse(input, 2U, UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "projection-overflow") || !strcmp(mode, "group-overflow")) {
        UmiCashPlanConfig config = Config();
        config.opening.minor_units = INT64_MAX;
        config.buffer_minor = 0;
        OK(UmiCashPlanSetConfig(b, &config));
        if (!strcmp(mode, "projection-overflow")) AddEntry(b, "overflow", 2, 1, 1, 1);
        Refuse(input, 2U, UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(mode, "capacity")) {
        Refuse(input, UMI_CASH_PLAN_PORTFOLIO_CAPACITY + 1U, UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(mode, "null")) {
        Refuse(NULL, 2U, UMI_STATUS_INVALID_ARGUMENT);
        Refuse(input, 0U, UMI_STATUS_INVALID_ARGUMENT);
        input[1] = NULL;
        Refuse(input, 2U, UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCashPlanProjectPortfolio(input, 1U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
        UmiCashPlanPortfolio output;
        if (!strcmp(mode, "disabled")) AddEntry(a, "inactive", 9, 9000, 0, 0);
        else if (!strcmp(mode, "negative")) AddEntry(a, "debit", 2, 150, 0, 1);
        else CHECK(!strcmp(mode, "one"));
        OK(UmiCashPlanProjectPortfolio(input, 1U, &output));
        CHECK(output.plan_count == 1U && output.group_count == 1U);
        if (!strcmp(mode, "negative"))
            Group(&output.groups[0], "GBP", 2U, (const int64_t[]){100, 0, 150, -50}, 1U, 1U);
        else Group(&output.groups[0], "GBP", 2U, (const int64_t[]){100, 0, 0, 100}, 1U, 0U);
    }
    UmiCashPlanDestroy(b); UmiCashPlanDestroy(a);
    return 0;
}
